///
/// @file Library/FileLib/FileLib.c
///
/// File library entry
///

#include <Library/FileLib.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Guid/Gpt.h>
#include <Guid/FileSystemInfo.h>
#include <Guid/FileSystemVolumeLabelInfo.h>

#include <Protocol/LoadedImage.h>

// FILE_GET_ROOT_VOLUME
/// Get the boot file system volume (from where this executable was loaded)
/// @return The boot volume, which must be freed, or NULL if there was an error
typedef FILE_VOLUME *
(EFIAPI
*FILE_GET_ROOT_VOLUME) (
  VOID
);
// FILE_GET_VOLUMES
/// Get list of file system volumes
/// @param Volumes On output, the current volumes list, which needs freed
/// @param Count   On output, the count of volumes returned
/// @return Whether the list of volumes was returned or not
/// @retval EFI_INVALID_PARAMETER If Volumes or Count is NULL
/// @retval EFI_OUT_OF_RESOURCES  The memory for the volumes could not be allocated
/// @retval EFI_SUCCESS           The list of volumes was returned successfully
typedef EFI_STATUS
(EFIAPI
*FILE_GET_VOLUMES) (
  OUT FILE_VOLUME **Volumes,
  OUT UINTN        *Count
);
// FILE_FIND_VOLUMES
/// Find volumes
/// @param Volumes On output, the matched volumes, which needs to be freed
/// @param Count   On output, the count of matched volumes
/// @param Match   The string to use in matching the volumes
/// @param Options The matching options
/// @return Whether any matching volumes were found and returned
/// @retval EFI_INVALID_PARAMETER If Volumes, Count or Match is NULL or Options is zero or invalid
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the volumes information
/// @retval EFI_NOT_FOUND         If no volumes matched the given string and options
/// @retval EFI_SUCCESS           If the volumes and count were returned successfully
typedef EFI_STATUS
(EFIAPI
*FILE_FIND_VOLUMES) (
  OUT FILE_VOLUME **Volumes,
  OUT UINTN        *Count,
  IN  CHAR16       *Match,
  IN  UINT32        Options
);
// FILE_FIND_VOLUME
/// Find volume by device handle
/// @param Volume On output, the volume, which needs to be freed
/// @param Device The device handle of the volume
/// @return Whether the volume was found and returned
/// @retval EFI_INVALID_PARAMETER If Volume or Device is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the volume information
/// @retval EFI_NOT_FOUND         If no volume was found
/// @retval EFI_SUCCESS           If the volume was returned successfully
typedef EFI_STATUS
(EFIAPI
*FILE_FIND_VOLUME) (
  OUT FILE_VOLUME **Volume,
  IN  EFI_HANDLE    Device
);

// FILE_VOLUME_PROTOCOL
/// File system volume protocol
typedef struct _FILE_VOLUME_PROTOCOL FILE_VOLUME_PROTOCOL;
struct _FILE_VOLUME_PROTOCOL {

  // GetBootVolume
  /// Get the boot file system volume
  FILE_GET_ROOT_VOLUME GetBootVolume;
  // GetVolumes
  /// Get the file system volume
  FILE_GET_VOLUMES     GetVolumes;
  // FindVolumes
  /// Find volumes
  FILE_FIND_VOLUMES    FindVolumes;
  // FindVolume
  /// Find volume
  FILE_FIND_VOLUME     FindVolume;

};
// FILE_VOLUME_INFO
/// File system volume information
typedef struct _FILE_VOLUME_INFO FILE_VOLUME_INFO;
struct _FILE_VOLUME_INFO {

  // Next
  /// The next file system volume information in this list
  FILE_VOLUME_INFO *Next;
  // Information
  /// The file system volume information
  FILE_VOLUME       Information;

};
// FILE_VOLUME_LIST
/// File system volume linked list
typedef struct _FILE_VOLUME_LIST FILE_VOLUME_LIST;
struct _FILE_VOLUME_LIST {

  // Next
  /// The next file system volume in this list
  FILE_VOLUME_LIST *Next;
  // Volume
  /// File system volume information
  FILE_VOLUME_INFO *Volume;

};

// mFileVolumeGuid
/// File system volume protocol GUID
STATIC EFI_GUID              mFileVolumeGuid = { 0xC93CF4BA, 0x7D26, 0x4C2F, { 0x9C, 0x84, 0xD5, 0x09, 0x18, 0xF0, 0xD5, 0xD5 } };
// mFileProtocol
/// Installed file system volume protocol
STATIC FILE_VOLUME_PROTOCOL *mFileProtocol = NULL;
// mFileVolumes
/// File system volumes information linked list
STATIC FILE_VOLUME_INFO     *mFileVolumes = NULL;
// mFileBootVolume
/// Boot file system volume
STATIC FILE_VOLUME           mFileBootVolume = { 0 };
// mFileRefreshEvent
/// File system volume notification refresh event
STATIC EFI_EVENT             mFileRefreshEvent = NULL;
// mFileNotifyEvent
/// File system volume notification event
STATIC EFI_EVENT             mFileNotifyEvent = NULL;
// mFileNotifyRegistration
/// File system volume notification registration
STATIC VOID                 *mFileNotifyRegistration = NULL;

// FileAddVolumeList
/// Add to a file system volume list
/// @param List The file system volume list
/// @param Add  The file system volume to add
/// @return The new head of the file system list
STATIC FILE_VOLUME_LIST *
EFIAPI
FileAddVolumeList (
  IN FILE_VOLUME_LIST *List,
  IN FILE_VOLUME_INFO *Add
) {
  FILE_VOLUME_LIST *Volume;
  FILE_VOLUME_LIST *Ptr;
  // Check if adding a new node
  if (Add == NULL) {
    return List;
  }
  // Allocate new list node
  Volume = (FILE_VOLUME_LIST *)AllocateZeroPool(sizeof(FILE_VOLUME_LIST));
  if (Volume == NULL) {
    return List;
  }
  // Set list members
  Volume->Next = NULL;
  Volume->Volume = Add;
  // Check if this is head
  if (List == NULL) {
    return Volume;
  }
  // Append to end of list
  for (Ptr = List; Ptr->Next != NULL; Ptr = Ptr->Next);
  Ptr->Next = Volume;
  return List;
}
// FileFreeVolumeList
/// Free file system volume list
/// @param List The file system volume list to free
STATIC VOID
EFIAPI
FileFreeVolumeList (
  IN FILE_VOLUME_LIST *List
) {
  while (List != NULL) {
    FILE_VOLUME_LIST *Tmp = List;
    List = Tmp->Next;
    FreePool(Tmp);
  }
}
// FileFreeVolumeMembers
/// Free the members of file system volume
/// @param Volume The volume whose members to free
STATIC VOID
EFIAPI
FileFreeVolumeMembers (
  IN FILE_VOLUME *Volume
) {
  if (Volume != NULL) {
    // Close the file system boot
    if (Volume->FileSystem != NULL) {
      Volume->FileSystem->Close(Volume->FileSystem);
      Volume->FileSystem = NULL;
    }
    // Free the device path
    if (Volume->DevicePath != NULL) {
      FreePool(Volume->DevicePath);
      Volume->DevicePath = NULL;
    }
    // Free volume label
    if (Volume->Label != NULL) {
      FreePool(Volume->Label);
      Volume->Label = NULL;
    }
    Volume->Device = NULL;
  }
}
// FileCopyVolumeMembers
/// Copy the members of file system volume
/// @param Destination The destination file system volume
/// @param Source      The source file system volume
STATIC VOID
EFIAPI
FileCopyVolumeMembers (
  IN OUT FILE_VOLUME *Destination,
  IN     FILE_VOLUME *Source
) {
  if ((Destination != NULL) && (Source != NULL)) {
    Destination->Device = Source->Device;
    Destination->DevicePath = DuplicateDevicePath(Source->DevicePath);
    Destination->FileSystem = NULL;
    FileHandleOpenRoot(&(Destination->FileSystem), Destination->Device);
    Destination->Label = StrDup(Source->Label);
    Destination->Path = StrDup(Source->Path);
  }
}

// FileGetBootVolume
/// Get the boot file system volume (from where this executable was loaded)
/// @return The boot volume, which must be freed, or NULL if there was an error
FILE_VOLUME *
EFIAPI
FileGetBootVolume (
  VOID
) {
  EFI_TPL      OldTpl;
  FILE_VOLUME *Volume = NULL;
  // Use file system protocol if present
  if ((mFileProtocol != NULL) && (mFileProtocol->GetBootVolume != NULL)) {
    return mFileProtocol->GetBootVolume();
  }
  // Check for boot services
  if ((gBS == NULL) || (gBS->RaiseTPL == NULL) || (gBS->RestoreTPL == NULL)) {
    return NULL;
  }
  // Raise TPL to prevent interruption by notify change
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  // Create file system volume
  Volume = (FILE_VOLUME *)AllocateZeroPool(sizeof(FILE_VOLUME));
  if (Volume != NULL) {
    // Copy file system volume members
    FileCopyVolumeMembers(Volume, &mFileBootVolume);
  }
  // Restore TPL
  gBS->RestoreTPL(OldTpl);
  return Volume;
}
// FileGetVolumes
/// Get list of file system volumes
/// @param Volumes On output, the current volumes list, which needs freed
/// @param Count   On output, the count of volumes returned
/// @return Whether the list of volumes was returned or not
/// @retval EFI_INVALID_PARAMETER If Volumes or Count is NULL
/// @retval EFI_OUT_OF_RESOURCES  The memory for the volumes could not be allocated
/// @retval EFI_SUCCESS           The list of volumes was returned successfully
EFI_STATUS
EFIAPI
FileGetVolumes (
  OUT FILE_VOLUME **Volumes,
  OUT UINTN        *Count
) {
  FILE_VOLUME      *Result;
  FILE_VOLUME_INFO *List;
  EFI_TPL           OldTpl;
  UINTN             ListCount = 0;
  // Check parameters
  if ((Volumes == NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Use file system protocol if present
  if ((mFileProtocol != NULL) && (mFileProtocol->GetVolumes != NULL)) {
    return mFileProtocol->GetVolumes(Volumes, Count);
  }
  // Check for boot services
  if ((gBS == NULL) || (gBS->RaiseTPL == NULL) || (gBS->RestoreTPL == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Raise TPL to prevent interruption by notify change
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  // Count the number of file system volumes in list
  for (List = mFileVolumes; List != NULL; List = List->Next) {
    ++ListCount;
  }
  // Create an array of file system volumes
  Result = (FILE_VOLUME *)AllocateZeroPool(sizeof(FILE_VOLUME) * ListCount);
  if (Result == NULL) {
    // Restore TPL
    gBS->RestoreTPL(OldTpl);
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy the array of file system volumes
  ListCount = 0;
  for (List = mFileVolumes; List != NULL; List = List->Next) {
    FileCopyVolumeMembers(Result + ListCount, &(List->Information));
    ++ListCount;
  }
  // Return the file system volumes and count
  *Volumes = Result;
  *Count = ListCount;
  // Restore TPL
  gBS->RestoreTPL(OldTpl);
  return EFI_SUCCESS;
}
// GetFileVolumesNoBoot
/// Get list of file system volumes without the boot file system volume
/// @param Volumes On output, the current volumes list, which needs freed
/// @param Count   On output, the count of volumes returned
/// @return Whether the list of volumes was returned or not
/// @retval EFI_INVALID_PARAMETER If Volumes or Count is NULL
/// @retval EFI_OUT_OF_RESOURCES  The memory for the volumes could not be allocated
/// @retval EFI_SUCCESS           The list of volumes was returned successfully
EFI_STATUS
EFIAPI
FileGetVolumesNoBoot (
  OUT FILE_VOLUME **Volumes,
  OUT UINTN        *Count
) {
  FILE_VOLUME      *Result;
  FILE_VOLUME_INFO *List;
  EFI_TPL           OldTpl;
  UINTN             ListCount = 0;
  // Check parameters
  if ((Volumes == NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Use file system protocol if present
  if ((mFileProtocol != NULL) && (mFileProtocol->GetVolumes != NULL)) {
    return mFileProtocol->GetVolumes(Volumes, Count);
  }
  // Check for boot services
  if ((gBS == NULL) || (gBS->RaiseTPL == NULL) || (gBS->RestoreTPL == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Raise TPL to prevent interruption by notify change
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  // Count the number of file system volumes in list
  for (List = mFileVolumes; List != NULL; List = List->Next) {
    if (List->Information.Device != mFileBootVolume.Device) {
      ++ListCount;
    }
  }
  // Create an array of file system volumes
  Result = (FILE_VOLUME *)AllocateZeroPool(sizeof(FILE_VOLUME) * ListCount);
  if (Result == NULL) {
    // Restore TPL
    gBS->RestoreTPL(OldTpl);
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy the array of file system volumes
  ListCount = 0;
  for (List = mFileVolumes; List != NULL; List = List->Next) {
    if (List->Information.Device != mFileBootVolume.Device) {
      FileCopyVolumeMembers(Result + ListCount, &(List->Information));
      ++ListCount;
    }
  }
  // Return the file system volumes and count
  *Volumes = Result;
  *Count = ListCount;
  // Restore TPL
  gBS->RestoreTPL(OldTpl);
  return EFI_SUCCESS;
}
// FileFreeVolumes
/// Free a list of volumes
/// @param Volumes The volumes list to free
/// @param Count   The count of volumes in the list
/// @return Whether the volumes list was freed or not
/// @retval EFI_INVALID_PARAMETER If Volumes is NULL or Count is zero
/// @retval EFI_SUCCESS           If the volumes list was freed
EFI_STATUS
EFIAPI
FileFreeVolumes (
  IN FILE_VOLUME *Volumes,
  IN UINTN        Count
) {
  UINTN Index = 0;
  // Check parameters
  if ((Volumes == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Free each volume
  while (Index < Count) {
    FileFreeVolumeMembers(Volumes + Index);
    ++Index;
  }
  // Free the volumes list
  FreePool(Volumes);
  return EFI_SUCCESS;
}
// FileFreeVolume
/// Free a volume
/// @param Volume The volume to free
/// @return Whether the volume was freed or not
/// @retval EFI_INVALID_PARAMETER If Volume is NULL
/// @retval EFI_SUCCESS           If the volume was freed
EFI_STATUS
EFIAPI
FileFreeVolume (
  IN FILE_VOLUME *Volume
) {
  return FileFreeVolumes(Volume, 1);
}

// FileFindVolumes
/// Find volumes
/// @param Volumes On output, the matched volumes, which needs to be freed
/// @param Count   On output, the count of matched volumes
/// @param Match   The string to use in matching the volumes
/// @param Options The matching options
/// @return Whether any matching volumes were found and returned
/// @retval EFI_INVALID_PARAMETER If Volumes, Count or Match is NULL or Options is zero or invalid
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the volumes information
/// @retval EFI_NOT_FOUND         If no volumes matched the given string and options
/// @retval EFI_SUCCESS           If the volumes and count were returned successfully
EFI_STATUS
EFIAPI
FileFindVolumes (
  OUT FILE_VOLUME **Volumes,
  OUT UINTN        *Count,
  IN  CHAR16       *Match,
  IN  UINT32        Options
) {
  EFI_TPL           OldTpl;
  FILE_VOLUME_INFO *List;
  FILE_VOLUME      *Result;
  FILE_VOLUME_LIST *ResultList = NULL;
  UINTN             ResultCount = 0;
  UINTN             Index = 0;
  // Check parameters
  if ((Volumes == NULL) || (Count == NULL) || (Match == NULL) || ((Options & VOLUME_MATCH_ALL) == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Use file system volume protocol if present
  if ((mFileProtocol != NULL) && (mFileProtocol->FindVolumes != NULL)) {
    return mFileProtocol->FindVolumes(Volumes, Count, Match, Options);
  }
  // Check for boot services
  if ((gBS == NULL) || (gBS->RaiseTPL == NULL) || (gBS->RestoreTPL == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Raise TPL to prevent interruption by notify change
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  // Iterate through each volume for matching
  for (List = mFileVolumes; List != NULL; List = List->Next) {
    // Try to match against volume device path or label
    if ((((Options & VOLUME_MATCH_DEVICE_PATH) != 0) && (StriStr(List->Information.Path, Match) != NULL)) ||
        (((Options & VOLUME_MATCH_LABEL) != 0) && (StriStr(List->Information.Label, Match) != NULL))) {
      // Add to result list
      ResultList = FileAddVolumeList(ResultList, List);
      ++ResultCount;
      continue;
    }
  }
  // Check for results
  if (ResultList == NULL) {
    // Restore TPL
    gBS->RestoreTPL(OldTpl);
    return EFI_NOT_FOUND;
  }
  if (ResultCount == 0) {
    FileFreeVolumeList(ResultList);
    // Restore TPL
    gBS->RestoreTPL(OldTpl);
    return EFI_NOT_FOUND;
  }
  // Allocate the volumes buffer
  Result = (FILE_VOLUME *)AllocateZeroPool(ResultCount * sizeof(FILE_VOLUME));
  if (Result == NULL) {
    FileFreeVolumeList(ResultList);
    // Restore TPL
    gBS->RestoreTPL(OldTpl);
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy the results
  Index = 0;
  while ((Result != NULL) && (Index < ResultCount)) {
    FILE_VOLUME_LIST *Volume = ResultList;
    // Advance the list
    ResultList = Volume->Next;
    // Copy members of the volume to result
    FileCopyVolumeMembers(Result + Index, &(Volume->Volume->Information));
    // Free the file system volume list node
    FreePool(Volume);
    // Advance the index
    ++Index;
  }
  // Return the results
  *Volumes = Result;
  *Count = Index;
  // Restore TPL
  gBS->RestoreTPL(OldTpl);
  return EFI_SUCCESS;
}
// FileFindVolume
/// Find volume by device handle
/// @param Volume On output, the volume, which needs to be freed
/// @param Device The device handle of the volume
/// @return Whether the volume was found and returned
/// @retval EFI_INVALID_PARAMETER If Volume or Device is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the volume information
/// @retval EFI_NOT_FOUND         If no volume was found
/// @retval EFI_SUCCESS           If the volume was returned successfully
EFI_STATUS
EFIAPI
FileFindVolume (
  OUT FILE_VOLUME **Volume,
  IN  EFI_HANDLE    Device
) {
  FILE_VOLUME_INFO *List;
  EFI_STATUS        Status;
  EFI_TPL           OldTpl;
  // Check parameters
  if ((Volume == NULL) || (Device == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if ((gBS == NULL) || (gBS->RaiseTPL == NULL) || (gBS->RestoreTPL == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Use file system volume protocol if present
  if ((mFileProtocol != NULL) && (mFileProtocol->FindVolume != NULL)) {
    return mFileProtocol->FindVolume(Volume, Device);
  }
  // Change TPL to prevent interruption by notify change
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  Status = EFI_NOT_FOUND;
  // Iterate through the volumes list
  for (List = mFileVolumes; List != NULL; List = List->Next) {
    // Check if device handles match
    if (List->Information.Device == Device) {
      // Create copy of the file system volume information
      FILE_VOLUME *Copy = (FILE_VOLUME *)AllocateZeroPool(sizeof(FILE_VOLUME));
      if (Copy == NULL) {
        Status = EFI_OUT_OF_RESOURCES;
        break;
      }
      FileCopyVolumeMembers(Copy, &(List->Information));
      Status = EFI_SUCCESS;
      break;
    }
  }
  // Restore TPL
  gBS->RestoreTPL(OldTpl);
  return Status;
}

// FileExists
/// Check file path exists
/// @param Handle The handle to optionally use in checking the path
/// @param Path   The path of the file to check exists
/// @return Whether the file path exists or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_NOT_FOUND         If the file path does not exist
/// @retval EFI_SUCCESS           If the file path exists
EFI_STATUS
EFIAPI
FileExists (
  IN EFI_FILE_HANDLE  Handle OPTIONAL,
  IN CHAR16          *Path
) {
  EFI_STATUS      Status;
  EFI_FILE_HANDLE Opened = NULL;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Open file handle to test existance
  Status = FileHandleOpen(&Opened, Handle, Path, 0, 0);
  if (EFI_ERROR(Status)) {
    // Not found on error
    Status = EFI_NOT_FOUND;
  } else if (Opened != NULL) {
    // Close file handle
    FileHandleClose(Opened);
  }
  return Status;
}
// FileExistsByDevice
/// Check file path exists by device handle
/// @param Device The device handle to use in checking the path
/// @param Path   The path of the file to check exists
/// @return Whether the file path exists or not
/// @retval EFI_INVALID_PARAMETER If Volume or Path is NULL
/// @retval EFI_NOT_FOUND         If the file path does not exist
/// @retval EFI_SUCCESS           If the file path exists
EFI_STATUS
EFIAPI
FileExistsByDevice (
  IN EFI_HANDLE  Device,
  IN CHAR16     *Path
) {
  EFI_STATUS       Status;
  EFI_DEVICE_PATH *DevicePath;
  // Check parameters
  if ((Device == NULL) || (Path == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create device path for file path
  DevicePath = FileDevicePath(Device, Path);
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Check file exists by device path
  Status = FileExistsByDevicePath(DevicePath);
  // Free device path
  FreePool(DevicePath);
  return Status;
}
// FileExistsByDevicePath
/// Check file path exists by device path
/// @param DevicePath The device path of the file to check exists
/// @return Whether the file path exists or not
/// @retval EFI_INVALID_PARAMETER If DevicePath is NULL
/// @retval EFI_NOT_FOUND         If the file path does not exist
/// @retval EFI_SUCCESS           If the file path exists
EFI_STATUS
EFIAPI
FileExistsByDevicePath (
  IN EFI_DEVICE_PATH *DevicePath
) {
  EFI_STATUS  Status;
  CHAR16     *Path;
  // Check parameters
  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Get path from device path
  Path = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
  if (Path == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Check file exists by path
  Status = FileExists(NULL, Path);
  FreePool(Path);
  return Status;
}

// FileHandleOpenRoot
/// Open the root file handle of the file system of device
/// @param Handle On output, the root file handle
/// @param Device The device handle
/// @return Whether the root file handle was opened or not
/// @retval EFI_INVALID_PARAMETER If Handle or Device is NULL
/// @retval EFI_SUCCESS If the root file handled was opened successfully
EFI_STATUS
EFIAPI
FileHandleOpenRoot (
  OUT EFI_FILE_HANDLE *Handle,
  IN  EFI_HANDLE       Device
) {
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *Volume = NULL;

  // Check parameters
  if ((Handle == NULL) || (Device == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Check boot services
  if ((gBS == NULL) || (gBS->HandleProtocol == NULL)) {
    return EFI_UNSUPPORTED;
  }

  // Get the file system protocolfrom the device
  Status = gBS->HandleProtocol(Device, &gEfiSimpleFileSystemProtocolGuid, (VOID *)&Volume);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if ((Volume == NULL) || (Volume->OpenVolume == NULL)) {
    return EFI_NOT_FOUND;
  }

  // Open the boot directory of the volume
  return Volume->OpenVolume(Volume, Handle);
}

// FileHandleOpen
/// Open a file handle
/// @param Handle     On output, the opened file handle
/// @param Root       The file handle to use in opening the file path or NULL to find by device path
/// @param Path       The path of the file to open
/// @param OpenMode   The mode to use in opening the file handle
/// @param Attributes If OpenMode has EFI_FILE_MODE_CREATE, then the attributes for the created file
/// @return Whether the file was opened or not
/// @retval EFI_INVALID_PARAMETER If Handle or Path is NULL
/// @retval EFI_NOT_FOUND         If the file path was not found
/// @retval EFI_SUCCESS           If the file handle opened successfully
EFI_STATUS
EFIAPI
FileHandleOpen (
  OUT EFI_FILE_HANDLE *Handle,
  IN  EFI_FILE_HANDLE  Root OPTIONAL,
  IN  CHAR16          *Path,
  IN  UINT64           OpenMode,
  IN  UINT64           Attributes
) {
  EFI_STATUS       Status;
  EFI_HANDLE       Device;
  EFI_DEVICE_PATH *RemainingPath;
  EFI_DEVICE_PATH *DevicePath;
  // Check parameters
  if ((Handle == NULL) || (Path == NULL) || (*Path == L'\0')) {
    return EFI_INVALID_PARAMETER;
  }
  // Check if root is valid
  if (Root != NULL) {
    // Open relative to root
    return Root->Open(Root, Handle, Path, OpenMode, Attributes);
  }
  // Get the device path from the path
  DevicePath = ConvertTextToDevicePath(Path);
  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the device handle from the device path
  Device = NULL;
  RemainingPath = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &RemainingPath, &Device);
  FreePool(DevicePath);
  if (EFI_ERROR(Status) || (Device == NULL)) {
    // Get the boot volume
    FILE_VOLUME *BootVolume = FileGetBootVolume();
    if ((BootVolume == NULL) || (BootVolume->FileSystem == NULL) || (BootVolume->FileSystem->Open == NULL)) {
      return EFI_NOT_FOUND;
    }
    // Open the file handle from the boot volume
    return BootVolume->FileSystem->Open(BootVolume->FileSystem, Handle, Path, OpenMode, Attributes);
  }
  // Open the root from the device
  Status = FileHandleOpenRoot(&Root, Device);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check if root is valid now
  if (Root == NULL) {
    return EFI_NOT_FOUND;
  }
  // Open file handle
  Status = Root->Open(Root, Handle, Path, OpenMode, Attributes);
  // Close root handle and free path
  FileHandleClose(Root);
  return Status;
}
// FileHandleOpenByDevicePath
/// Open a file handle from device path
/// @param Handle     On output, the opened file handle
/// @param DevicePath The device path of the file to open
/// @param OpenMode   The mode to use in opening the file handle
/// @param Attributes If OpenMode has EFI_FILE_MODE_CREATE, then the attributes for the created file
/// @return Whether the file was opened or not
/// @retval EFI_INVALID_PARAMETER If Handle or DevicePath is NULL
/// @retval EFI_NOT_FOUND         If the device path was not found
/// @retval EFI_SUCCESS           If the file handle opened successfully
EFI_STATUS
EFIAPI
FileHandleOpenByDevicePath (
  OUT EFI_FILE_HANDLE *Handle,
  IN  EFI_DEVICE_PATH *DevicePath,
  IN  UINT64           OpenMode,
  IN  UINT64           Attributes
) {
  EFI_STATUS  Status;
  CHAR16     *Path;
  // Check parameters
  if ((Handle == NULL) || (DevicePath == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get path from device path
  Path = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
  // Open file handle
  Status = FileHandleOpen(Handle, NULL, Path, OpenMode, Attributes);
  FreePool(Path);
  return Status;
}
// FileHandleOpenByDevice
/// Open a file handle by device handle
/// @param Handle     On output, the opened file handle
/// @param Device     The device handle to use to open the file
/// @param Path       The path of the file to open
/// @param OpenMode   The mode to use in opening the file handle
/// @param Attributes If OpenMode has EFI_FILE_MODE_CREATE, then the attributes for the created file
/// @return Whether the file was opened or not
/// @retval EFI_INVALID_PARAMETER If Handle, Device, or Path is NULL
/// @retval EFI_NOT_FOUND         If the file path not found
/// @retval EFI_SUCCESS           If the file handle opened successfully
EFI_STATUS
EFIAPI
FileHandleOpenByDevice (
  OUT EFI_FILE_HANDLE *Handle,
  IN  EFI_HANDLE       Device,
  IN  CHAR16          *Path,
  IN  UINT64           OpenMode,
  IN  UINT64           Attributes
) {
  EFI_STATUS       Status;
  EFI_DEVICE_PATH *DevicePath;
  // Check parameters
  if ((Handle == NULL) || (Path == NULL) || (Device == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get device path from volume and path
  DevicePath = FileDevicePath(Device, Path);
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Open file handle by device path
  Status = FileHandleOpenByDevicePath(Handle, DevicePath, OpenMode, Attributes);
  FreePool(DevicePath);
  return Status;
}

// FileHandleGetInfoByGuid
/// Get file handle information by GUID
/// @param Handle The file handle
/// @param Guid   The GUID of the file information to retrieve
/// @param Size   Optionally, on output, the size of the returned information
/// @return The file handle GUID information or NULL if there was an error
VOID *
EFIAPI
FileHandleGetInfoByGuid (
  IN  EFI_FILE_HANDLE  Handle,
  IN  EFI_GUID        *Guid,
  OUT UINTN           *Size OPTIONAL
) {
  UINTN  BufferSize = 0;
  VOID  *Buffer = NULL;
  if ((Handle == NULL) || (Guid == NULL)) {
    return NULL;
  }
  // Get the size needed for the buffer
  if (Handle->GetInfo(Handle, Guid, &BufferSize, Buffer) != EFI_BUFFER_TOO_SMALL) {
    return NULL;
  }
  if (BufferSize == 0) {
    return NULL;
  }
  // Increase the buffer to prevent overflow (let's make it 64bit aligned for fun)
  BufferSize += (16 - (BufferSize & 7));
  // Allocate the buffer
  Buffer = AllocateZeroPool(BufferSize);
  if (Buffer == NULL) {
    return NULL;
  }
  // Fill the buffer with file information
  if (EFI_ERROR(Handle->GetInfo(Handle, Guid, &BufferSize, Buffer))) {
    FreePool(Buffer);
    return NULL;
  }
  // Return the buffer
  if (Size != NULL) {
    *Size = BufferSize;
  }
  return Buffer;
}
// FileHandleGetSystemInfo
/// Get the file system information
/// @param Handle The file handle
/// @return The file system information or NULL if there was an error
EFI_FILE_SYSTEM_INFO *
FileHandleGetSystemInfo (
  IN EFI_FILE_HANDLE Handle
) {
  return (EFI_FILE_SYSTEM_INFO *)FileHandleGetInfoByGuid(Handle, &gEfiFileSystemInfoGuid, NULL);
}
// FileHandleGetSystemLabelInfo
/// Get the file system information
/// @param Handle The file handle
/// @return The file system information or NULL if there was an error
EFI_FILE_SYSTEM_VOLUME_LABEL *
FileHandleGetSystemVolumeLabel (
  IN EFI_FILE_HANDLE Handle
) {
  EFI_FILE_SYSTEM_VOLUME_LABEL *Label;
  EFI_FILE_SYSTEM_INFO         *Info;
  UINTN                         Size = 0;
  // Get the file system volume label
  Label = (EFI_FILE_SYSTEM_VOLUME_LABEL *)FileHandleGetInfoByGuid(Handle, &gEfiFileSystemVolumeLabelInfoIdGuid, &Size);
  if (Label == NULL) {
    return NULL;
  }
  // Check to make sure this is not mistakenly EFI_FILE_SYSTEM_INFO
  Info = (EFI_FILE_SYSTEM_INFO *)Label;
  if (Info->Size == Size) {
    // Create a new EFI_FILE_SYSTEM_VOLUME_LABEL
    UINTN Length = StrLen(Info->VolumeLabel);
    Label = (EFI_FILE_SYSTEM_VOLUME_LABEL *)AllocateZeroPool(sizeof(EFI_FILE_SYSTEM_VOLUME_LABEL) + (++Length * sizeof(CHAR16)));
    if (Label != NULL) {
      StrCpyS(Label->VolumeLabel, Length, Info->VolumeLabel);
    }
    FreePool(Info);
  }
  return Label;
}

// FileHandleAppend
/// Append write to file handle
/// @param Handle     The file handle to use in appending
/// @param BufferSize On input, the size, in bytes, of the buffer, on output, the size, in bytes, written to the file
/// @param Buffer     The buffer to append to the file
/// @return Whether the buffer was appended to the file or not
/// @retval EFI_INVALID_PARAMETER If Handle, BufferSize, or Buffer is NULL or *BufferSize is zero
/// @retval EFI_SUCCESS           If the buffer was appended to the file successfully
EFI_STATUS
EFIAPI
FileHandleAppend (
  IN     EFI_FILE_HANDLE  Handle,
  IN OUT UINTN           *BufferSize,
  IN     VOID            *Buffer
) {
  EFI_STATUS Status;
  // Check parameters
  if ((Handle == NULL) || (Buffer == NULL) || (BufferSize == NULL) || (*BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Set position at end of file
  Status = FileHandleSetPosition(Handle, 0xFFFFFFFFFFFFFFFF);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Write buffer at end of file
  return FileHandleWrite(Handle, BufferSize, Buffer);
}
// FileHandleAppendByPath
/// Append write to file handle
/// @param Handle     The file handle to use in opening the file or NULL to find by device path
/// @param Path       The path of the file to use in appending
/// @param BufferSize On input, the size, in bytes, of the buffer, on output, the size, in bytes, written to the file
/// @param Buffer     The buffer to append to the file
/// @return Whether the buffer was appended to the file or not
/// @retval EFI_INVALID_PARAMETER If Path, BufferSize, or Buffer is NULL or *BufferSize is zero
/// @retval EFI_SUCCESS           If the buffer was appended to the file successfully
EFI_STATUS
EFIAPI
FileHandleAppendByPath (
  IN     EFI_HANDLE  Handle OPTIONAL,
  IN     CHAR16     *Path,
  IN OUT UINTN      *BufferSize,
  IN     VOID       *Buffer
) {
  EFI_STATUS      Status;
  EFI_FILE_HANDLE Opened = NULL;
  // Check parameters
  if ((Path == NULL) || (Buffer == NULL) || (BufferSize == NULL) || (*BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Open file handle to append
  Status = FileHandleOpen(&Opened, Handle, Path, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Append to file
  Status = FileHandleAppend(Opened, BufferSize, Buffer);
  // Close file handle
  FileHandleClose(Opened);
  return Status;
}
// FileHandleAppendByDevicePath
/// Append write to file handle
/// @param DevicePath The device path of the file to use in appending
/// @param BufferSize On input, the size, in bytes, of the buffer, on output, the size, in bytes, written to the file
/// @param Buffer     The buffer to append to the file
/// @return Whether the buffer was appended to the file or not
/// @retval EFI_INVALID_PARAMETER If DevicePath, BufferSize, or Buffer is NULL or *BufferSize is zero
/// @retval EFI_SUCCESS           If the buffer was appended to the file successfully
EFI_STATUS
EFIAPI
FileHandleAppendByDevicePath (
  IN     EFI_DEVICE_PATH *DevicePath,
  IN OUT UINTN           *BufferSize,
  IN     VOID            *Buffer
) {
  EFI_STATUS  Status;
  CHAR16     *Path;
  // Check parameters
  if ((DevicePath == NULL) || (Buffer == NULL) || (BufferSize == NULL) || (*BufferSize == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get path from device path
  Path = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
  if (Path == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Append to file by path
  Status = FileHandleAppendByPath(NULL, Path, BufferSize, Buffer);
  FreePool(Path);
  return Status;
}

// FileCreateDirectory
/// Create a directory
/// @param Handle Optionally, on output, the file handle of the created directory
/// @param Boot   The file handle to use in creating the directory or NULL to find by device path
/// @param Path   The path of the directory to create
/// @return Whether the directory was created or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_SUCCESS           If the directory was created successfully
EFI_STATUS
EFIAPI
FileCreateDirectory (
  OUT EFI_FILE_HANDLE *Handle OPTIONAL,
  IN  EFI_FILE_HANDLE  Boot OPTIONAL,
  IN  CHAR16          *Path
) {
  EFI_STATUS      Status;
  EFI_FILE_HANDLE Opened = NULL;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Create directory
  Status = FileHandleOpen(&Opened, Boot, Path, EFI_FILE_MODE_CREATE, EFI_FILE_DIRECTORY);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Close or return the file handle
  if (Handle == NULL) {
    FileHandleClose(Opened);
  } else {
    *Handle = Opened;
  }
  return Status;
}
// FileCreateDirectoryByDevice
/// Create a directory by device handle
/// @param Handle Optionally, on output, the file handle of the created directory
/// @param Device The device handle to use in creating the directory
/// @param Path   The path of the directory to create
/// @return Whether the directory was created or not
/// @retval EFI_INVALID_PARAMETER If Device or Path is NULL
/// @retval EFI_SUCCESS           If the directory was created successfully
EFI_STATUS
EFIAPI
FileCreateDirectoryByDevice (
  OUT EFI_FILE_HANDLE *Handle OPTIONAL,
  IN  EFI_HANDLE       Device,
  IN  CHAR16          *Path
) {
  EFI_STATUS       Status;
  EFI_DEVICE_PATH *DevicePath;
  // Check parameters
  if ((Device == NULL) || (Path == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get device path from device handle and path
  DevicePath = FileDevicePath(Device, Path);
  if (DevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Create directory by device path
  Status = FileCreateDirectoryByDevicePath(Handle, DevicePath);
  FreePool(DevicePath);
  return Status;
}
// FileCreateDirectoryByDevicePath
/// Create a directory by device path
/// @param Handle     Optionally, on output, the file handle of the created directory
/// @param DevicePath The path of the directory to create
/// @return Whether the directory was created or not
/// @retval EFI_INVALID_PARAMETER If DevicePath is NULL
/// @retval EFI_SUCCESS           If the directory was created successfully
EFI_STATUS
EFIAPI
FileCreateDirectoryByDevicePath (
  OUT EFI_FILE_HANDLE *Handle OPTIONAL,
  IN  EFI_DEVICE_PATH *DevicePath
) {
  EFI_STATUS  Status;
  CHAR16     *Path;
  // Check parameters
  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Get path from device path
  Path = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
  if (Path == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Create directory from path
  Status = FileCreateDirectory(Handle, NULL, Path);
  FreePool(Path);
  return Status;
}

// FileMakePath
/// Create a path from two parts
/// @param Root The root part of the path
/// @param Path The part of the path to append
/// @return The created path
CHAR16 *
EFIAPI
FileMakePath (
  IN CHAR16 *Root OPTIONAL,
  IN CHAR16 *Path
) {
  CHAR16 *FullPath;
  UINTN   Length;
  UINTN   Count;
  // Check parameters
  if (Path == NULL) {
    if (Root == NULL) {
      return NULL;
    }
    return StrDup(Root);
  }
  Count = StrLen(Path);
  if (Root == NULL) {
    if (*Path == L'\\') {
      return StrDup(Path);
    }
    FullPath = (CHAR16 *)AllocateZeroPool((Count + 2) * sizeof(CHAR16));
    if (FullPath != NULL) {
      *FullPath = L'\\';
      StrCpyS(FullPath + 1, Count + 1, Path);
    }
    return FullPath;
  }
  Length = StrLen(Root);
  if ((Root[Length - 1] == L'\\') && (*Path == L'\\')) {
    FullPath = (CHAR16 *)AllocateZeroPool((Count + Length) * sizeof(CHAR16));
    if (FullPath != NULL) {
      StrCpyS(FullPath, Length + 1, Root);
      StrCpyS(FullPath + Length, Count, Path + 1);
    }
    return FullPath;
  } else if ((Root[Length - 1] == L'\\') || (*Path == L'\\')) {
    FullPath = (CHAR16 *)AllocateZeroPool((Count + Length + 1) * sizeof(CHAR16));
    if (FullPath != NULL) {
      StrCpyS(FullPath, Length + 1, Root);
      StrCpyS(FullPath + Length, Count + 1, Path);
    }
    return FullPath;
  }
  FullPath = (CHAR16 *)AllocateZeroPool((Count + Length + 2) * sizeof(CHAR16));
  if (FullPath != NULL) {
    StrCpyS(FullPath, Length + 1, Root);
    FullPath[Length] = L'\\';
    StrCpyS(FullPath + Length + 1, Count + 1, Path);
  }
  return FullPath;
}

// mFileRefresh
/// Whether the volumes need refreshed or not
STATIC BOOLEAN              mFileRefresh = FALSE;
// mFileHandle
/// The handle for the file system volume protocol
STATIC EFI_HANDLE           mFileHandle = NULL;
// mFileThisProtocol
/// The file system volume protocol to install
STATIC FILE_VOLUME_PROTOCOL mFileThisProtocol = {
  FileGetBootVolume,
  FileGetVolumes,
  FileFindVolumes,
  FileFindVolume
};

// FilePopulateVolume
/// Populate volume information
/// @param Volume The volume to populate
/// @param Device The device handle of the volume
STATIC VOID
EFIAPI
FilePopulateVolume (
  IN OUT FILE_VOLUME *Volume,
  IN     EFI_HANDLE   Device
) {
  if ((Volume != NULL) && (Device != NULL)) {
    // Get device handle
    Volume->Device = Device;
    // Get device path from device handle
    Volume->DevicePath = DuplicateDevicePath(DevicePathFromHandle(Device));
    if (Volume->DevicePath != NULL) {
      // Convert device path to path
      Volume->Path = ConvertDevicePathToText(Volume->DevicePath, FALSE, FALSE);
    } else {
      Volume->Path = NULL;
    }
    // Open file system volume boot handle
    Volume->FileSystem = NULL;
    FileHandleOpenRoot(&(Volume->FileSystem), Device);
    // Get file system volume label from file system information
    Volume->Label = NULL;
    if (Volume->FileSystem != NULL) {
      EFI_FILE_SYSTEM_INFO *Info = FileHandleGetSystemInfo(Volume->FileSystem);
      if (Info != NULL) {
        Volume->Label = StrDup(Info->VolumeLabel);
        FreePool(Info);
      }
      if (Volume->Label == NULL) {
        // Get file system volume label from file system volume label
        EFI_FILE_SYSTEM_VOLUME_LABEL *Label = FileHandleGetSystemVolumeLabel(Volume->FileSystem);
        if (Label != NULL) {
          Volume->Label = StrDup(Label->VolumeLabel);
          FreePool(Label);
        }
        if (Volume->Label == NULL) {
          // Get file system volume label from file handle information
          FileHandleGetFileName(Volume->FileSystem, &(Volume->Label));
          // Some file handle names are stupid so ignore them
          if ((Volume->Label != NULL) && ((Volume->Label[0] == L'\\') || (Volume->Label[0] == L'/')) && (Volume->Label[1] == L'\0')) {
            FreePool(Volume->Label);
            Volume->Label = NULL;
          }
          if (Volume->Label == NULL) {
            VOID *SystemPart = NULL;
            if (!EFI_ERROR(gBS->HandleProtocol(Volume->Device, &gEfiPartTypeSystemPartGuid, &SystemPart))) {
              Volume->Label = StrDup(L"EFI");
            }
          }
        }
      }
    }
  }
}

// FileRefreshVolumes
/// Refresh file system volumes
STATIC VOID
EFIAPI
FileRefreshVolumes (
  VOID
) {
  FILE_VOLUME_INFO *Volumes = NULL;
  EFI_HANDLE       *Devices;
  EFI_STATUS        Status;
  EFI_TPL           OldTpl;
  UINTN             Count;
  
  // Check boot services
  if ((gBS == NULL) || (gBS->LocateHandleBuffer == NULL) ||
      (gBS->RaiseTPL == NULL) || (gBS->RestoreTPL == NULL)) {
    return;
  }
  // Raise TPL to prevent interruption by notify
  OldTpl = gBS->RaiseTPL(TPL_NOTIFY);
  // Free all current volumes
  while (mFileVolumes != NULL) {
    FILE_VOLUME_INFO *Info = mFileVolumes;
    mFileVolumes = Info->Next;
    FileFreeVolumeMembers(&(Info->Information));
    FreePool(Info);
  }
  // Get all devices with a file system
  Count = 0;
  Devices = NULL;
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &Count, &Devices);
  if (!EFI_ERROR(Status) && (Count != 0) && (Devices != NULL)) {
    UINTN Index = 0;
    // Iterate through the device to populate volumes
    while (Index < Count) {
      // Create a new volume information
      FILE_VOLUME_INFO *Info = (FILE_VOLUME_INFO *)AllocateZeroPool(sizeof(FILE_VOLUME_INFO));
      if (Info != NULL) {
        Info->Next = NULL;
        // Populate the volume information
        FilePopulateVolume(&(Info->Information), Devices[Index]);
        // Append to the list
        if (Volumes == NULL) {
          if (mFileVolumes == NULL) {
            Volumes = mFileVolumes = Info;
          } else {
            Volumes = mFileVolumes;
            while (Volumes->Next != NULL) {
              Volumes = Volumes->Next;
            }
            Volumes = Volumes->Next = Info;
          }
        } else {
          Volumes = Volumes->Next = Info;
        }
      }
      // Advance the index
      ++Index;
    }
  }
  // Free the devices buffer
  if (Devices != NULL) {
    FreePool(Devices);
  }
  // Restore the old TPL
  gBS->RestoreTPL(OldTpl);
}

// FileVolumeRefreshNotify
/// Notification of file system volume change event
/// @param Event   The event that triggered the notification
/// @param Context The context passed when registering this notification
STATIC VOID
EFIAPI
FileVolumeRefreshNotify (
  IN EFI_EVENT  Event,
  IN VOID      *Context
) {
  // Refresh volumes if needed
  if (mFileRefresh) {
    FileRefreshVolumes();
    mFileRefresh = FALSE;
  }
  // Restart timer
  gBS->SetTimer(Event, TimerRelative, VOLUME_REFRESH_TIMEOUT);
}
// FileVolumeChangeNotify
/// Notification of file system volume change event
/// @param Event   The event that triggered the notification
/// @param Context The context passed when registering this notification
STATIC VOID
EFIAPI
FileVolumeChangeNotify (
  IN EFI_EVENT  Event,
  IN VOID      *Context
) {
  mFileRefresh = TRUE;
}

// FileLibInitialize
/// File library initialize use
/// @return Whether the library initialized successfully or not
/// @retval EFI_SUCCESS The library successfully initialized
EFI_STATUS
EFIAPI
FileLibInitialize (
  VOID
) {
  EFI_STATUS        Status;
  EFI_LOADED_IMAGE *LoadedImage = NULL;
  // Check for boot services
  if ((gBS == NULL) || (gBS->LocateProtocol == NULL) ||
      (gBS->CreateEvent == NULL) || (gBS->CloseEvent == NULL) ||
      (gBS->InstallMultipleProtocolInterfaces == NULL) ||
      (gBS->UninstallMultipleProtocolInterfaces == NULL) ||
      (gBS->RegisterProtocolNotify == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Check for file system volume protocol
  if (!EFI_ERROR(gBS->LocateProtocol(&mFileVolumeGuid, NULL, &mFileProtocol))) {
    return EFI_SUCCESS;
  }
  // Get the loaded image protocol
  Status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (LoadedImage == NULL) {
    return EFI_NOT_FOUND;
  }
  // Populate the boot volume
  FilePopulateVolume(&mFileBootVolume, LoadedImage->DeviceHandle);
  // Get the file system volumes
  FileRefreshVolumes();
  // Create event for file system volume notification
  Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_NOTIFY, FileVolumeChangeNotify, NULL, &mFileNotifyEvent);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Create notification for file system volume change
  Status = gBS->RegisterProtocolNotify(&gEfiSimpleFileSystemProtocolGuid, &mFileNotifyEvent, &mFileNotifyRegistration);
  if (EFI_ERROR(Status)) {
    gBS->CloseEvent(mFileNotifyEvent);
    return Status;
  }
  // Create event for volume refresh
  Status = gBS->CreateEvent(EVT_NOTIFY_SIGNAL, TPL_NOTIFY, FileVolumeRefreshNotify, NULL, &mFileRefreshEvent);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Set timer for volume refresh
  Status = gBS->SetTimer(mFileRefreshEvent, TimerRelative, VOLUME_REFRESH_TIMEOUT);
  if (EFI_ERROR(Status)) {
    gBS->CloseEvent(mFileRefreshEvent);
    return Status;
  }
  // Install file system volume protocol
  mFileHandle = NULL;
  mFileThisProtocol.GetBootVolume = FileGetBootVolume,
  mFileThisProtocol.GetVolumes = FileGetVolumes;
  mFileThisProtocol.FindVolumes = FileFindVolumes;
  mFileThisProtocol.FindVolume = FileFindVolume;
  return gBS->InstallMultipleProtocolInterfaces(&mFileHandle, &mFileVolumeGuid, &mFileThisProtocol, NULL);
}

// FileLibFinish
/// File library finish use
/// @return Whether the library was finished successfully or not
/// @retval EFI_SUCCESS The library successfully finished
EFI_STATUS
EFIAPI
FileLibFinish (
  VOID
) {
  // Uninstall the file system volume protocol
  if (mFileHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces(mFileHandle, &mFileVolumeGuid, &mFileThisProtocol, NULL);
    mFileHandle = NULL;
  }
  // Cancel event for volume refresh
  if (mFileRefreshEvent != NULL) {
    gBS->CloseEvent(mFileRefreshEvent);
    mFileRefreshEvent = NULL;
  }
  if (mFileNotifyEvent != NULL) {
    gBS->CloseEvent(mFileNotifyEvent);
    mFileNotifyEvent = NULL;
  }
  // Free volumes
  FileFreeVolumeMembers(&mFileBootVolume);
  while (mFileVolumes != NULL) {
    FILE_VOLUME_INFO *Volume = mFileVolumes;
    mFileVolumes = Volume->Next;
    FileFreeVolumeMembers(&(Volume->Information));
    FreePool(Volume);
  }
  return EFI_SUCCESS;
}

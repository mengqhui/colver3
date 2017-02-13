//
/// @file Include/Library/FileLib.h
///
/// File library
///

#pragma once
#ifndef __FILE_LIBRARY_HEADER__
#define __FILE_LIBRARY_HEADER__

#include <Library/StringLib.h>

#include <Library/DevicePathLib.h>
#include <Library/FileHandleLib.h>

// VOLUME_MATCH_DEVICE_PATH
/// Match volumes with a string using device path
#define VOLUME_MATCH_DEVICE_PATH 0x1
// VOLUME_MATCH_LABEL
/// Match volumes with a string using volume label
#define VOLUME_MATCH_LABEL 0x2
// VOLUME_MATCH_ALL
/// Match volumes with a string using all methods
#define VOLUME_MATCH_ALL (VOLUME_MATCH_DEVICE_PATH | VOLUME_MATCH_LABEL)
// VOLUME_MATCH_EXCLUDE
/// Use exclusion matching instead of inclusive (effectively negating the set)
#define VOLUME_MATCH_EXCLUDE 0x8000

// VOLUME_REFRESH_TIMEOUT
/// The amount of time in between checking for volumes refresh - 1/2 second
#define VOLUME_REFRESH_TIMEOUT 5000000

// FILE_VOLUME
/// File system volume
typedef struct _FILE_VOLUME FILE_VOLUME;
struct _FILE_VOLUME {

  // Device
  /// The device handle for this volume
  EFI_HANDLE       Device;
  // FileSystem
  /// The file system on this volume
  EFI_FILE_HANDLE  FileSystem;
  // DevicePath
  /// The device path for this volume
  EFI_DEVICE_PATH *DevicePath;
  // Path
  /// The path for this volume
  CHAR16          *Path;
  // Label
  /// The label of this volume
  CHAR16          *Label;

};

// FileGetBootVolume
/// Get the boot volume (from where this executable was loaded)
/// @return The boot volume, which must be freed, or NULL if there was an error
FILE_VOLUME *
EFIAPI
FileGetBootVolume (
  VOID
);
// GetFileVolumes
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
);
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
);
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
);
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
);

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
);
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
);

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
);
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
);
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
);

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
);

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
);
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
);
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
);

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
);

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
);
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
);
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
);

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
);
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
);
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
);

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
);

#endif // __FILE_LIBRARY_HEADER__

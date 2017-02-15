///
/// @file Library/PlatformLib/PlatformLib.c
///
/// Platform library
///

#include "Platform.h"

#include <Library/ConfigLib.h>
#include <Library/SmBiosLib.h>

#include <Library/UefiBootServicesTableLib.h>

// PrintSystemInformation
/// Print system information
STATIC VOID
EFIAPI
PrintSystemInformation (
  VOID
) {
  EFI_STATUS   Status;
  FILE_VOLUME *BootVolume;
  FILE_VOLUME *Volumes = NULL;
  UINTN        Count;

  // Print CPU information
  PrintCPUInformation();
  // Print memory information
  PrintMemoryInformation();

  // Get boot volume
  BootVolume = FileGetBootVolume();
  if (BootVolume != NULL) {
    //Root = BootVolume->FileSystem;
    Log2(L"Boot volume:", L"\"%s\" <%s>\n", BootVolume->Label, BootVolume->Path);
    // Free boot volume
    FileFreeVolume(BootVolume);
  }
  // Print volume information
  Volumes = NULL;
  // Get all volumes
  Status = FileGetVolumesNoBoot(&Volumes, &Count);
  if (!EFI_ERROR(Status) && (Volumes != NULL) && (Count > 0)) {
    CHAR16 *VolumeLabel;
    UINTN   MaxLength = 0;
    UINTN   Length;
    UINTN   Index;
    // Find the longest volume label
    for (Index = 0; Index < Count; ++Index) {
      Length = StrLen(Volumes[Index].Label);
      if (Length > MaxLength) {
        MaxLength = Length;
      }
    }
    MaxLength += 5;
    // Print each volume's information
    Log2(L"Other volumes:", L"%u\n", Count);
    for (Index = 0; Index < Count; ++Index) {
      Length = StrLen(Volumes[Index].Label);
      VolumeLabel = (CHAR16 *)AllocateZeroPool((Length + 6) * sizeof(CHAR16));
      if (VolumeLabel != NULL) {
        StrCpyS(VolumeLabel, 4, L"  \"");
        StrnCpyS(VolumeLabel + 3, Length + 1, Volumes[Index].Label, Length);
        StrCpyS(VolumeLabel + Length + 3, 3, L"\":");
      }
      Log3((UINT32)MaxLength, (VolumeLabel == NULL) ? L"  \"\":" : VolumeLabel, L"<%s>\n", Volumes[Index].Path);
      if (VolumeLabel != NULL) {
        FreePool(VolumeLabel);
      }
    }
    FileFreeVolumes(Volumes, Count);
  } else {
    Log2(L"Other volumes:", L"None\n");
  }

}

// PlatformLibInitialize
/// Platform library initialize use
/// @return Whether the platform initialized successfully or not
/// @retval EFI_UNSUPPORTED The platform is unsupported or no boot services locate/install/uninstall protocol
/// @retval EFI_SUCCESS     The platform successfully initialized
EFI_STATUS
EFIAPI
PlatformLibInitialize (
  VOID
) {
  EFI_STATUS Status;

  // Check if platform information already exists
  if (ConfigGetBooleanWithDefault(L"\\Platform\\Initialized", FALSE)) {
    return EFI_SUCCESS;
  }

  // Update package information
  Status = UpdatePackageInformation(0);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  // TODO: Run detection code on each package

  // Update all packages information
  UpdatePackagesInformation();

  // Detect memory information from SPD and SMBIOS
  DetectMemoryInformation();

  // Print system information
  PrintSystemInformation();

  // Set platform information intialized
  return ConfigSetBoolean(L"\\Platform\\Initialized", TRUE);
}

// PlatformLibFinish
/// Platform library finish use
/// @return Whether the library was finished successfully or not
/// @retval EFI_SUCCESS The platform library successfully finished
EFI_STATUS
EFIAPI
PlatformLibFinish (
  VOID
) {
  return EFI_SUCCESS;
}

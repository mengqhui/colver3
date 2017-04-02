///
/// @file Library/PlatformLib/PlatformLib.c
///
/// Platform library
///

#include "Platform.h"

#include <Library/ConfigLib.h>
#include <Library/SmBiosLib.h>

#include <Library/UefiBootServicesTableLib.h>

// PrintVolumeInformation
/// Print volume informaion
STATIC VOID
EFIAPI
PrintVolumeInformation (
  VOID
) {
  EFI_STATUS   Status;
  FILE_VOLUME *BootVolume;
  FILE_VOLUME *Volumes = NULL;
  UINTN        Count;

  // Get boot volume
  BootVolume = FileGetBootVolume();
  if (BootVolume != NULL) {
    //Root = BootVolume->FileSystem;
    Log2(L"  Boot volume:", L"\"%s\" <%s>\n", BootVolume->Label, BootVolume->Path);
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
    Log2(L"  Other volumes:", L"%u\n", Count);
    for (Index = 0; Index < Count; ++Index) {
      Length = StrLen(Volumes[Index].Label);
      VolumeLabel = (CHAR16 *)AllocateZeroPool((Length + 8) * sizeof(CHAR16));
      if (VolumeLabel != NULL) {
        StrCpyS(VolumeLabel, 6, L"    \"");
        StrnCpyS(VolumeLabel + 5, Length + 1, Volumes[Index].Label, Length);
        StrCpyS(VolumeLabel + Length + 5, 3, L"\":");
      }
      Log3((UINT32)MaxLength, (VolumeLabel == NULL) ? L"    \"\":" : VolumeLabel, L"<%s>\n", Volumes[Index].Path);
      if (VolumeLabel != NULL) {
        FreePool(VolumeLabel);
      }
    }
    FileFreeVolumes(Volumes, Count);
  } else {
    Log2(L"  Other volumes:", L"None\n");
  }
}

// PrintSystemInformation
/// Print system information
STATIC VOID
EFIAPI
PrintSystemInformation (
  VOID
) {
  Log(L"System information:\n");
  // Print CPU information
  PrintCPUInformation();
  // Print memory information
  PrintMemoryInformation();
  // Print volume information
  PrintVolumeInformation();
}

// PlatformLibInitialize
/// Platform library initialize use
/// @return Whether the platform initialized successfully or not
/// @retval EFI_UNSUPPORTED The platform is unsupported
/// @retval EFI_SUCCESS     The platform successfully initialized
EFI_STATUS
EFIAPI
PlatformLibInitialize (
  VOID
) {
  EFI_STATUS  Status;
  CHAR16     *Path;
  CHAR16     *SystemName;
  UINTN       Unsigned;

  // Check if platform information already exists
  if (ConfigGetBooleanWithDefault(L"\\Platform\\Initialized", FALSE)) {
    return EFI_SUCCESS;
  }

  // Print version information
  Log(L"%s %s\n", PROJECT_NAME, PROJECT_VERSION);
  Log2(L"Architecture:", L"%s\n", PROJECT_ARCH);
  Log2(L"Build date/time:", L"%s\n", PROJECT_DATETIME);
  Log2(L"Start:", NULL);
  LogDateAndTime();
  Log(L"\n");
  // Print firmware and system name
  Log2(L"Firmware:", L"%s %u.%u\n", gST->FirmwareVendor, ((gST->Hdr.Revision >> 16) & 0xFFFF), (gST->Hdr.Revision & 0xFFFF));
  SystemName = ConfigGetStringWithDefault(L"\\System\\Name", NULL);
  if (SystemName != NULL) {
    Log2(L"System name:", L"%s\n", SystemName);
  }

  // Load configuration
  Log2(L"Configuration:", L"\"%s\"\n", DEFAULT_CONFIG_FILE);
  Status = ConfigLoad(NULL, DEFAULT_CONFIG_FILE);
  Log2(L"  Load status:", L"%r\n", Status);

  // Print verbose level
  Unsigned = ConfigGetUnsignedWithDefault(L"\\Log\\Verbose", LOG_VERBOSE_LEVEL);
  if (Unsigned != 0) {
    Log2(L"Verbose:", L"%u\n", Unsigned);
  }
  // Start saving the log now if needed
  Path = ConfigGetStringWithDefault(L"\\Log\\Path", PLATFORM_LOG_FILE);
  if (Path != NULL) {
    if (!EFI_ERROR(SetLogPath(Path, TRUE, TRUE))) {
      Log2(L"Log path:", L"\"%s\"\n", Path);
    }
  }
  // Print output methods for log
  Unsigned = (GetLogOutput() & LOG_OUTPUT_ALL);
  Log3(LOG_PREFIX_WIDTH - 1, L"Log outputs:", L"");
  if (Unsigned == 0) {
    Log(L" None\n");
  } else {
    if ((Unsigned & LOG_OUTPUT_CONSOLE) != 0) {
      Log(L" Console");
    }
    if ((Unsigned & LOG_OUTPUT_FILE) != 0) {
      Log(L" File");
    }
    if ((Unsigned & LOG_OUTPUT_SERIAL) != 0) {
      Log(L" Serial");
    }
    Log(L"\n");
  }

  // Update package information
  LOG(L"Detecting packages:\n");
  Status = UpdatePackageInformation(0);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // TODO: Run detection code on each CPU package to update package information
  // Update CPU packages from SMBIOS information
  UpdatePackagesInformation();

  // Detect memory information from SPD and SMBIOS
  LOG(L"Detecting memory:\n");
  DetectMemoryInformation();

  // Print system information
  PrintSystemInformation();

  // Set platform information intialized
  return ConfigSetBoolean(L"\\Platform\\Initialized", TRUE, TRUE);
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

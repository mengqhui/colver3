///
/// @file Library/PlatformLib/PlatformLib.c
///
/// Platform library
///

#include "Platform.h"

#include <Library/SmBiosLib.h>

#include <Library/UefiBootServicesTableLib.h>

// mSystemPlatformLibGuid
/// CPU information internal GUID
STATIC EFI_GUID    mSystemPlatformLibGuid = { 0x91BCF481, 0x744E, 0x4764, { 0xA3, 0x3A, 0x09, 0x7A, 0xBC, 0x59, 0x28, 0x0D } };
// mSystemInformationHandle
/// CPU information installation handle
STATIC EFI_HANDLE  mSystemInformationHandle = NULL;
// mCPUIDRegisters
/// Registers for CPUID results
UINT32             mCPUIDRegisters[4] = { 0 };
// mCPUMaxIndex
/// CPU maximum CPUID index
UINT32             mCPUMaxIndex = 0;
// mCPUMaxExtIndex
/// CPU maximum extended CPUID index
UINT32             mCPUMaxExtIndex = 0x80000000;
// mSystemInformation
/// CPU information
SYSTEM_INFORMATION mSystemInformation = {
  // LogicalCount
  1,
  // CoreCount
  1,
  // PackageCount
  1,
  // Packages
  {
    {
      // Type
      CPU_VENDOR_UNKNOWN,
      // Family
      CPU_FAMILY_UNKNOWN,
      // Model
      CPU_MODEL_UNKNOWN,
      // LogicalCount
      1,
      // CoreCount
      1,
      // Frequency
      {
        // Clock
        100000000,
        // Minimum
        1,
        // Maximum
        1,
        // Step
        0,
        // TurboCount
        0,
        // Turbo
        { 0 }
      },
      // Features
      { 0 },
      // Description
      { 0 }
    }
  },
  // SlotCount
  0,
  // Slots
  { 0 }
};

// GetSystemInformation
/// Get system information such as type, family, frequency, and features
/// @param Information On output, the system information which should be freed after use
/// @return Whether the system information was returned successfully or not
/// @retval EFI_INVALID_PARAMETER If Information is NULL or *Information is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the system information was returned
EFI_STATUS
EFIAPI
GetSystemInformation (
  OUT SYSTEM_INFORMATION **Information
) {
  // Check parameters
  if ((Information == NULL) || (*Information != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate CPU information
  *Information = AllocateZeroPool((UINTN)sizeof(mSystemInformation));
  if (*Information == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy CPU information
  CopyMem((VOID *)*Information, (VOID *)&mSystemInformation, (UINTN)sizeof(mSystemInformation));
  return EFI_SUCCESS;
}

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
  EFI_STATUS           Status;
  SYSTEM_INFORMATION  *Information = NULL;

  // Check for boot services
  if ((gBS == NULL) || (gBS->LocateProtocol == NULL) ||
      (gBS->InstallMultipleProtocolInterfaces == NULL) ||
      (gBS->UninstallMultipleProtocolInterfaces == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Check if CPU information already exists
  if ((gBS->LocateProtocol(&mSystemPlatformLibGuid, NULL, (VOID **)&Information) == EFI_SUCCESS) && (Information != NULL)) {
    // Copy CPU information from installed information
    CopyMem((VOID *)&mSystemInformation, Information, sizeof(mSystemInformation));
    return EFI_SUCCESS;
  }

  // Set CPU package count
  mSystemInformation.PackageCount = 1;
  mSystemInformation.LogicalCount = 0;
  mSystemInformation.CoreCount = 0;
  // Update package count
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

  // Install system information
  mSystemInformationHandle = NULL;
  return gBS->InstallMultipleProtocolInterfaces(&mSystemInformationHandle, &mSystemPlatformLibGuid, (VOID *)&mSystemInformation, NULL);
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
  // Uninstall system information
  if (mSystemInformationHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces(mSystemInformationHandle, &mSystemPlatformLibGuid, (VOID *)&mSystemInformation, NULL);
    mSystemInformationHandle = NULL;
  }
  return EFI_SUCCESS;
}

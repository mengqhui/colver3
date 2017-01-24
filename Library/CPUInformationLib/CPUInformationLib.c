///
/// @file Library/CPUInformationLib/CPUInformationLib.c
///
/// CPU information library entry
///

#include "CPUInformation.h"

#include <Library/UefiBootServicesTableLib.h>

// mCPUInformationLibGuid
/// CPU information internal GUID
STATIC EFI_GUID   mCPUInformationLibGuid = { 0x91BCF481, 0x744E, 0x4764, { 0xA3, 0x3A, 0x09, 0x7A, 0xBC, 0x59, 0x28, 0x0D } };
// mCPUInformationHandle
/// CPU information installation handle
STATIC EFI_HANDLE mCPUInformationHandle = NULL;
// mCPUIDRegisters
/// Registers for CPUID results
UINT32            mCPUIDRegisters[4] = { 0 };
// mCPUMaxIndex
/// CPU maximum CPUID index
UINT32            mCPUMaxIndex = 0;
// mCPUMaxExtIndex
/// CPU maximum extended CPUID index
UINT32            mCPUMaxExtIndex = 0x80000000;
// mCPUInformation
/// CPU information
CPU_INFORMATION   mCPUInformation = {
  // Type
  CPU_VENDOR_UNKNOWN,
  // Family
  CPU_FAMILY_UNKNOWN,
  // Model
  CPU_MODEL_UNKNOWN,
  // Count
  1,
  // LogicalCount
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
};

// GetCPUInformation
/// Get CPU information such as type, family, frequency, and features
/// @param Information On output, the CPU information which should be freed after use
/// @return Whether the CPU information was returned successfully or not
/// @retval EFI_INVALID_PARAMETER If Information is NULL or *Information is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the CPU information was returned
EFI_STATUS
EFIAPI
GetCPUInformation (
  OUT CPU_INFORMATION **Information
) {
  // Check parameters
  if ((Information == NULL) || (*Information != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate CPU information
  *Information = AllocateZeroPool((UINTN)sizeof(mCPUInformation));
  if (*Information == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy CPU information
  CopyMem((VOID *)*Information, (VOID *)&mCPUInformation, (UINTN)sizeof(mCPUInformation));
  return EFI_SUCCESS;
}

// GetCPUVendor
/// Get CPU vendor information
/// @return The CPU vendor information
CPU_VENDOR
EFIAPI
GetCPUVendor (
  VOID
) {
  return mCPUInformation.Vendor;
}
// GetCPUFamily
/// Get CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
GetCPUFamily (
  VOID
) {
  return mCPUInformation.Family;
}
// GetCPUModel
/// Get CPU model information
/// @return The CPU model information
CPU_MODEL
EFIAPI
GetCPUModel (
  VOID
) {
  return mCPUInformation.Model;
}
// GetCPUFrequency
/// Get CPU frequency
/// @return The CPU frequency
UINT64
EFIAPI
GetCPUFrequency (
  VOID
) {
  return RShiftU64(MultU64x32((UINT64)(mCPUInformation.Frequency.Maximum), (UINTN)mCPUInformation.Frequency.Clock), (UINTN)mCPUInformation.Frequency.Step);
}
// IsCPUMobile
/// Check if CPU is mobile
BOOLEAN
EFIAPI
IsCPUMobile (
  VOID
) {
  return mCPUInformation.Features.Mobile;
}

// DetermineCPUVendor
/// Determine CPU vendor information
/// @return The CPU vendor information
STATIC CPU_VENDOR
EFIAPI
DetermineCPUVendor (
  VOID
) {

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IDF)

  // Perform CPUID
  AsmCpuid(0x80000000, mCPUIDRegisters, mCPUIDRegisters + 1, mCPUIDRegisters + 3, mCPUIDRegisters + 2);
  mCPUMaxExtIndex = mCPUIDRegisters[0];
  AsmCpuid(0, mCPUIDRegisters, mCPUIDRegisters + 1, mCPUIDRegisters + 3, mCPUIDRegisters + 2);
  mCPUMaxIndex = mCPUIDRegisters[0];
  // Intel
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"GenuineIntel", 12) == 0) {
    return CPU_VENDOR_INTEL;
  }

# if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  // AMD
  if ((CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"AuthenticAMD", 12) == 0) ||
      (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"AMDisbetter!", 12) == 0)) {
    return CPU_VENDOR_AMD;
  }

# endif

  // KVM virtual machine
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"KVMKVMKVM\0\0\0", 12) == 0) {
    return CPU_VENDOR_VIRTUAL_KVM;
  }
  // Microsoft Hyper-V/Windows Virtual PC virtual machine
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"Microsoft Hv", 12) == 0) {
    return CPU_VENDOR_VIRTUAL_MICROSOFT;
  }
  // Parallels virtual machine
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)" lrpepyh vr\0", 12) == 0) {
    return CPU_VENDOR_VIRTUAL_PARALLELS;
  }
  // VMWare virtual machine
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"VMwareVMware", 12) == 0) {
    return CPU_VENDOR_VIRTUAL_VMWARE;
  }
  // Xen virtual machine
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"XenVMMXenVMM", 12) == 0) {
    return CPU_VENDOR_VIRTUAL_XEN;
  }

#endif

  return CPU_VENDOR_UNKNOWN;
}

// CPULibInitialize
/// CPU library initialize use
/// @return Whether the library initialized successfully or not
/// @retval EFI_UNSUPPORTED The CPU is unsupported or no boot services locate/install/uninstall protocol
/// @retval EFI_SUCCESS     The GUI successfully initialized
EFI_STATUS
EFIAPI
CPULibInitialize (
  VOID
) {
  // Check if CPU information already exists
  if (gBS != NULL) {
    // Check for boot services locate protocol
    if (gBS->LocateProtocol != NULL) {
      CPU_INFORMATION *Information = NULL;
      // Locate installed CPU information
      if ((gBS->LocateProtocol(&mCPUInformationLibGuid, NULL, (VOID **)&Information) == EFI_SUCCESS) && (Information != NULL)) {
        // Copy CPU information from installed information
        CopyMem((VOID *)&mCPUInformation, Information, sizeof(mCPUInformation));
        return EFI_SUCCESS;
      } else if ((gBS->InstallMultipleProtocolInterfaces == NULL) || (gBS->UninstallMultipleProtocolInterfaces == NULL)) {
        // No boot services install/uninstall protocol
        return EFI_UNSUPPORTED;
      }
    } else {
      // No boot services locate protocol
      return EFI_UNSUPPORTED;
    }
  } else {
    // No boot services table so we can't install
    return EFI_UNSUPPORTED;
  }
  
  // Get CPU vendor information
  mCPUInformation.Vendor = DetermineCPUVendor();
  if (mCPUInformation.Vendor == CPU_VENDOR_UNKNOWN) {
    return EFI_UNSUPPORTED;
  }

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

  if (mCPUInformation.Vendor == CPU_VENDOR_INTEL) {
    // Get CPU model information
    mCPUInformation.Model = DetermineCPUIntelModelAndFeatures();
    if (mCPUInformation.Model == CPU_MODEL_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU family information
    mCPUInformation.Family = DetermineCPUIntelFamily();
    if (mCPUInformation.Family == CPU_FAMILY_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU frequency information
    DetermineCPUIntelFrequency();
  }

# if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  else if (mCPUInformation.Vendor == CPU_VENDOR_AMD) {
    // Get CPU model information
    mCPUInformation.Model = DetermineCPUAMDModelAndFeatures();
    if (mCPUInformation.Model == CPU_MODEL_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU family information
    mCPUInformation.Family = DetermineCPUAMDFamily();
    if (mCPUInformation.Family == CPU_FAMILY_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU frequency information
    DetermineCPUAMDFrequency();
  }

# endif

  // TODO: Update CPU information with information from SMBIOS

#endif

  // Install CPU information
  mCPUInformationHandle = NULL;
  return gBS->InstallMultipleProtocolInterfaces(&mCPUInformationHandle, &mCPUInformationLibGuid, (VOID *)&mCPUInformation, NULL);
}

// CPULibFinish
/// CPU library finish use
/// @return Whether the library was finished successfully or not
/// @retval EFI_SUCCESS The GUI successfully finished
EFI_STATUS
EFIAPI
CPULibFinish (
  VOID
) {
  // Uninstall CPU information
  if (mCPUInformationHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces(mCPUInformationHandle, &mCPUInformationLibGuid, (VOID *)&mCPUInformation, NULL);
    mCPUInformationHandle = NULL;
  }
  return EFI_SUCCESS;
}

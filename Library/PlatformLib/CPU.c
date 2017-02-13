///
/// @file Library/PlatformLib/CPU.c
///
/// CPU information
///

#include "Platform.h"

#include <Library/ConfigLib.h>
#include <Library/SmBiosLib.h>

// CPU_FEATURE_PRINT
/// Print a CPU package feature and value
#define CPU_FEATURE_PRINT(Feature) LOG3(33, L"      " L ## #Feature L":", L"%u\n", mSystemInformation.Packages[Index].Features.Feature)

// GetPackageFrequency
/// Get CPU package frequency
/// @return The CPU package frequency
STATIC UINT64
EFIAPI
GetPackageFrequency (
  UINTN Index
) {
  if (Index >= mSystemInformation.PackageCount) {
    return 0;
  }
  return RShiftU64(MultU64x32(mSystemInformation.Packages[Index].Frequency.Clock,
                              (UINT32)(mSystemInformation.Packages[Index].Frequency.Maximum)),
                              (UINTN)mSystemInformation.Packages[Index].Frequency.Step);
}
// PrintCPUInformation
/// Print CPU package information
VOID
EFIAPI
PrintCPUInformation (
  VOID
) {
  UINT32           Index;
  // Print package information
  Log2(L"Packages:", L"%u\n", mSystemInformation.PackageCount);
  Log2(L"  Total threads:", L"%u\n", mSystemInformation.LogicalCount);
  Log2(L"  Total cores:", L"%u\n", mSystemInformation.CoreCount);
  // Print information about each package
  for (Index = 0; Index < mSystemInformation.PackageCount; ++Index) {
    UINT32 Width = (UINT32)Log(L"  Package(%u)", Index);
    Log3(LOG_PREFIX_WIDTH - Width, L":", L"%a\n", mSystemInformation.Packages[Index].Description);
    Log2(L"    Model:", L"%X\n", mSystemInformation.Packages[Index].Model);
    Log2(L"    Threads:", L"%u\n", mSystemInformation.Packages[Index].LogicalCount);
    Log2(L"    Cores:", L"%u\n", mSystemInformation.Packages[Index].CoreCount);
    Log2(L"    Frequency:", L"%uMHz\n", DivU64x32(GetPackageFrequency(Index), 1000000));
    Log2(L"    Clock:", L"%ukHz\n", DivU64x32(mSystemInformation.Packages[Index].Frequency.Clock, 1000));
    Log2(L"    Minimum:", L"%u\n", mSystemInformation.Packages[Index].Frequency.Minimum);
    Log2(L"    Maximum:", L"%u\n", mSystemInformation.Packages[Index].Frequency.Maximum);
    if (mSystemInformation.Packages[Index].Features.Turbo && (mSystemInformation.Packages[Index].Frequency.TurboCount > 0)) {
      UINT8 i = 1;
      Log2(L"    Turbo:", L"%u", mSystemInformation.Packages[Index].Frequency.Turbo[mSystemInformation.Packages[Index].Frequency.TurboCount - i]);
      while (i++ < mSystemInformation.Packages[Index].Frequency.TurboCount) {
        Log(L"/%u", mSystemInformation.Packages[Index].Frequency.Turbo[mSystemInformation.Packages[Index].Frequency.TurboCount - i]);
      }
      Log(L"\n");
    } else {
      Log2(L"    Turbo:", L"No\n");
    }
    // Print package features
    LOG(L"    Features:\n");
    CPU_FEATURE_PRINT(FPUOnChip);
    CPU_FEATURE_PRINT(VirtualModeEnhancements);
    CPU_FEATURE_PRINT(DebugExtensions);
    CPU_FEATURE_PRINT(PageSizeExtension);
    CPU_FEATURE_PRINT(TimeStampCounter);
    CPU_FEATURE_PRINT(ModelSpecificRegisters);
    CPU_FEATURE_PRINT(PhysicalAddressExtension);
    CPU_FEATURE_PRINT(MachineCheckException);
    CPU_FEATURE_PRINT(CMPXCHG8BInstruction);
    CPU_FEATURE_PRINT(APICOnChip);
    CPU_FEATURE_PRINT(SystemExtensions);
    CPU_FEATURE_PRINT(MemoryTypeRangeRegisters);
    CPU_FEATURE_PRINT(PageGlobal);
    CPU_FEATURE_PRINT(MachineCheckArchitecture);
    CPU_FEATURE_PRINT(ConditionalMove);
    CPU_FEATURE_PRINT(PageAttributeTable);
    CPU_FEATURE_PRINT(PageAddressExtension);
    CPU_FEATURE_PRINT(ProcessorSerialNumber);
    CPU_FEATURE_PRINT(CLFLUSHInstruction);
    CPU_FEATURE_PRINT(DebugStore);
    CPU_FEATURE_PRINT(ACPIThermalMonitor);
    CPU_FEATURE_PRINT(MMXInstructions);
    CPU_FEATURE_PRINT(FastInstructions);
    CPU_FEATURE_PRINT(SSEInstructions);
    CPU_FEATURE_PRINT(SSE2Instructions);
    CPU_FEATURE_PRINT(SelfSnoop);
    CPU_FEATURE_PRINT(HardwareMultiThread);
    CPU_FEATURE_PRINT(ThermalMonitor);
    CPU_FEATURE_PRINT(PendingBreakEnable);
    CPU_FEATURE_PRINT(SSE3Instructions);
    CPU_FEATURE_PRINT(PCLMULQDQInstruction);
    CPU_FEATURE_PRINT(DebugStoreArea64Bit);
    CPU_FEATURE_PRINT(MonitorWait);
    CPU_FEATURE_PRINT(DebugStoreCPL);
    CPU_FEATURE_PRINT(VMExtensions);
    CPU_FEATURE_PRINT(SaferModeExtensions);
    CPU_FEATURE_PRINT(Dynamic);
    CPU_FEATURE_PRINT(ThermalMonitor2);
    CPU_FEATURE_PRINT(SSSE3Instructions);
    CPU_FEATURE_PRINT(L1ContextID);
    CPU_FEATURE_PRINT(DebugInterface);
    CPU_FEATURE_PRINT(FMAExtensions);
    CPU_FEATURE_PRINT(CMPXCHG16BInstruction);
    CPU_FEATURE_PRINT(xTPRUpdateControl);
    CPU_FEATURE_PRINT(PerfmonDebug);
    CPU_FEATURE_PRINT(ProcessContextIdentifiers);
    CPU_FEATURE_PRINT(Prefetch);
    CPU_FEATURE_PRINT(SSE41Instructions);
    CPU_FEATURE_PRINT(SSE42Instructions);
    CPU_FEATURE_PRINT(x2APIC);
    CPU_FEATURE_PRINT(MOVEBEInstruction);
    CPU_FEATURE_PRINT(POPCNTInstruction);
    CPU_FEATURE_PRINT(TimeStampCounterDeadline);
    CPU_FEATURE_PRINT(AESNIInstructions);
    CPU_FEATURE_PRINT(ExtFastInstructions);
    CPU_FEATURE_PRINT(OSExtFastInstructions);
    CPU_FEATURE_PRINT(AVXInstructions);
    CPU_FEATURE_PRINT(FP16BitConversion);
    CPU_FEATURE_PRINT(RDRANDInstruction);
    //
    // Extended features
    //
    CPU_FEATURE_PRINT(FSGSBASEInstructions);
    CPU_FEATURE_PRINT(TimeStampCounterAdjust);
    CPU_FEATURE_PRINT(SoftwareGuardExtensions);
    CPU_FEATURE_PRINT(BitManip1Instructions);
    CPU_FEATURE_PRINT(HardwareLockElision);
    CPU_FEATURE_PRINT(AVX2Instructions);
    CPU_FEATURE_PRINT(FPDPExceptionOnly);
    CPU_FEATURE_PRINT(SupervisorExecutePrevent);
    CPU_FEATURE_PRINT(BitManip2Instructions);
    CPU_FEATURE_PRINT(ERMSBInstructions);
    CPU_FEATURE_PRINT(INVPCIDInstruction);
    CPU_FEATURE_PRINT(TransactSyncExtensions);
    CPU_FEATURE_PRINT(ResourceDirectorMonitor);
    CPU_FEATURE_PRINT(FPUDeprecated);
    CPU_FEATURE_PRINT(MemoryProtectionExtensions);
    CPU_FEATURE_PRINT(ResourceDirectorAllocation);
    CPU_FEATURE_PRINT(RDSEEDInstruction);
    CPU_FEATURE_PRINT(ArithmeticExtensions);
    CPU_FEATURE_PRINT(SupervisorAccessPrevent);
    CPU_FEATURE_PRINT(CLFLUSHOPTInstruction);
    CPU_FEATURE_PRINT(CLWBInstruction);
    CPU_FEATURE_PRINT(ProcessorTrace);
    CPU_FEATURE_PRINT(SHAExtensions);
    CPU_FEATURE_PRINT(PREFETCHWT1Instruction);
    CPU_FEATURE_PRINT(UserModeIntructionPrevent);
    CPU_FEATURE_PRINT(UserModeProtectKeys);
    CPU_FEATURE_PRINT(OSProtectKeys); 
    CPU_FEATURE_PRINT(RDPIDInstruction);
    CPU_FEATURE_PRINT(SoftwareGuardLaunchConfig);
    //
    // Other features
    //
    CPU_FEATURE_PRINT(AHFromFlags64Bit);
    CPU_FEATURE_PRINT(LZCNTInstruction);
    CPU_FEATURE_PRINT(PREFETCHWInstruction);
    CPU_FEATURE_PRINT(SystemExtensions64Bit);
    CPU_FEATURE_PRINT(ExecuteDisableBit);
    CPU_FEATURE_PRINT(GBPageExtension);
    CPU_FEATURE_PRINT(RDTSCPInstruction);
    CPU_FEATURE_PRINT(X64);
    //
    // Additional features
    //
    CPU_FEATURE_PRINT(Mobile);

  }
}

// GetCPUVendor
/// Get CPU vendor information
/// @return The CPU vendor information
CPU_VENDOR
EFIAPI
GetCPUVendor (
  VOID
) {
  return mSystemInformation.Packages[0].Vendor;
}
// GetCPUFamily
/// Get CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
GetCPUFamily (
  VOID
) {
  return mSystemInformation.Packages[0].Family;
}
// GetCPUModel
/// Get CPU model information
/// @return The CPU model information
CPU_MODEL
EFIAPI
GetCPUModel (
  VOID
) {
  return mSystemInformation.Packages[0].Model;
}
// GetCPUFrequency
/// Get CPU frequency
/// @return The CPU frequency
UINT64
EFIAPI
GetCPUFrequency (
  VOID
) {
  return GetPackageFrequency(0);
}
// IsCPUMobile
/// Check if CPU is mobile
BOOLEAN
EFIAPI
IsCPUMobile (
  VOID
) {
  return mSystemInformation.Packages[0].Features.Mobile;
}
// IsCPUBigEndian
/// Check if CPU is little-endian byte order
BOOLEAN
EFIAPI
IsCPUBigEndian (
  VOID
) {
  return !IsCPULittleEndian();
}
// IsCPULittleEndian
/// Check if CPU is little-endian byte order
BOOLEAN
EFIAPI
IsCPULittleEndian (
  VOID
) {
#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)
  return TRUE;
#elif
# error Need to set endianness detection code for this CPU architecture
#endif
}

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

// DetermineCPUIntelModelAndFeatures
/// Determine CPU model and features information
/// @param Index The physical package index
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUIntelModelAndFeatures (
  IN UINT32 Index
);
// DetermineCPUIntelFamily
/// Determine CPU family information
/// @param Index The physical package index
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUIntelFamily (
  IN UINT32 Index
);
// DetermineCPUIntelFrequency
/// Determine CPU frequency information
/// @param Index The physical package index
VOID
EFIAPI
DetermineCPUIntelFrequency (
  IN UINT32 Index
);

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

// DetermineCPUAMDModelAndFeatures
/// Determine CPU model and features information
/// @param Index The physical package index
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUAMDModelAndFeatures (
  IN UINT32 Index
);
// DetermineCPUAMDFamily
/// Determine CPU family information
/// @param Index The physical package index
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUAMDFamily (
  IN UINT32 Index
);
// DetermineCPUAMDFrequency
/// Determine CPU frequency information
/// @param Index The physical package index
VOID
EFIAPI
DetermineCPUAMDFrequency (
  IN UINT32 Index
);

#endif

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

// UpdatePackageInformation
/// Update package information
/// @param Index The physical package index
EFI_STATUS
EFIAPI
UpdatePackageInformation (
  IN UINT32 Index
) {
  // Get CPU vendor information
  mSystemInformation.Packages[Index].Vendor = DetermineCPUVendor();
  if (mSystemInformation.Packages[Index].Vendor == CPU_VENDOR_UNKNOWN) {
    return EFI_UNSUPPORTED;
  }

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

  if (mSystemInformation.Packages[Index].Vendor == CPU_VENDOR_INTEL) {
    // Get CPU model information
    mSystemInformation.Packages[Index].Model = DetermineCPUIntelModelAndFeatures(Index);
    if (mSystemInformation.Packages[Index].Model == CPU_MODEL_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU family information
    mSystemInformation.Packages[Index].Family = DetermineCPUIntelFamily(Index);
    if (mSystemInformation.Packages[Index].Family == CPU_FAMILY_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU frequency information
    DetermineCPUIntelFrequency(Index);
  }

# if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  else if (mSystemInformation.Packages[Index].Vendor == CPU_VENDOR_AMD) {
    // Get CPU model information
    mSystemInformation.Packages[Index].Model = DetermineCPUAMDModelAndFeatures(Index);
    if (mSystemInformation.Packages[Index].Model == CPU_MODEL_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU family information
    mSystemInformation.Packages[Index].Family = DetermineCPUAMDFamily(Index);
    if (mSystemInformation.Packages[Index].Family == CPU_FAMILY_UNKNOWN) {
      return EFI_UNSUPPORTED;
    }
    // Get CPU frequency information
    DetermineCPUAMDFrequency(Index);
  }

# endif

#endif

  return EFI_SUCCESS;
}

// UpdatePackagesInformation
/// Update all packages information
VOID
EFIAPI
UpdatePackagesInformation (
  VOID
) {
  SMBIOS_STRUCTURE **Tables = NULL;
  UINTN              Count = 0;
  UINTN              Index;

  // Check if CPU information should be updated by SMBIOS
  if (ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override", FALSE) ||
      ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU", FALSE) ||
      ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU\\Clock", FALSE) ||
      ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU\\Frequency", FALSE)) {
    // Update CPU information with information from SMBIOS
    Count = 0;
    Tables = NULL;
    if (!EFI_ERROR(FindSmBiosTables(SMBIOS_TYPE_PROCESSOR_INFORMATION, &Count, &Tables)) && (Tables != NULL)) {
      UINT32 PkgIndex = 0;
      // Iterate through the processor information tables
      for (Index = 0; Index < Count; ++Index) {
        if (Tables[Index] != NULL) {
          SMBIOS_TABLE_TYPE4 *Type4 = (SMBIOS_TABLE_TYPE4 *)Tables[Index];
          // Check processor tray is populated and enabled
          if ((CPUBitmask(Type4->Status, 6, 6) == 1) && (CPUBitmask(Type4->Status, 0, 2) == 1) && (Type4->ProcessorType == CentralProcessor)) {
            // Adjust package information
            if ((Type4->ExternalClock != 0) &&
                (ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override", FALSE) ||
                 ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU", FALSE) ||
                 ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU\\Clock", FALSE))) {
              // Adjust package clock
              mSystemInformation.Packages[PkgIndex].Frequency.Clock = (Type4->ExternalClock * 1000000);
            }
            if (ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override", FALSE) ||
                ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU", FALSE) ||
                ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU\\Frequency", FALSE)) {
              // Adjust package frequency
              UINT64 Frequency = LShiftU64(MultU64x32((UINT64)Type4->CurrentSpeed, 1000000), mSystemInformation.Packages[PkgIndex].Frequency.Step);
              mSystemInformation.Packages[PkgIndex].Frequency.Maximum = (UINT8)DivU64x64Remainder(Frequency, mSystemInformation.Packages[PkgIndex].Frequency.Clock, NULL);
            }
            // Increment package index
            ++PkgIndex;
          }
        }
      }
      // Set the package count
      mSystemInformation.PackageCount = PkgIndex;
      FreePool(Tables);
    }
  }
}

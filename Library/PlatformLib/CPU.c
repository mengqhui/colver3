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
#define CPU_FEATURE_PRINT(Feature) LOG3(33, L"      " L ## #Feature L":", L"%a\n", ConfigSGetBooleanWithDefault(L"\\CPU\\Package\\%u\\Feature\\" L ## #Feature, FALSE, Index) ? "true" : "false")

// mCPUIDRegisters
/// Registers for CPUID results
UINT32 mCPUIDRegisters[4] = { 0 };
// mCPUMaxIndex
/// CPU maximum CPUID index
UINT32 mCPUMaxIndex = 0;
// mCPUMaxExtIndex
/// CPU maximum extended CPUID index
UINT32 mCPUMaxExtIndex = 0x80000000;

// GetPackageFrequency
/// Get CPU package frequency
/// @return The CPU package frequency
STATIC UINT64
EFIAPI
GetPackageFrequency (
  UINTN Index
) {
  return RShiftU64(MultU64x64(ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Clock", 0, Index),
                              ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Maximum", 1, Index)),
                              ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Step", 0, Index));
}
// PrintCPUInformation
/// Print CPU package information
VOID
EFIAPI
PrintCPUInformation (
  VOID
) {
  UINTN Index;
  UINTN PackageCount;
  UINTN TurboCount;
  // Print package information
  Log2(L"Total threads:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Threads", 1));
  Log2(L"Total cores:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Cores", 1));
  PackageCount = ConfigGetUnsignedWithDefault(L"\\CPU\\Count", 1);
  Log2(L"Packages:", L"%u\n", PackageCount);
  // Print information about each package
  for (Index = 0; Index < PackageCount; ++Index) {
    Log3(LOG_PREFIX_WIDTH - Log(L"  Package(%u)", Index), L":", L"%s\n", ConfigSGetStringWithDefault(L"\\CPU\\Package\\%u\\Description", L"Unknown CPU", Index));
    Log2(L"    Model:", L"%X\n", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Model", 0, Index));
    Log2(L"    Threads:", L"%u\n", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Threads", 1, Index));
    Log2(L"    Cores:", L"%u\n", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Threads", 1, Index));
    Log2(L"    Frequency:", L"%uMHz\n", DivU64x32(GetPackageFrequency(Index), 1000000));
    Log2(L"    Clock:", L"%ukHz\n", DivU64x32(ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Clock", 1, Index), 1000));
    Log2(L"    Minimum:", L"%u\n", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Minimum", 1, Index));
    Log2(L"    Maximum:", L"%u\n", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Maximum", 1, Index));
    TurboCount = ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Turbo\\Count", 0, Index);
    if (TurboCount > 0) {
      UINTN TurboIndex = 0;
      Log2(L"    Turbo:", L"%u", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Turbo\\%u", 1, Index, TurboIndex));
      while (++TurboIndex < TurboCount) {
        Log(L"/%u", ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Turbo\\%u", 1, Index, TurboIndex));
      }
      Log(L"\n");
    } else {
      Log2(L"    Turbo:", L"false\n");
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
  return (CPU_VENDOR)ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\0\\Vendor", CPU_VENDOR_UNKNOWN);
}
// GetCPUFamily
/// Get CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
GetCPUFamily (
  VOID
) {
  return (CPU_FAMILY)ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\0\\Family", CPU_FAMILY_UNKNOWN);
}
// GetCPUModel
/// Get CPU model information
/// @return The CPU model information
CPU_MODEL
EFIAPI
GetCPUModel (
  VOID
) {
  return (CPU_MODEL)ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\0\\Model", CPU_MODEL_UNKNOWN);
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
  return ConfigGetBooleanWithDefault(L"\\CPU\\Package\\0\\Feature\\Mobile", FALSE);
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
  return TRUE;
}

// UpdateIntelPackageInformation
/// Update Intel package information
/// @param Index The physical package index
EFI_STATUS
EFIAPI
UpdateIntelPackageInformation (
  IN UINTN Index
);

// UpdateAMDPackageInformation
/// Update AMD package information
/// @param Index The physical package index
EFI_STATUS
EFIAPI
UpdateAMDPackageInformation (
  IN UINTN Index
);

// DetermineCPUVendor
/// Determine CPU vendor information
/// @return The CPU vendor information
STATIC CPU_VENDOR
EFIAPI
DetermineCPUVendor (
  VOID
) {

  // Perform CPUID
  AsmCpuid(0x80000000, mCPUIDRegisters, mCPUIDRegisters + 1, mCPUIDRegisters + 3, mCPUIDRegisters + 2);
  mCPUMaxExtIndex = mCPUIDRegisters[0];
  AsmCpuid(0, mCPUIDRegisters, mCPUIDRegisters + 1, mCPUIDRegisters + 3, mCPUIDRegisters + 2);
  mCPUMaxIndex = mCPUIDRegisters[0];
  // Intel
  if (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"GenuineIntel", 12) == 0) {
    return CPU_VENDOR_INTEL;
  }
  // AMD
  if ((CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"AuthenticAMD", 12) == 0) ||
      (CompareMem((CONST VOID *)(mCPUIDRegisters + 1), (CONST VOID *)"AMDisbetter!", 12) == 0)) {
    return CPU_VENDOR_AMD;
  }

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

  return CPU_VENDOR_UNKNOWN;
}

// UpdatePackageInformation
/// Update package information
/// @param Index The physical package index
EFI_STATUS
EFIAPI
UpdatePackageInformation (
  IN UINTN Index
) {
  EFI_STATUS Status;

  // Get CPU vendor information
  CPU_VENDOR Vendor = DetermineCPUVendor();
  if (Vendor == CPU_VENDOR_UNKNOWN) {
    return EFI_UNSUPPORTED;
  }

  // Set vendor for this package in configuration
  Status = ConfigSSetUnsigned(L"\\CPU\\Package\\%u\\Vendor", (UINTN)Vendor, Index);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (Vendor == CPU_VENDOR_INTEL) {
    // Get Intel CPU information
    return UpdateIntelPackageInformation(Index);
  } else if (Vendor == CPU_VENDOR_AMD) {
    // Get AMD CPU information
    return UpdateAMDPackageInformation(Index);
  }

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
      UINTN PkgIndex = 0;
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
              ConfigSSetUnsigned(L"\\CPU\\Package\\%u\\Clock", Type4->ExternalClock * 1000000, PkgIndex);
            }
            if (ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override", FALSE) ||
                ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU", FALSE) ||
                ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\CPU\\Frequency", FALSE)) {
              // Adjust package frequency
              UINT64 Frequency = LShiftU64(MultU64x32((UINT64)Type4->CurrentSpeed, 1000000), ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Step", 0, PkgIndex));
              ConfigSSetUnsigned(L"\\CPU\\Package\\%u\\Maximum",
                                (UINTN)DivU64x64Remainder(Frequency, ConfigSGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Clock", (UINTN)Frequency, PkgIndex), NULL), PkgIndex);
            }
            // Increment package index
            ++PkgIndex;
          }
        }
      }
      // Set the package count
      ConfigSetUnsigned(L"\\CPU\\Count", PkgIndex);
      FreePool(Tables);
    }
  }
}

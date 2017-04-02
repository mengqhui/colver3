///
/// @file Library/PlatformLib/CPU/X86.c
///
/// X86 CPU information (IA32/X64)
///

#include "../Platform.h"

#include <Library/ConfigLib.h>
#include <Library/SmBiosLib.h>

// CPU_FEATURE_PRINT
/// Print a CPU package feature and value
#define CPU_FEATURE_PRINT(Feature) LOG3(36, L"        " L ## #Feature L":", L"%a\n", ConfigGetBooleanWithDefault(L"\\CPU\\Package\\%u\\Feature\\" L ## #Feature, FALSE, Index) ? "true" : "false")

// mCPUIDRegisters
/// Registers for CPUID results
UINT32 mCPUIDRegisters[4] = { 0 };
// mCPUMaxIndex
/// CPU maximum CPUID index
UINT32 mCPUMaxIndex = 0;
// mCPUMaxExtIndex
/// CPU maximum extended CPUID index
UINT32 mCPUMaxExtIndex = 0x80000000;

// mCPUVendorStrings
/// CPU vendor strings
STATIC CHAR16 *mCPUVendorStrings[] = {

  // Unknown CPU vendor
  L"Unknown",

  // Intel CPU vendor
  L"Intel",
  // AMD CPU vendor
  L"AMD",

  // KVM virtual machine
  L"KVM",
  // Microsoft Hyper-V/Windows Virtual PC virtual machine
  L"Microsoft",
  // Parallels virtual machine
  L"Parallels",
  // VMWare virtual machine
  L"VMWare",
  // Xen virtual machine
  L"Xen"

};
// mCPUFamilyStrings
/// CPU family strings
STATIC CHAR16 *mCPUFamilyStrings[] = {

  // Unknown CPU family
  L"Unknown",

#if defined(MDE_CPU_IA32)

  // CPU_FAMILY_486
  /// 486 compatible CPU family
  L"i486",

  // Intel Pentium compatible CPU family
  L"Pentium",
  // Intel Mobile Pentium compatible CPU family
  L"Mobile Pentium",
  // Intel Pentium compatible CPU family
  L"Pentium Overdrive",

  // Intel Pentium MMX compatible CPU family
  L"Pentium MMX",
  // Intel Mobile Pentium MMX compatible CPU family
  L"Mobile Pentium MMX",
  // Intel Pentium MMX compatible CPU family
  L"Pentium MMX Overdrive",

  // Intel Pentium Pro compatible CPU family
  L"Pentium Pro",

  // Intel Pentium II compatible CPU family
  L"Pentium II",
  // Intel Mobile Pentium II compatible CPU family
  L"Mobile Pentium II",
  // Intel Pentium II Overdrive compatible CPU family
  L"Pentium II Overdrive",

  // Intel Pentium III compatible CPU family
  L"Pentium III",

  // Intel Pentium III Xeon compatible CPU family
  "Pentium III Xeon",

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  // Intel Pentium 4 compatible CPU family
  L"Pentium 4",

  // Intel Pentium D compatible CPU family
  L"Pentium D",

  // Intel Pentium M compatible CPU family
  L"Pentium M",

  // Intel Celeron compatible CPU family
  L"Celeron",

  // Intel Celeron D compatible CPU family
  L"Celeron D",

  // Intel Core compatible CPU family
  L"Core",

  // Intel Core2 compatible CPU family
  L"Core2",
  // Intel Core2 Mobile compatible CPU family
  L"Core2 Mobile",

  // Intel Core i compatible CPU family
  L"1st Gen Core i",
  // Intel 2nd Gen Core i compatible CPU family
  L"2dn Gen Core i",
  // Intel 3rd Gen Core i compatible CPU family
  L"3rd Gen Core i",
  // Intel 4th Gen Core i compatible CPU family
  L"4th Gen Core i",
  // Intel 5th Gen Core i compatible CPU family
  L"5th Gen Core i",
  // Intel 6th Gen Core i compatible CPU family
  L"6th Gen Core i",
  // Intel 7th Gen Core i compatible CPU family
  L"7th Gen Core i",

  // Intel Core i Mobile compatible CPU family
  L"1st Gen Core i Mobile",
  // Intel2nd Gen Core i Mobile compatible CPU family
  L"2nd Gen Core i Mobile",
  // Intel 3rd Gen Core i Mobile compatible CPU family
  L"3rd Gen Core i Mobile",
  // Intel 4th Gen Core i Mobile compatible CPU family
  L"4th Gen Core i Mobile",
  // Intel 5th Gen Core i Mobile compatible CPU family
  L"5th Gen Core i Mobile",
  // Intel 6th Gen Core i Mobile compatible CPU family
  L"6th Gen Core i Mobile",
  // Intel 7th Gen Core i Mobile compatible CPU family
  L"7th Gen Core i Mobile",

  // Intel Xeon compatible CPU family
  L"Xeon",
  // Intel Xeon MP compatible CPU family
  L"Xeon MP",
  // Intel Xeon Core2 compatible CPU family
  L"Core2 Xeon",
  // Intel Xeon Core i compatible CPU family
  L"1st Gen Core i Xeon",
  // Intel Xeon 2nd Gen Core i compatible CPU family
  L"2nd Gen Core i Xeon",
  // Intel Xeon 3rd Gen Core i compatible CPU family
  L"3rd Gen Core i Xeon",
  // Intel Xeon 4th Gen Core i compatible CPU family
  L"4th Gen Core i Xeon",
  // Intel Xeon 5th Gen Core i compatible CPU family
  L"5th Gen Core i Xeon",
  // Intel Xeon 6th Gen Core i compatible CPU family
  L"6th Gen Core i Xeon",
  // Intel Xeon 7th Gen Core i compatible CPU family
  L"7th Gen Core i Xeon",

  // CPU_FAMILY_XEON_PHI
  /// Intel Xeon Phi compatible CPU family
  L"Xeon Phi",

  // CPU_FAMILY_ATOM
  /// Intel Atom compatible CPU family
  L"1st Gen Atom",
  // Intel 2nd Gen Atom compatible CPU family
  L"2nd Gen Atom",
  // Intel 3rd Gen Atom compatible CPU family
  L"3rd Gen Atom",
  // Intel 4th Gen Atom compatible CPU family
  L"4th Gen Atom",

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_IPF)

  // Intel Itanium compatible CPU family
  L"Itanium",
  // Intel Itanium 2 compatible CPU family
  L"Itanium 2",

#endif

};

// GetCPUVendorString
/// Get the CPU vendor string
/// @param Vendor The CPU vendor identifier
/// @return The CPU vendor string
CHAR16 *
GetCPUVendorString (
  CPU_VENDOR Vendor
) {
  return mCPUVendorStrings[(Vendor < ARRAY_SIZE(mCPUVendorStrings)) ? Vendor : 0];
}
// GetCPUFamilyString
/// Get the CPU family string
/// @param Family The CPU family identifier
/// @return The CPU family string
CHAR16 *
GetCPUFamilyString (
  CPU_FAMILY Family
) {
  return mCPUFamilyStrings[(Family < ARRAY_SIZE(mCPUFamilyStrings)) ? Family : 0];
}

// GetPackageFrequency
/// Get CPU package frequency
/// @return The CPU package frequency
STATIC UINT64
EFIAPI
GetPackageFrequency (
  UINTN Index
) {
  return RShiftU64(MultU64x64(ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Clock", 100000000, Index),
                              ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Maximum", 1, Index)),
                              ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Step", 0, Index));
}
// PrintCPUInformation
/// Print CPU package information
VOID
EFIAPI
PrintCPUInformation (
  VOID
) {
  UINTN Index;
  UINTN Vendor;
  UINTN Family;
  UINTN PackageCount;
  UINTN TurboCount;
  // Print total physical thread and core information
  Log2(L"  Total threads:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Threads", 1));
  Log2(L"  Total cores:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Cores", 1));
  // Print package information
  PackageCount = ConfigGetUnsignedWithDefault(L"\\CPU\\Count", 1);
  Log2(L"  Total packages:", L"%u\n", PackageCount);
  // Print information about each package
  for (Index = 0; Index < PackageCount; ++Index) {
    Vendor = ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Vendor", CPU_VENDOR_UNKNOWN, Index);
    Log2(L"    Package:", L"%u\n", Index);
    Log2(L"      Vendor:", L"%s (%u)\n", GetCPUVendorString(Vendor), Vendor);
    Log2(L"      Description:", L"%s\n", ConfigGetStringWithDefault(L"\\CPU\\Package\\%u\\Description", L"Unknown CPU", Index));
    Family = ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Family", CPU_FAMILY_UNKNOWN, Index);
    Log2(L"      Family:", L"%s (%u)\n", GetCPUFamilyString(Family), Family);
    Log2(L"      Model:", L"%X\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Model", CPU_MODEL_UNKNOWN, Index));
    Log2(L"      Threads:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Threads", 1, Index));
    Log2(L"      Cores:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Cores", 1, Index));
    Log2(L"      Frequency:", L"%u MHz\n", DivU64x32(GetPackageFrequency(Index), 1000000));
    Log2(L"      Clock:", L"%u MHz\n", DivU64x32(ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Clock", 1, Index), 1000000));
    Log2(L"      Minimum:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Minimum", 1, Index));
    Log2(L"      Maximum:", L"%u\n", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Maximum", 1, Index));
    TurboCount = ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Turbo\\Count", 0, Index);
    if (TurboCount > 0) {
      UINTN TurboIndex = 0;
      Log2(L"      Turbo:", L"%u", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Turbo\\%u", 1, Index, TurboIndex));
      while (++TurboIndex < TurboCount) {
        Log(L"/%u", ConfigGetUnsignedWithDefault(L"\\CPU\\Package\\%u\\Turbo\\%u", 1, Index, TurboIndex));
      }
      Log(L"\n");
    } else {
      Log2(L"      Turbo:", L"false\n");
    }
    // Print package features
    LOG(L"      Features:\n");
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
  Status = ConfigSetUnsigned(L"\\CPU\\Package\\%u\\Vendor", (UINTN)Vendor, TRUE, Index);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  LOG2(L"  Package:", L"%u\n", Index);
  LOG2(L"    Vendor:", L"%s (%u)\n", GetCPUVendorString(Vendor), Vendor);
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
  SMBIOS_STRUCTURE_POINTER *Tables;
  CHAR16                   *String;
  CHAR8                    *Ascii;
  UINTN                     Count;
  UINTN                     Index;
  UINTN                     Clock = 0;
  UINTN                     Minimum = 0;
  UINTN                     Maximum = 0;

  // Update CPU information with information from SMBIOS
  Count = 0;
  Tables = NULL;
  if (!EFI_ERROR(FindSmBiosTables(SMBIOS_TYPE_PROCESSOR_INFORMATION, &Count, &Tables)) && (Tables != NULL)) {
    // Iterate through the processor information tables
    LOG2(L"  SMBIOS packages:", L"%u\n", Count);
    for (Index = 0; Index < Count; ++Index) {
      SMBIOS_STRUCTURE_POINTER Table = Tables[Index];
      if (Table.Hdr != NULL) {
        // Check processor tray is populated and enabled
        if ((CPUBitmask(Table.Type4->Status, 6, 6) == 1) && (CPUBitmask(Table.Type4->Status, 0, 2) == 1) && (Table.Type4->ProcessorType == CentralProcessor)) {
          // Adjust package clock
          LOG2(L"    Package:", L"%u\n", Index);
          // Get the processor status
          LOG2(L"      Status:", L"0x%02X\n", Table.Type4->Status);
          LOG2(L"      Threads:", L"%u\n", Table.Type4->ThreadCount);
          LOG2(L"      Cores:", L"%u\n", Table.Type4->CoreCount);
          // Get the processor socket designation
          Ascii = GetStringFromSmBiosTable(Table, Table.Type4->Socket);
          if (Ascii != NULL) {
            LOG2(L"      Socket:", L"%a\n", Ascii);
          }
          // Get the processor socket upgrade type
          String = GetSmBiosProcessorUpgrade(Table.Type4->ProcessorUpgrade);
          if (String != NULL) {
            LOG2(L"      Type:", L"%s\n", String);
          }
          // Get the processor manufacturer
          Ascii = GetStringFromSmBiosTable(Table, Table.Type4->ProcessorManufacture);
          if (Ascii != NULL) {
            LOG2(L"      Vendor:", L"%a\n", Ascii);
          }
          // Get the processor description
          Ascii = GetStringFromSmBiosTable(Table, Table.Type4->ProcessorVersion);
          if (Ascii != NULL) {
            LOG2(L"      Description:", L"%a\n", Ascii);
          }
          // Get the processor serial number
          Ascii = GetStringFromSmBiosTable(Table, Table.Type4->SerialNumber);
          if (Ascii != NULL) {
            LOG2(L"      Serial number:", L"%a\n", Ascii);
          }
          // Get the processor part number
          Ascii = GetStringFromSmBiosTable(Table, Table.Type4->PartNumber);
          if (Ascii != NULL) {
            LOG2(L"      Part number:", L"%a\n", Ascii);
          }
          // Get the processor asset tag
          Ascii = GetStringFromSmBiosTable(Table, Table.Type4->AssetTag);
          if (Ascii != NULL) {
            LOG2(L"      Asset tag:", L"%a\n", Ascii);
          }
          // Get the package clock
          if (Table.Type4->ExternalClock != 0) {
            LOG2(L"      Clock:", L"%u MHz\n", Table.Type4->ExternalClock);
            Clock = Table.Type4->ExternalClock * 1000000;
          }
          if (Clock == 0) {
            Clock = 100000000;
          }
          ConfigSetUnsigned(L"\\CPU\\Package\\%u\\Clock", Clock, TRUE, Index);
          // Get the package speed
          if (Table.Type4->CurrentSpeed != 0) {
            Minimum = ((((UINTN)Table.Type4->CurrentSpeed) * 1000000) / Clock);
          }
          // Get the package speed
          if (Table.Type4->MaxSpeed != 0) {
            Maximum = ((((UINTN)Table.Type4->CurrentSpeed) * 1000000) / Clock);
          }
          if (Minimum == 0) {
            if (Maximum == 0) {
              Minimum = Maximum = 1;
            } else {
              Minimum = Maximum;
            }
          } else if (Maximum == 0) {
            Maximum = Minimum;
          }
          LOG2(L"      Minimum:", L"%u\n", Minimum);
          ConfigSetUnsigned(L"\\CPU\\Package\\%u\\Minimum", Minimum, FALSE, Index);
          LOG2(L"      Maximum:", L"%u\n", Maximum);
          ConfigSetUnsigned(L"\\CPU\\Package\\%u\\Maximum", Maximum, FALSE, Index);
        }
      }
    }
    // Set the package count
    ConfigSetUnsigned(L"\\CPU\\Count", Count, TRUE);
    FreePool(Tables);
  }
}

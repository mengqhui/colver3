///
/// @file Library/PlatformLib/Intel.c
///
/// Platform library Intel CPUs
///

#include "Platform.h"

#include <Library/TimerLib.h>

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

// CPU_SAMPLE_COUNT
/// The number of samples for TSC calibration
#define CPU_SAMPLE_COUNT 10
// CPU_SAMPLE_MICROSECONDS
/// The microseconds to wait being taking each sample for TSC calibration
#define CPU_SAMPLE_MICROSECONDS 30

// DetectCPUFrequency
/// Detect CPU frequency by inspecting the TSC
/// @return The approximate frequency of the CPU in Hz
UINT64
EFIAPI
DetectCPUFrequency (
  VOID
) {
  UINTN  Index;
  UINT64 Samples = 0;
  for (Index = 0; Index < CPU_SAMPLE_COUNT; ++Index) {
    // Get a sample of ticks per delay
    UINT64 Start = AsmReadTsc();
    MicroSecondDelay(CPU_SAMPLE_MICROSECONDS);
    Samples += (AsmReadTsc() - Start);
  }
  return DivU64x32(MultU64x32(Samples, 1000000), CPU_SAMPLE_COUNT * CPU_SAMPLE_MICROSECONDS);
}

// SwitchCPUFamilyToMobile
/// Switch CPU family information to mobile version
STATIC CPU_FAMILY
EFIAPI
SwitchCPUIntelFamilyToMobile (
  IN CPU_FAMILY Family
) {

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  switch (Family) {

#if defined(MDE_CPU_IA32)

    case CPU_FAMILY_PENTIUM:
      return CPU_FAMILY_MOBILE_PENTIUM;

    case CPU_FAMILY_PENTIUM_MMX:
      return CPU_FAMILY_MOBILE_PENTIUM_MMX;

    case CPU_FAMILY_PENTIUM_II:
      return CPU_FAMILY_MOBILE_PENTIUM_II;

#endif

    case CPU_FAMILY_CORE2:
      return CPU_FAMILY_CORE2_MOBILE;

    case CPU_FAMILY_CORE_i:
      return CPU_FAMILY_CORE_i_MOBILE;

    case CPU_FAMILY_2ND_CORE_i:
      return CPU_FAMILY_2ND_CORE_i_MOBILE;

    case CPU_FAMILY_3RD_CORE_i:
      return CPU_FAMILY_3RD_CORE_i_MOBILE;

    case CPU_FAMILY_4TH_CORE_i:
      return CPU_FAMILY_4TH_CORE_i_MOBILE;

    case CPU_FAMILY_5TH_CORE_i:
      return CPU_FAMILY_5TH_CORE_i_MOBILE;

    case CPU_FAMILY_6TH_CORE_i:
      return CPU_FAMILY_6TH_CORE_i_MOBILE;

    case CPU_FAMILY_7TH_CORE_i:
      return CPU_FAMILY_7TH_CORE_i_MOBILE;

    default:
      break;
  }

#endif

  return Family;
}

// DetermineCPUIntelFeatures
/// Determine CPU features information
/// @param Index The physical package index
STATIC VOID
EFIAPI
DetermineCPUIntelFeatures (
  IN UINTN Index
) {
  // Floating Point Unit On-Chip, the CPU contains an FPU
  mSystemInformation.Packages[Index].Features.FPUOnChip                 = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 0, 0);
  // Virtual 8086 mode enhancements, including CR4.VME for controlling the feature, CR4.PVI for
  // protected mode virtual interrupts, software interrupt indirection, expansion of the TSS
  // with the software indirection bitmap, and EFLAGS.VIF and EFLAGS.VIP flags
  mSystemInformation.Packages[Index].Features.VirtualModeEnhancements   = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 1, 1);
  // Support for I/O breakpoints, including CR4.DE for controlling the feature, and optional trapping of accesses to DR4 and DR5
  mSystemInformation.Packages[Index].Features.DebugExtensions           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 2, 2);
  // Large pages of size 4 MByte are supported, including CR4.PSE for controlling the feature, the defined
  // dirty bit in PDE (Page Directory Entries), optional reserved bit trapping in CR3, PDEs, and PTEs
  mSystemInformation.Packages[Index].Features.PageSizeExtension         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 3, 3);
  // The RDTSC instruction is supported, including CR4.TSD for controlling privilege
  mSystemInformation.Packages[Index].Features.TimeStampCounter          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 4, 4);
  // The RDMSR and WRMSR instructions are supported
  mSystemInformation.Packages[Index].Features.ModelSpecificRegisters    = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 5, 5);
  // Physical addresses greater than 32 bits are supported: extended page table entry formats, an extra level
  // in the page translation tables is defined, 2-MByte pages are supported instead of 4-Mbyte pages
  mSystemInformation.Packages[Index].Features.PhysicalAddressExtension  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 6, 6);
  // Exception 18 is defined for Machine Checks, including CR4.MCE for controlling the feature
  mSystemInformation.Packages[Index].Features.MachineCheckException     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 7, 7);
  // The compare-and-exchange 8 bytes (64 bits) instruction is supported (implicitly locked and atomic)
  mSystemInformation.Packages[Index].Features.CMPXCHG8BInstruction      = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 8, 8);
  // The processor contains an Advanced Programmable Interrupt Controller (APIC), responding to
  // memory mapped commands in the physical address range FFFE0000H to FFFE0FFFH (by default)
  mSystemInformation.Packages[Index].Features.APICOnChip                = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 9, 9);
  // The SYSENTER and SYSEXIT and associated MSRs are supported
  mSystemInformation.Packages[Index].Features.SystemExtensions          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 11, 11);
  // MTRRs are supported, the MTRRcap MSR contains feature bits that describe what memory types
  // are supported, how many variable MTRRs are supported, and whether fixed MTRRs are supported
  mSystemInformation.Packages[Index].Features.MemoryTypeRangeRegisters  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 12, 12);
  // The global bit is supported in paging-structure entries that map a page, indicating TLB entries that
  // are common to different processes and need not be flushed. The CR4.PGE bit controls this feature
  mSystemInformation.Packages[Index].Features.PageGlobal                = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 13, 13);
  // Machine Check Architecture of reporting machine errors is supported. The MCG_CAP MSR
  // contains feature bits describing how many banks of error reporting MSRs are supported
  mSystemInformation.Packages[Index].Features.MachineCheckArchitecture  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 14, 14);
  // The conditional move instruction CMOV is supported, if FPU is present then the FCOMI and FCMOV instructions are supported
  mSystemInformation.Packages[Index].Features.ConditionalMove           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 15, 15);
  // Page Attribute Table is supported, this feature augments the Memory Type Range Registers (MTRRs), allowing
  // an operating system to specify attributes of memory accessed through a linear address on a 4KB granularity
  mSystemInformation.Packages[Index].Features.PageAttributeTable        = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 16, 16);
  // 4-MByte pages addressing physical memory beyond 4 GBytes are supported with 32-bit paging. This feature
  // indicates that upper bits of the physical address of a 4-MByte page are encoded in bits 20:13 of the
  // page-directory entry. Such physical addresses are limited by MAXPHYADDR and may be up to 40 bits in size
  mSystemInformation.Packages[Index].Features.PageSizeExtension         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 17, 17);
  // The processor supports the 96-bit processor identification number feature and the feature is enabled
  mSystemInformation.Packages[Index].Features.ProcessorSerialNumber     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 18, 18);
  // CLFLUSH Instruction is supported
  mSystemInformation.Packages[Index].Features.CLFLUSHInstruction        = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 19, 19);
  // The processor supports the ability to write debug information into a memory resident buffer
  mSystemInformation.Packages[Index].Features.DebugStore                = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 21, 21);
  // The processor implements internal MSRs that allow processor temperature to be monitored and
  // processor performance to be modulated in predefined duty cycles under software control
  mSystemInformation.Packages[Index].Features.ACPIThermalMonitor        = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 22, 22);
  // The processor supports the Intel MMX technology
  mSystemInformation.Packages[Index].Features.MMXInstructions           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 23, 23);
  // The FXSAVE and FXRSTOR instructions are supported for fast save and restore of the
  // floating point context. Presence of this bit also indicates that CR4.OSFXSR is available
  // for an operating system to indicate that it supports the FXSAVE and FXRSTOR instructions
  mSystemInformation.Packages[Index].Features.FastInstructions          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 24, 24);
  // The processor supports the SSE extensions
  mSystemInformation.Packages[Index].Features.SSEInstructions           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 25, 25);
  // The processor supports the SSE2 extensions
  mSystemInformation.Packages[Index].Features.SSE2Instructions          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 26, 26);
  // The processor supports the management of conflicting memory types by performing
  // a snoop of its own cache structure for transactions issued to the bus
  mSystemInformation.Packages[Index].Features.SelfSnoop                 = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 27, 27);
  // The processor supports hardware multithreading
  mSystemInformation.Packages[Index].Features.HardwareMultiThread       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 28, 28);
  // The processor implements the thermal monitor automatic thermal control circuitry (TCC)
  mSystemInformation.Packages[Index].Features.ThermalMonitor            = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 29, 29);
  // The processor supports the use of the FERR#/PBE# pin when the processor is in the stop-clock state (STPCLK# is asserted)
  // to signal the processor that an interrupt is pending and that the processor should return to normal
  // operation to handle the interrupt. Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability
  mSystemInformation.Packages[Index].Features.PendingBreakEnable        = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 31, 31);
  // Streaming SIMD Extensions 3 (SSE3)
  mSystemInformation.Packages[Index].Features.SSE3Instructions          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 0, 0);
  // Indicates the processor supports the PCLMULQDQ instruction
  mSystemInformation.Packages[Index].Features.PCLMULQDQInstruction      = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 1, 1);
  // Indicates the processor supports DS area using 64-bit layout
  mSystemInformation.Packages[Index].Features.DebugStoreArea64Bit       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 2, 2);
  // Indicates the processor supports MONITOR/MWAIT
  mSystemInformation.Packages[Index].Features.MonitorWait               = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 3, 3);
  // Indicates the processor supports the extensions to the Debug Store feature to allow for branch message storage qualified by CPL
  mSystemInformation.Packages[Index].Features.DebugStoreCPL             = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 4, 4);
  // Indicates that the processor supports this virtual machine extensions
  mSystemInformation.Packages[Index].Features.VMExtensions              = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 5, 5);
  // Indicates that the processor supports Safer Mode
  mSystemInformation.Packages[Index].Features.SaferModeExtensions       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 6, 6);
  // Whether CPU supports dynamic frequency
  mSystemInformation.Packages[Index].Features.Dynamic                   = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 7, 7);
  // Indicates whether the processor supports Thermal Monitor 2
  mSystemInformation.Packages[Index].Features.ThermalMonitor2           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 8, 8);
  // Indicates the presence of the Supplemental Streaming SIMD Extensions 3 (SSSE3)
  mSystemInformation.Packages[Index].Features.SSSE3Instructions         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 9, 9);
  // Indicates the L1 data cache mode can be set to either adaptive mode or shared mode
  mSystemInformation.Packages[Index].Features.L1ContextID               = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 10, 10);
  // The processor supports IA32_DEBUG_INTERFACE MSR for silicon debug
  mSystemInformation.Packages[Index].Features.DebugInterface            = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 11, 11);
  // Indicates the processor supports FMA extensions using YMM state
  mSystemInformation.Packages[Index].Features.FMAExtensions             = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 12, 12);
  // The compare-and-exchange 16 bytes (128 bits) instruction is supported (implicitly locked and atomic)
  mSystemInformation.Packages[Index].Features.CMPXCHG16BInstruction     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 13, 13);
  // Indicates that the processor supports changing IA32_MISC_ENABLE[bit 23]
  mSystemInformation.Packages[Index].Features.xTPRUpdateControl         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 14, 14);
  // Indicates the processor supports the performance and debug feature indication MSR IA32_PERF_CAPABILITIES
  mSystemInformation.Packages[Index].Features.PerfmonDebug              = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 15, 15);
  // Indicates that the processor supports PCIDs and that software may set CR4.PCIDE to 1
  mSystemInformation.Packages[Index].Features.ProcessContextIdentifiers = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 17, 17);
  // Indicates the processor supports the ability to prefetch data from a memory mapped device
  mSystemInformation.Packages[Index].Features.Prefetch                  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 18, 18);
  // Indicates that the processor supports SSE4.1
  mSystemInformation.Packages[Index].Features.SSE41Instructions         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 19, 19);
  // Indicates that the processor supports SSE4.2
  mSystemInformation.Packages[Index].Features.SSE42Instructions         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 20, 20);
  // Indicates that the processor supports x2APIC feature
  mSystemInformation.Packages[Index].Features.x2APIC                    = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 21, 21);
  // Indicates that the processor supports MOVBE instruction
  mSystemInformation.Packages[Index].Features.MOVEBEInstruction         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 22, 22);
  // Indicates that the processor supports the POPCNT instruction
  mSystemInformation.Packages[Index].Features.POPCNTInstruction         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 23, 23);
  // Indicates that the processor’s local APIC timer supports one-shot operation using a TSC deadline value
  mSystemInformation.Packages[Index].Features.TimeStampCounterDeadline  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 24, 24);
  // Indicates that the processor supports the AESNI instruction extensions
  mSystemInformation.Packages[Index].Features.AESNIInstructions         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 25, 25);
  // Indicates that the processor supports the XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV instructions, and XCR0
  mSystemInformation.Packages[Index].Features.ExtFastInstructions       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 26, 26);
  // Indicates that the OS has set CR4.OSXSAVE[bit 18] to enable XSETBV/XGETBV instructions to access XCR0
  // and to support processor extended state management using XSAVE/XRSTOR
  mSystemInformation.Packages[Index].Features.OSExtFastInstructions     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 27, 27);
  // Indicates the processor supports the AVX instruction extensions
  mSystemInformation.Packages[Index].Features.AVXInstructions           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 28, 28);
  // Indicates that processor supports 16-bit floating-point conversion instructions
  mSystemInformation.Packages[Index].Features.FP16BitConversion         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 29, 29);
  // Indicates that processor supports RDRAND instruction
  mSystemInformation.Packages[Index].Features.RDRANDInstruction         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 30, 30);
}
// DetermineCPUIntelExtendedFeatures
/// Determine CPU extended features information
/// @param Index The physical package index
STATIC VOID
EFIAPI
DetermineCPUIntelExtendedFeatures (
  IN UINT32 Index
) {
  // Indicates that processor supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE instructions
  mSystemInformation.Packages[Index].Features.FSGSBASEInstructions      = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 0, 0);
  // Indicates that processor supports IA32_TSC_ADJUST machine specific register
  mSystemInformation.Packages[Index].Features.TimeStampCounterAdjust    = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 1, 1);
  // Indicates that processor supports supports Intel Software Guard Extensions
  mSystemInformation.Packages[Index].Features.SoftwareGuardExtensions   = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 2, 2);
  // Indicates that processor supports supports BMI1 instructions
  mSystemInformation.Packages[Index].Features.BitManip1Instructions     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 3, 3);
  // Indicates that processor supports supports HLE instructions
  mSystemInformation.Packages[Index].Features.HardwareLockElision       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 4, 4);
  // Indicates that processor supports supports AVX2 instructions
  mSystemInformation.Packages[Index].Features.AVX2Instructions          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 5, 5);
  // Indicates floating point unit data pointer updated only on floating point exceptions
  mSystemInformation.Packages[Index].Features.FPDPExceptionOnly         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 6, 6);
  // Indicates that processor supports Supervisor-Mode Execution Prevention
  mSystemInformation.Packages[Index].Features.SupervisorExecutePrevent  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 7, 7);
  // Indicates that processor supports supports RTM extensions
  mSystemInformation.Packages[Index].Features.TransactSyncExtensions    = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 8, 8);
  // Indicates that processor supports Enhanced REP MOVSB/STOSB instructions
  mSystemInformation.Packages[Index].Features.ERMSBInstructions         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 9, 9);
  // Indicates that processor supports INVPCID instruction for system software that manages process-context identifiers
  mSystemInformation.Packages[Index].Features.INVPCIDInstruction        = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 10, 10);
  // Indicates that processor supports supports HLE instructions
  mSystemInformation.Packages[Index].Features.HardwareLockElision       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 11, 11);
  // Indicates that processor supports Intel Resource Director Technology Monitoring capability
  mSystemInformation.Packages[Index].Features.ResourceDirectorMonitor   = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 12, 12);
  // Indicates that processor deprecates FPU CS and FPU DS
  mSystemInformation.Packages[Index].Features.FPUDeprecated             = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 13, 13);
  // Indicates that processor supports Intel Memory Protection Extensions
  mSystemInformation.Packages[Index].Features.MemoryProtectionExtensions= (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 14, 14);
  // Indicates that processor supports Intel Resource Director Technology Allocation
  mSystemInformation.Packages[Index].Features.ResourceDirectorAllocation= (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 15, 15);
  // Indicates that processor supports RDSEED instruction
  mSystemInformation.Packages[Index].Features.RDSEEDInstruction         = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 18, 18);
  // Indicates that processor supports arbitrary precision arithmetic extensions
  mSystemInformation.Packages[Index].Features.ArithmeticExtensions      = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 19, 19);
  // Indicates that processor supports Supervisor-Mode Access Prevention (and the CLAC/STAC instructions)
  mSystemInformation.Packages[Index].Features.SupervisorAccessPrevent   = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 20, 20);
  // Indicates that processor supports CLFLUSHOPT instruction
  mSystemInformation.Packages[Index].Features.CLFLUSHOPTInstruction     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 23, 23);
  // Indicates that processor supports CLWB instruction
  mSystemInformation.Packages[Index].Features.CLWBInstruction           = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 24, 24);
  // Indicates that processor supports Intel Processor Trace
  mSystemInformation.Packages[Index].Features.ProcessorTrace            = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 25, 25);
  // Indicates that processor supports Intel Secure Hash Algorithm Extensions
  mSystemInformation.Packages[Index].Features.SHAExtensions             = (BOOLEAN)CPUBitmask(mCPUIDRegisters[1], 29, 29);
  // Indicates that processor supports PREFETCHWT1 instruction
  mSystemInformation.Packages[Index].Features.PREFETCHWT1Instruction    = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 0, 0);
  // Indicates that processor supports user-mode instruction prevention
  mSystemInformation.Packages[Index].Features.UserModeIntructionPrevent = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 2, 2);
  // Indicates that processor supports protection keys for user-mode pages
  mSystemInformation.Packages[Index].Features.UserModeProtectKeys       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 3, 3);
  // Indicates that OS has set CR4.PKE to enable protection keys (and the RDPKRU/WRPKRU instructions)
  mSystemInformation.Packages[Index].Features.OSProtectKeys             = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 4, 4); 
  // Indicates that processor supports RDPID instruction
  mSystemInformation.Packages[Index].Features.RDPIDInstruction          = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 22, 22);
  // Indicates that processor supports Software Guard Launch Configuration
  mSystemInformation.Packages[Index].Features.SoftwareGuardLaunchConfig = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 30, 30);
}
// DetermineCPUIntelOtherFeatures
/// Determine CPU features information
/// @param Index The physical package index
STATIC VOID
EFIAPI
DetermineCPUIntelOtherFeatures (
  IN UINT32 Index
) {
  // Indicates LAHF/SAHF available in 64-bit mode
  mSystemInformation.Packages[Index].Features.AHFromFlags64Bit      = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 0, 0);
  // Indicates the processor supports the LZCNT instruction
  mSystemInformation.Packages[Index].Features.LZCNTInstruction      = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 5, 5);
  // Indicates the processor supports the PREFETCHW instruction
  mSystemInformation.Packages[Index].Features.PREFETCHWInstruction  = (BOOLEAN)CPUBitmask(mCPUIDRegisters[2], 8, 8);
  // Indicates SYSCALL/SYSRET available in 64-bit mode
  mSystemInformation.Packages[Index].Features.SystemExtensions64Bit = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 11, 11);
  // Indicates Execute Disable Bit available
  mSystemInformation.Packages[Index].Features.ExecuteDisableBit     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 20, 20);
  // Indicates 1-GByte pages are available
  mSystemInformation.Packages[Index].Features.GBPageExtension       = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 26, 26);
  // Indicates RDTSCP instruction and IA32_TSC_AUX machine specific register are available
  mSystemInformation.Packages[Index].Features.RDTSCPInstruction     = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 27, 27);
  // Indicates Intel64 or AMD64 Architecture available
  mSystemInformation.Packages[Index].Features.X64                   = (BOOLEAN)CPUBitmask(mCPUIDRegisters[3], 29, 29);
}
// DetermineCPUIntelModelAndFeatures
/// Determine CPU model and features information
/// @param Index The physical package index
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUIntelModelAndFeatures (
  IN UINT32 Index
) {
  UINT32 Model = CPU_MODEL_UNKNOWN;
  // Check maximum CPUID index
  if (mCPUMaxIndex >= 0x1) {
    UINT32 SubIndex = 0;
    // Clear IA32_BIOS_SIGN_ID model specific register
    AsmWriteMsr64(0x8B, 0);
    // Determine logical and physical CPU counts
    mSystemInformation.Packages[Index].LogicalCount = mSystemInformation.Packages[Index].CoreCount = 1;
    // Check if core topology reporting is supported
    if (mCPUMaxIndex >= 0xB) {
      UINT32 LevelCount = 0;
      // Inspect core topology
      do {
        UINT32 LevelType;
        // Perform CPUID for core topology
        CPUIDEx(0xB, SubIndex);
        // Get the level type
        LevelType = (UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[2], 8, 15);
        if ((LevelType == 0) || (LevelType > 2)) {
          break;
        }
        // Determine what to do with the count based on level type
        switch (LevelType) {

          case 1:
            // SMT level
            LevelCount = (UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[1], 0, 15);
            break;

          case 2:
            // Core level
            if (LevelCount <= 1) {
              mSystemInformation.Packages[Index].LogicalCount = mSystemInformation.Packages[Index].CoreCount = (UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[1], 0, 15);
            } else {
              mSystemInformation.Packages[Index].LogicalCount = (UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[1], 0, 15);
              mSystemInformation.Packages[Index].CoreCount = (mSystemInformation.Packages[Index].LogicalCount / LevelCount);
            }
            break;

          default:
            break;

        }
        ++SubIndex;
      } while (TRUE);
    }
    // Check if cache topology reporting is supported if core topology reporting is not or failed
    if ((mCPUMaxIndex >= 0x4) && (SubIndex == 0)) {
      // Inspect cache topology
      CPUIDEx(0x4, SubIndex);
      if ((mCPUIDRegisters[0] & 0x1F) != 0) {
        // Get the physical CPU count
        mSystemInformation.Packages[Index].CoreCount = (UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[0], 26, 31) + 1;
        // Get the logical CPU count
        mSystemInformation.Packages[Index].LogicalCount = ((UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[0], 14, 25) + 1);
        ++SubIndex;
      }
    }
    // Perform CPUID
    CPUID(0x1);
    // Get the CPU model signature
    Model = mCPUIDRegisters[0];
    // Determine features
    DetermineCPUIntelFeatures(Index);
    // Check if hardware multi threading is supported if core or cache topology reporting is not or failed
    if (mSystemInformation.Packages[Index].Features.HardwareMultiThread && (SubIndex == 0)) {
      // Get the logical CPU count the old way
      mSystemInformation.Packages[Index].LogicalCount = (UINT32)(UINTN)CPUBitmask(mCPUIDRegisters[1], 16, 23);
    }
    // Check physical CPU count
    if (mSystemInformation.Packages[Index].CoreCount == 0) {
      mSystemInformation.Packages[Index].CoreCount = 1;
    }
    // Check logical CPU count
    if (mSystemInformation.Packages[Index].LogicalCount == 0) {
      mSystemInformation.Packages[Index].LogicalCount = 1;
    }
    if (mSystemInformation.Packages[Index].LogicalCount < mSystemInformation.Packages[Index].CoreCount) {
      mSystemInformation.Packages[Index].LogicalCount = mSystemInformation.Packages[Index].CoreCount;
    }
    // Add these counts to the total system counts
    mSystemInformation.LogicalCount += mSystemInformation.Packages[Index].LogicalCount;
    mSystemInformation.CoreCount += mSystemInformation.Packages[Index].CoreCount;
    // Determine extended features
    if (mCPUMaxIndex >= 0x7) {
      CPUID(0x7);
      DetermineCPUIntelExtendedFeatures(Index);
    }
    // Determine other features
    if (mCPUMaxExtIndex >= 0x80000001) {
      CPUID(0x80000001);
      DetermineCPUIntelOtherFeatures(Index);
    }
    // Get CPU description
    if (mCPUMaxExtIndex >= 0x80000004) {
      UINT32  ExtIndex = 0x80000002;
      CHAR8   Description[CPU_DESCRIPTION_SIZE + 1];
      CHAR8  *Str = Description;
      UINT32 *Ptr = (UINT32 *)Description;
      // Zero the description string memory
      ZeroMem((VOID *)Description, CPU_DESCRIPTION_SIZE + 1);
      // Perform CPUID to get description
      while (ExtIndex <= 0x80000004) {
        AsmCpuid(ExtIndex++, Ptr, Ptr + 1, Ptr + 2, Ptr + 3);
        Ptr += 4;
      }
      // Remove any leading spaces
      Str = Description;
      while (*Str == ' ') {
        ++Str;
      }
      // Copy description to CPU information
      CopyMem((VOID *)mSystemInformation.Packages[Index].Description, (CONST VOID *)Str, (CPU_DESCRIPTION_SIZE - (Str - Description)) + 1);
    }
    // Read IA32_MISC_ENABLE model specific register
    if (mSystemInformation.Packages[Index].Features.ModelSpecificRegisters) {
      UINT64 MSR;
      MSR = AsmReadMsr64(0x1A0);
      // Determine if speedstep is enabled
      if (mSystemInformation.Packages[Index].Features.Dynamic) {
        mSystemInformation.Packages[Index].Features.Dynamic = (BOOLEAN)CPUBitmask(MSR, 16, 16);
      }
      // Determine if turbo is enabled
      if ((mCPUMaxIndex >= 0x6) && (CPUBitmask(MSR, 38, 38) == 0)) {
        CPUID(0x6);
        mSystemInformation.Packages[Index].Features.Turbo = (BOOLEAN)CPUBitmask(mCPUIDRegisters[0], 1, 1);
      }
      // Determine mobile from IA32_PLATFORM_ID model specific register
      if (CPUBitmask(AsmReadMsr32(0x17), 28, 28) != 0) {
        mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      }
      if (mSystemInformation.Packages[Index].Features.Mobile) {
        Model = SwitchCPUIntelFamilyToMobile(Model);
      }
    }
  }
  return (CPU_MODEL)Model;
}
// DetermineCPUIntelFrequency
/// Determine CPU frequency information
/// @param Index The physical package index
VOID
EFIAPI
DetermineCPUIntelFrequency (
  IN UINT32 Index
) {
  UINT64 MSR;
  // Determine CPU clock frequency from CPU family
  switch (mSystemInformation.Packages[Index].Family) {

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

    case CPU_FAMILY_PENTIUM_4:
    case CPU_FAMILY_PENTIUM_D:
    case CPU_FAMILY_XEON:
    case CPU_FAMILY_XEON_MP:
      // Determine Pentium 4 clock frequency
      switch (CPUBitmask(AsmReadMsr32(0x2C), 0, 2)) {
        case 1:
          // 001B 133 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
          break;

        case 2:
          // 010B 200 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 200000000;
          break;

        case 3:
          // 011B 166 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 166666667;
          break;

        case 4:
          // 100B 333 MHz (Model 6)
          mSystemInformation.Packages[Index].Frequency.Clock = 333333333;
          break;

        default:
          
          switch (CPUBitmask(mSystemInformation.Packages[Index].Model, 4, 7)) {
            case 3:
            case 4:
              // 000B 266 MHz (Model 3 or 4)
              mSystemInformation.Packages[Index].Frequency.Clock = 266666667;
              break;

            case 2:
            default:
              // 000B 100 MHz (Model 2)
              mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
              break;
          }
          break;
      }
      break;

    case CPU_FAMILY_CELERON_D:
    case CPU_FAMILY_CORE:
    case CPU_FAMILY_CORE2:
    case CPU_FAMILY_CORE2_MOBILE:
    case CPU_FAMILY_XEON_CORE2:
      // Determine Core/Core2 clock frequency
      switch (CPUBitmask(AsmReadMsr32(0xCD), 0, 2)) {
        case 0:
          // 000B: 267 MHz (FSB 1067)
          mSystemInformation.Packages[Index].Frequency.Clock = 266666667;
          break;

        case 1:
          // 001B: 133 MHz (FSB 533)
          mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
          break;

        case 2:
          // 010B: 200 MHz (FSB 800)
          mSystemInformation.Packages[Index].Frequency.Clock = 200000000;
          break;

        case 3:
          // 011B: 167 MHz (FSB 667)
          mSystemInformation.Packages[Index].Frequency.Clock = 166666667;
          break;

        case 4:
          // 100B: 333 MHz (FSB 1333)
          mSystemInformation.Packages[Index].Frequency.Clock = 333333333;
          break;

        case 6:
          // 110B: 400 MHz (FSB 1600)
          mSystemInformation.Packages[Index].Frequency.Clock = 400000000;
          break;

        case 5:
        default:
          // 101B: 100 MHz (FSB 400)
          mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
          break;
      }
      break;

    case CPU_FAMILY_ATOM:
      // Atom clock frequency
      switch (CPUBitmask(AsmReadMsr32(0xCD), 0, 2)) {
        case 1:
          // 001B: 133 MHz (FSB 533)
          mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
          break;

        case 3:
          // 011B: 167 MHz (FSB 667)
          mSystemInformation.Packages[Index].Frequency.Clock = 166666667;
          break;

        case 7:
          // 111B: 083 MHz (FSB 333)
          mSystemInformation.Packages[Index].Frequency.Clock = 83333333;
          break;

        case 5:
        default:
          // 101B: 100 MHz (FSB 400)
          mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
          break;
      }
      break;

    case CPU_FAMILY_2ND_ATOM:
      // 2nd Gen Atom clock frequency
      switch (CPUBitmask(AsmReadMsr32(0xCD), 0, 2)) {
        case 0:
          // 000B: 083.3 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 83333333;
          break;

        case 2:
          // 010B: 133.3 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
          break;

        case 3:
          // 011B: 116.7 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 116666667;
          break;

        case 4:
          // 100B: 080.0 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 80000000;
          break;

        case 1:
        default:
          // 001B: 100.0 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
          break;
      }
      break;

    case CPU_FAMILY_3RD_ATOM:
      // 3rd Gen Atom clock frequency
      switch (CPUBitmask(AsmReadMsr32(0xCD), 0, 3)) {
        case 0:
          // 0000B: 083.3 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 83333333;
          break;

        case 2:
          // 0010B: 133.3 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
          break;

        case 3:
          // 0011B: 116.7 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 116666667;
          break;

        case 4:
          // 0100B: 080.0 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 80000000;
          break;

        case 5:
          // 0101B: 093.3 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 93333333;
          break;

        case 6:
          // 0110B: 090.0 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 90000000;
          break;

        case 7:
          // 0111B: 088.9 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 88888889;
          break;

        case 8:
          // 1000B: 087.5 MHz
          mSystemInformation.Packages[Index].Frequency.Clock = 87500000;
          break;

        case 1:
        default:
          // 0000B: 100.0 Mhz
          mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
          break;
      }
      break;

    case CPU_FAMILY_4TH_ATOM:
      // 4th Gen Atom clock frequency
      switch (CPUBitmask(AsmReadMsr32(0xCD), 0, 3)) {
        default:
          mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
          break;
      }
      break;

    case CPU_FAMILY_CORE_i:
    case CPU_FAMILY_CORE_i_MOBILE:
    case CPU_FAMILY_XEON_CORE_i:
      // Core i clock frequency - 133MHz
      mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
      break;

    case CPU_FAMILY_PENTIUM_M:
    case CPU_FAMILY_2ND_CORE_i:
    case CPU_FAMILY_3RD_CORE_i:
    case CPU_FAMILY_4TH_CORE_i:
    case CPU_FAMILY_5TH_CORE_i:
    case CPU_FAMILY_6TH_CORE_i:
    case CPU_FAMILY_7TH_CORE_i:
    case CPU_FAMILY_2ND_CORE_i_MOBILE:
    case CPU_FAMILY_3RD_CORE_i_MOBILE:
    case CPU_FAMILY_4TH_CORE_i_MOBILE:
    case CPU_FAMILY_5TH_CORE_i_MOBILE:
    case CPU_FAMILY_6TH_CORE_i_MOBILE:
    case CPU_FAMILY_7TH_CORE_i_MOBILE:
    case CPU_FAMILY_XEON_2ND_CORE_i:
    case CPU_FAMILY_XEON_3RD_CORE_i:
    case CPU_FAMILY_XEON_4TH_CORE_i:
    case CPU_FAMILY_XEON_5TH_CORE_i:
    case CPU_FAMILY_XEON_6TH_CORE_i:
    case CPU_FAMILY_XEON_7TH_CORE_i:
      // >= 2nd Gen Core i families clock frequency - 100MHz
      mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
      break;

    case CPU_FAMILY_XEON_PHI:
      // Xeon Phi clock frequency
      mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
      break;

#endif

#if defined(MDE_CPU_IA32)

    case CPU_FAMILY_ITANIUM:
      // Itanium clock frequency - 133MHz
      mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
      break;

    case CPU_FAMILY_ITANIUM2:
      // Itanium 2 clock frequency
      // Default clock is 400MHz but no idea how to determine frequency in IA32 mode
      mSystemInformation.Packages[Index].Frequency.Clock = 400000000;
      break;

#endif

#if defined(MDE_CPU_IA32)

    case CPU_FAMILY_PENTIUM_PRO:
    case CPU_FAMILY_PENTIUM_II:
    case CPU_FAMILY_MOBILE_PENTIUM_II:
    case CPU_FAMILY_PENTIUM_II_OVERDRIVE:
    case CPU_FAMILY_PENTIUM_III:
    case CPU_FAMILY_XEON_PENTIUM_III:
    case CPU_FAMILY_CELERON:
      // P6 families
      switch (CPUBitmask(AsmReadMsr32(0x2A), 18, 19)) {
        case 1:
          mSystemInformation.Packages[Index].Frequency.Clock = 133333333;
          break;

        case 2:
          mSystemInformation.Packages[Index].Frequency.Clock = 100000000;
          break;

        case 0:
        default:
          mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
          break;
      }
      break;

    case CPU_FAMILY_486:
    case CPU_FAMILY_PENTIUM:
    case CPU_FAMILY_MOBILE_PENTIUM:
    case CPU_FAMILY_PENTIUM_OVERDRIVE:
    case CPU_FAMILY_PENTIUM_MMX:
    case CPU_FAMILY_MOBILE_PENTIUM_MMX:
    case CPU_FAMILY_PENTIUM_MMX_OVERDRIVE:

#endif

    default:
      // Determine clock frequency by TSC if possible
      mSystemInformation.Packages[Index].Frequency.Clock = DetectCPUFrequency();
      break;

  }
  // Determine CPU minimum, maximum, and turbo multipliers
  switch (mSystemInformation.Packages[Index].Family) {

#if defined(MDE_CPU_IA32)

    case CPU_FAMILY_PENTIUM_MMX:
      mSystemInformation.Packages[Index].Frequency.Step = 1;
      mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = (UINT8)(UINTN)DivU64x32(LShiftU64((UINT64)mSystemInformation.Packages[Index].Frequency.Clock, 1), 66666667);
      mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
      mSystemInformation.Packages[Index].Frequency.TurboCount = 0;
      break;

    case CPU_FAMILY_PENTIUM:
    case CPU_FAMILY_MOBILE_PENTIUM:
    case CPU_FAMILY_PENTIUM_OVERDRIVE:
    case CPU_FAMILY_MOBILE_PENTIUM_MMX:
    case CPU_FAMILY_PENTIUM_MMX_OVERDRIVE:
      mSystemInformation.Packages[Index].Frequency.Step = 1;
      mSystemInformation.Packages[Index].Frequency.TurboCount = 0;
      if ((mSystemInformation.Packages[Index].Frequency.Clock >= 70000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 80000000)) {
        // 75MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 50000000;
        mSystemInformation.Packages[Index].Frequency.Maximum = 3;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 80000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 85000000)) {
        // 83MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 3;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 85000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 95000000)) {
        // 90MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 60000000;
        mSystemInformation.Packages[Index].Frequency.Maximum = 3;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 95000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 105000000)) {
        // 100MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 3;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 115000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 125000000)) {
        // 120MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 60000000;
        mSystemInformation.Packages[Index].Frequency.Maximum = 4;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 128000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 138000000)) {
        // 133MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 4;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 145000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 155000000)) {
        // 150MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 60000000;
        mSystemInformation.Packages[Index].Frequency.Maximum = 3;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 161000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 171000000)) {
        // 166MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 5;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 175000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 185000000)) {
        // 180MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 60000000;
        mSystemInformation.Packages[Index].Frequency.Maximum = 6;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 195000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 205000000)) {
        // 200MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 6;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 228000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 238000000)) {
        // 233MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 7;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 261000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 271000000)) {
        // 266MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 8;
      } else if ((mSystemInformation.Packages[Index].Frequency.Clock > 295000000) && (mSystemInformation.Packages[Index].Frequency.Clock <= 305000000)) {
        // 300MHz
        mSystemInformation.Packages[Index].Frequency.Clock = 66666667;
        mSystemInformation.Packages[Index].Frequency.Maximum = 9;
      } else {
        // Set default multiplers
        mSystemInformation.Packages[Index].Frequency.Maximum = 2;
      }
      mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum;
      break;

#endif

    case CPU_FAMILY_PENTIUM_4:
    case CPU_FAMILY_PENTIUM_D:
    case CPU_FAMILY_PENTIUM_M:
    case CPU_FAMILY_CELERON_D:
    case CPU_FAMILY_CORE:
    case CPU_FAMILY_CORE2:
    case CPU_FAMILY_CORE2_MOBILE:
    case CPU_FAMILY_XEON:
    case CPU_FAMILY_XEON_MP:
    case CPU_FAMILY_XEON_CORE2:
    case CPU_FAMILY_ATOM:
    case CPU_FAMILY_XEON_PHI:
      // Determine Pentium M/Core/Core2/Atom multipliers
      if (mSystemInformation.Packages[Index].Features.ModelSpecificRegisters) {
        // Read the MSR_PERF_STATUS model specific register
        MSR = AsmReadMsr64(0x198);
        mSystemInformation.Packages[Index].Frequency.Step = (UINT8)CPUBitmask(MSR, 46, 46);
        mSystemInformation.Packages[Index].Frequency.Minimum = (6 << mSystemInformation.Packages[Index].Frequency.Step);
        // Check if XE operation is enabled
        if (CPUBitmask(MSR, 31, 31) != 0) {
          mSystemInformation.Packages[Index].Frequency.Maximum = (UINT8)CPUBitmask(MSR, 40, 44);
        } else {
          // Get maximum from MSR_PLATFORM_ID model specific register
          mSystemInformation.Packages[Index].Frequency.Maximum = (UINT8)CPUBitmask(AsmReadMsr64(0x17), 8, 13);
        }
        // Set turbo multiplier
        if (mSystemInformation.Packages[Index].Features.Turbo) {
          mSystemInformation.Packages[Index].Frequency.TurboCount = 1;
          mSystemInformation.Packages[Index].Frequency.Turbo[0] = mSystemInformation.Packages[Index].Frequency.Maximum + (1 << mSystemInformation.Packages[Index].Frequency.Step);
        } else {
          mSystemInformation.Packages[Index].Frequency.TurboCount = 0;
        }
        // Check if multipliers exist
        if ((mSystemInformation.Packages[Index].Frequency.Minimum != 0) &&
            (mSystemInformation.Packages[Index].Frequency.Maximum != 0)) {
          break;
        }
      } else

    case CPU_FAMILY_CORE_i:
    case CPU_FAMILY_2ND_CORE_i:
    case CPU_FAMILY_3RD_CORE_i:
    case CPU_FAMILY_4TH_CORE_i:
    case CPU_FAMILY_5TH_CORE_i:
    case CPU_FAMILY_6TH_CORE_i:
    case CPU_FAMILY_7TH_CORE_i:
    case CPU_FAMILY_CORE_i_MOBILE:
    case CPU_FAMILY_2ND_CORE_i_MOBILE:
    case CPU_FAMILY_3RD_CORE_i_MOBILE:
    case CPU_FAMILY_4TH_CORE_i_MOBILE:
    case CPU_FAMILY_5TH_CORE_i_MOBILE:
    case CPU_FAMILY_6TH_CORE_i_MOBILE:
    case CPU_FAMILY_7TH_CORE_i_MOBILE:
    case CPU_FAMILY_XEON_CORE_i:
    case CPU_FAMILY_XEON_2ND_CORE_i:
    case CPU_FAMILY_XEON_3RD_CORE_i:
    case CPU_FAMILY_XEON_4TH_CORE_i:
    case CPU_FAMILY_XEON_5TH_CORE_i:
    case CPU_FAMILY_XEON_6TH_CORE_i:
    case CPU_FAMILY_XEON_7TH_CORE_i:
    case CPU_FAMILY_2ND_ATOM:
    case CPU_FAMILY_3RD_ATOM:
    case CPU_FAMILY_4TH_ATOM:
      // Core i families
      if (mSystemInformation.Packages[Index].Features.ModelSpecificRegisters) {
        // Determine CPU minimum and maximum multipliers from MSR_PLATFORM_INFO model specific register
        MSR = AsmReadMsr64(0xCE);
        mSystemInformation.Packages[Index].Frequency.Maximum = (UINT8)CPUBitmask(MSR, 8, 15);
        mSystemInformation.Packages[Index].Frequency.Minimum = (UINT8)CPUBitmask(MSR, 40, 47);
        mSystemInformation.Packages[Index].Frequency.Step = 0;
        // Determine CPU turbo multipliers
        mSystemInformation.Packages[Index].Frequency.TurboCount = 0;
        if (mSystemInformation.Packages[Index].Features.Turbo) {
          // Read MSR_TURBO_RATIO_LIMIT model specific register
          MSR = AsmReadMsr64(0x1AD);
          while (mSystemInformation.Packages[Index].Frequency.TurboCount < CPU_TURBO_COUNT) {
            if (MSR == 0ULL) {
              break;
            }
            // Get each turbo group ratio
            mSystemInformation.Packages[Index].Frequency.Turbo[(mSystemInformation.Packages[Index].Frequency.TurboCount)++] = (UINT8)(MSR & 0xFF);
            MSR = RShiftU64(MSR, 8);
          }
        }
        // Check if multipliers exist
        if ((mSystemInformation.Packages[Index].Frequency.Minimum != 0) &&
            (mSystemInformation.Packages[Index].Frequency.Maximum != 0)) {
          break;
        }
      } else

#if defined(MDE_CPU_IA32)

    case CPU_FAMILY_PENTIUM_PRO:
    case CPU_FAMILY_PENTIUM_II:
    case CPU_FAMILY_MOBILE_PENTIUM_II:
    case CPU_FAMILY_PENTIUM_II_OVERDRIVE:
    case CPU_FAMILY_PENTIUM_III:
    case CPU_FAMILY_XEON_PENTIUM_III:
    case CPU_FAMILY_CELERON:
      // P6 families
      if (mSystemInformation.Packages[Index].Features.ModelSpecificRegisters) {
        mSystemInformation.Packages[Index].Frequency.TurboCount = 0;
        switch (CPUBitmask(AsmReadMsr32(0x2A), 22, 25)) {
          case 1:
            mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = 3;
            mSystemInformation.Packages[Index].Frequency.Step = 0;
            break;

          case 2:
            mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = 4;
            mSystemInformation.Packages[Index].Frequency.Step = 0;
            break;

          case 5:
            mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = 7;
            mSystemInformation.Packages[Index].Frequency.Step = 1;
            break;

          case 6:
            mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = 9;
            mSystemInformation.Packages[Index].Frequency.Step = 1;
            break;

          case 7:
            mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = 5;
            mSystemInformation.Packages[Index].Frequency.Step = 1;
            break;

          case 0:
          case 3:
          case 4:
          default:
            mSystemInformation.Packages[Index].Frequency.Minimum = mSystemInformation.Packages[Index].Frequency.Maximum = 2;
            mSystemInformation.Packages[Index].Frequency.Step = 0;
            break;
        }
        break;
      }

    case CPU_FAMILY_486:

#endif

    default:
      // Set default multiplers
      mSystemInformation.Packages[Index].Frequency.Maximum = 1;
      mSystemInformation.Packages[Index].Frequency.Minimum = 1;
      mSystemInformation.Packages[Index].Frequency.Step = 0;
      mSystemInformation.Packages[Index].Frequency.TurboCount = 0;
      break;
  }
}
// DetermineCPUIntelFamily
/// Determine CPU family information
/// @param Index The physical package index
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUIntelFamily (
  IN UINT32 Index
) {
  UINTN ExtendedModel;
  UINTN Family;

  // Determine Intel CPU family from model signature
  switch (mSystemInformation.Packages[Index].Model) {

# if defined(MDE_CPU_IA32)

    case CPU_MODEL_PENTIUM_PRO:
    case CPU_MODEL_PENTIUM_PRO_B0:
    case CPU_MODEL_PENTIUM_PRO_C0:
    case CPU_MODEL_PENTIUM_PRO_sA0:
    case CPU_MODEL_PENTIUM_PRO_sA1:
    case CPU_MODEL_PENTIUM_PRO_sB0:
      // P6/Pentium Pro family
      return CPU_FAMILY_PENTIUM_PRO;

    case CPU_MODEL_PENTIUM_II:
    case CPU_MODEL_PENTIUM_II_C1:
    case CPU_MODEL_PENTIUM_II_dA0:
    case CPU_MODEL_PENTIUM_II_dA1:
    case CPU_MODEL_PENTIUM_II_dB0:
    case CPU_MODEL_PENTIUM_II_dB1:
      // Pentium II family
      return CPU_FAMILY_PENTIUM_II;

    case CPU_MODEL_MOBILE_PENTIUM_II:
      // Mobile Pentium II family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_MOBILE_PENTIUM_II;

    case CPU_MODEL_PENTIUM_II_OVERDRIVE:
      // Pentium II Overdrive family
      return CPU_FAMILY_PENTIUM_II_OVERDRIVE;

    case CPU_MODEL_PENTIUM_III:
    case CPU_MODEL_PENTIUM_III_kC0:
    case CPU_MODEL_PENTIUM_III_cA0:
    case CPU_MODEL_PENTIUM_III_cB0:
    case CPU_MODEL_PENTIUM_III_cC0:
    case CPU_MODEL_PENTIUM_III_cD0:
    case CPU_MODEL_PENTIUM_III_tA0:
    case CPU_MODEL_PENTIUM_III_tA1:
    case CPU_MODEL_PENTIUM_III_tB1:
      // Pentium III family
      return CPU_FAMILY_PENTIUM_III;

    case CPU_MODEL_XEON_PENTIUM_III:
      // Pentium III Xeon family
      return CPU_FAMILY_XEON_PENTIUM_III;

# endif

# if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

    case CPU_MODEL_PENTIUM_4:
    case CPU_MODEL_PENTIUM_4_B0:
    case CPU_MODEL_PENTIUM_4_B2:
    case CPU_MODEL_PENTIUM_4_C0:
    case CPU_MODEL_PENTIUM_4_C1:
    case CPU_MODEL_PENTIUM_4_C1_2:
    case CPU_MODEL_PENTIUM_4_C1_3:
    case CPU_MODEL_PENTIUM_4_D0:
    case CPU_MODEL_PENTIUM_4_D0_2:
    case CPU_MODEL_PENTIUM_4_D1:
    case CPU_MODEL_PENTIUM_4_E0:
    case CPU_MODEL_PENTIUM_4_G1:
    case CPU_MODEL_PENTIUM_4_M0:
    case CPU_MODEL_PENTIUM_4_N0:
    case CPU_MODEL_PENTIUM_4_R0:
      // Pentium 4 family
      return CPU_FAMILY_PENTIUM_4;

    case CPU_MODEL_PENTIUM_D:
    case CPU_MODEL_PENTIUM_D_B0:
    case CPU_MODEL_PENTIUM_D_B1:
    case CPU_MODEL_PENTIUM_D_C1:
    case CPU_MODEL_PENTIUM_D_D0:
      // Pentium D family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_PENTIUM_D;

    case CPU_MODEL_PENTIUM_M:
    case CPU_MODEL_PENTIUM_M_B1:
    case CPU_MODEL_PENTIUM_M_C0:
    case CPU_MODEL_INTEL_EMBEDDED:
    case CPU_MODEL_INTEL_EMBEDDED_B1:
      // Pentium M family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_PENTIUM_M;

    case CPU_MODEL_CELERON:
    case CPU_MODEL_CELERON_A1:
    case CPU_MODEL_CELERON_B0:
      // Celeron family
      return CPU_FAMILY_CELERON;

    case CPU_MODEL_CELERON_D:
      // Celeron D family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_CELERON_D;

    case CPU_MODEL_CORE:
    case CPU_MODEL_CORE_D0:
      // Core family
      return CPU_FAMILY_CORE;

    case CPU_MODEL_CORE2:
    case CPU_MODEL_CORE2_B1:
    case CPU_MODEL_CORE2_B2:
    case CPU_MODEL_CORE2_B3:
    case CPU_MODEL_CORE2_G0:
    case CPU_MODEL_CORE2_M0:
    case CPU_MODEL_CORE2_M1:
    case CPU_MODEL_CORE2_R0:
      // Core2 family
      return CPU_FAMILY_CORE2;

    case CPU_MODEL_CORE2_MOBILE:
    case CPU_MODEL_CORE2_MOBILE_E0:
    case CPU_MODEL_CORE2_MOBILE_E1:
    case CPU_MODEL_CORE2_MOBILE_M1:
      // Core2 Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_CORE2_MOBILE;

    case CPU_MODEL_CORE_i:
    case CPU_MODEL_CORE_i_C0:
    case CPU_MODEL_CORE_i_D0:
      // Core i family
      return CPU_FAMILY_CORE_i;

    case CPU_MODEL_2ND_CORE_i:
    case CPU_MODEL_2ND_CORE_i_D2:
      // 2nd Gen Core i family
      return CPU_FAMILY_2ND_CORE_i;
        
    case CPU_MODEL_3RD_CORE_i:
    case CPU_MODEL_3RD_CORE_i_E2:
      // 3rd Gen Core i family
      return CPU_FAMILY_3RD_CORE_i;

    case CPU_MODEL_4TH_CORE_i:
      // 4th Gen Core i family
      return CPU_FAMILY_4TH_CORE_i;

    case CPU_MODEL_5TH_CORE_i:
      // 5th Gen Core i family
      return CPU_FAMILY_5TH_CORE_i;

    case CPU_MODEL_6TH_CORE_i:
      // 6th Gen Core i family
      return CPU_FAMILY_6TH_CORE_i;

    case CPU_MODEL_CORE_i_MOBILE:
      // Core i Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_CORE_i_MOBILE;

    case CPU_MODEL_2ND_CORE_i_MOBILE:
    case CPU_MODEL_2ND_CORE_i_MOBILE_A1:
    case CPU_MODEL_2ND_CORE_i_MOBILE_A2:
    case CPU_MODEL_2ND_CORE_i_MOBILE_K0:
    case CPU_MODEL_2ND_CORE_i_MOBILE_J1:
      // 2nd Gen Core i Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_2ND_CORE_i_MOBILE;

    case CPU_MODEL_3RD_CORE_i_MOBILE:
    case CPU_MODEL_3RD_CORE_i_MOBILE_D0:
    case CPU_MODEL_3RD_CORE_i_MOBILE_E1:
      // 3rd Gen Core i Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_3RD_CORE_i_MOBILE;

    case CPU_MODEL_4TH_CORE_i_MOBILE:
      // 4th Gen Core i Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_4TH_CORE_i_MOBILE;

    case CPU_MODEL_5TH_CORE_i_MOBILE:
      // 5th Gen Core i Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_5TH_CORE_i_MOBILE;

    case CPU_MODEL_6TH_CORE_i_MOBILE:
      // 6th Gen Core i Mobile family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_6TH_CORE_i_MOBILE;

    case CPU_MODEL_XEON:
    case CPU_MODEL_XEON_B0:
    case CPU_MODEL_XEON_B1:
      // Xeon family
      return CPU_FAMILY_XEON;

    case CPU_MODEL_XEON_MP:
    case CPU_MODEL_XEON_MP_A0:
    case CPU_MODEL_XEON_MP_C0:
      // Xeon MP family
      return CPU_FAMILY_XEON_MP;

    case CPU_MODEL_XEON_CORE2:
      // Xeon Core2 family
      return CPU_FAMILY_XEON_CORE2;

    case CPU_MODEL_XEON_CORE_i:
    case CPU_MODEL_XEON_CORE_i_D0:
      // Xeon Core i family
      return CPU_FAMILY_XEON_CORE_i;

    case CPU_MODEL_XEON_2ND_CORE_i:
    case CPU_MODEL_XEON_2ND_CORE_i_B0:
    case CPU_MODEL_XEON_2ND_CORE_i_B1:
    case CPU_MODEL_XEON_2ND_CORE_i_C0:
    case CPU_MODEL_XEON_2ND_CORE_i_C1:
    case CPU_MODEL_XEON_2ND_CORE_i_C2:
      // Xeon 2nd Gen Core i family
      return CPU_FAMILY_XEON_2ND_CORE_i;

    case CPU_MODEL_XEON_3RD_CORE_i:
    case CPU_MODEL_XEON_3RD_CORE_i_R1:
    case CPU_MODEL_XEON_3RD_CORE_i_R2:
      // Xeon 3rd Gen Core i family
      return CPU_FAMILY_XEON_3RD_CORE_i;

    case CPU_MODEL_XEON_4TH_CORE_i:
    case CPU_MODEL_XEON_4TH_CORE_i_M0:
      // Xeon 4th Gen Core i family
      return CPU_FAMILY_XEON_4TH_CORE_i;

    case CPU_MODEL_ATOM:
    case CPU_MODEL_ATOM_A0:
    case CPU_MODEL_ATOM_B1:
    case CPU_MODEL_ATOM_B2:
      // Atom family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_ATOM;

    case CPU_MODEL_2ND_ATOM:
    case CPU_MODEL_2ND_ATOM_C0:
      // 2nd Gen Atom family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_2ND_ATOM;

    case CPU_MODEL_3RD_ATOM:
    case CPU_MODEL_3RD_ATOM_C1:
      // 3rd Gen Atom family
      mSystemInformation.Packages[Index].Features.Mobile = TRUE;
      return CPU_FAMILY_3RD_ATOM;

# endif

    default:
      // Model signature was not found, so attempt to return family from extended model
      Family = (UINT32)(UINTN)CPUBitmask(mSystemInformation.Packages[Index].Model, 8, 11);
      ExtendedModel = (UINT32)(UINTN)CPUBitmask(mSystemInformation.Packages[Index].Model, 4, 7);

# if defined(MDE_CPU_IA32)

      if (Family == 0x4) {
        return CPU_FAMILY_486;
      } else if (Family == 0x5) {
        switch (ExtendedModel) {

          case 0x0:
          case 0x1:
          case 0x2:
            if (CPUBitmask(mSystemInformation.Packages[Index].Model, 12, 12) != 0) {
              // Pentium Overdrive family
              return CPU_FAMILY_PENTIUM_OVERDRIVE;
            }
            // Pentium family
            return CPU_FAMILY_PENTIUM;

          case 0x4:
            if (CPUBitmask(mSystemInformation.Packages[Index].Model, 12, 12) != 0) {
              // Pentium MMX Overdrive family
              return CPU_FAMILY_PENTIUM_MMX_OVERDRIVE;
            }
            // Pentium MMX family
            return CPU_FAMILY_PENTIUM_MMX;

          case 0x7:
            // Mobile Pentium family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_MOBILE_PENTIUM;

          case 0x8:
            // Mobile Pentium MMX family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_MOBILE_PENTIUM_MMX;

          default:
            // Unknown CPU family
            break;

        }
      } else

# endif

# if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

      if (Family == 0x6) {
        // P6/P2/P3/Core/Core2/Core i architecture families
        ExtendedModel |= ((mSystemInformation.Packages[Index].Model >> 12) & 0xF0);
        switch (ExtendedModel) {

#  if defined(MDE_CPU_IA32)

          case 0x0:
          case 0x1:
            // Pentium Pro family
            return CPU_FAMILY_PENTIUM_PRO;

          case 0x3:
          case 0x5:
            if (CPUBitmask(mSystemInformation.Packages[Index].Model, 12, 12) != 0) {
              // Pentium II Overdrive family
              return CPU_FAMILY_PENTIUM_II_OVERDRIVE;
            }
            // Pentium II family
            return CPU_FAMILY_PENTIUM_II;

          case 0x6:
            // Mobile Pentium II family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_MOBILE_PENTIUM_II;

          case 0x7:
          case 0x8:
          case 0xB:
            // Pentium III family
            return CPU_FAMILY_PENTIUM_III;

          case 0xA:
            // Pentium III Xeon family
            return CPU_FAMILY_XEON_PENTIUM_III;

#  endif

#  if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

          case 0x9:
          case 0xD:
          case 0x15:
            // Pentium M family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_PENTIUM_M;

#   if !defined(MDE_CPU_IA32)

          case 0x6:
            // Celeron family
            return CPU_FAMILY_CELERON;

#   endif

          case 0x16:
            // Celeron D family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_CELERON_D;

          case 0xE:
            // Core family
            return CPU_FAMILY_CORE;

          case 0xF:
          case 0x17:
            // Core2 family
            return CPU_FAMILY_CORE2;

          case 0x1A:
          case 0x1E:
            // Core i family
            return CPU_FAMILY_CORE_i;

          case 0x1F:
            // Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_CORE_i_MOBILE;

          case 0x25:
            // 2nd Gen Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_2ND_CORE_i_MOBILE;

          case 0x2A:
            // 2nd Gen Core i family
            return CPU_FAMILY_2ND_CORE_i;

          case 0x3A:
            // 3rd Gen Core i family
            return CPU_FAMILY_3RD_CORE_i;

          case 0x3C:
          case 0x3D:
            // 3rd Gen Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_3RD_CORE_i_MOBILE;

          case 0x3E:
            // 4th Gen Core i family
            return CPU_FAMILY_4TH_CORE_i;

          case 0x45:
            // 4th Gen Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_4TH_CORE_i_MOBILE;

          case 0x46:
            // 5th Gen Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_5TH_CORE_i_MOBILE;

          case 0x47:
            // 5th Gen Core i family
            return CPU_FAMILY_5TH_CORE_i;

          case 0x4E:
            // 6th Gen Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_6TH_CORE_i_MOBILE;

          case 0x5E:
            // 6th Gen Core i family
            return CPU_FAMILY_6TH_CORE_i;

          case 0x8E:
            // 7th Gen Core i Mobile family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_7TH_CORE_i_MOBILE;

          case 0x9E:
            // 7th Gen Core i family
            return CPU_FAMILY_7TH_CORE_i;

          case 0x1D:
            // Xeon Core2 family
            return CPU_FAMILY_XEON_CORE2;

          case 0x2E:
          case 0x2F:
            // Xeon Core i family
            return CPU_FAMILY_XEON_CORE_i;

          case 0x2C:
          case 0x2D:
            // Xeon 2nd Gen Core i family
            return CPU_FAMILY_XEON_2ND_CORE_i;

          case 0x3F:
            // Xeon 3rd Gen Core i family
            return CPU_FAMILY_XEON_3RD_CORE_i;

          case 0x4F:
            // Xeon 4th Gen Core i family
            return CPU_FAMILY_XEON_4TH_CORE_i;

          case 0x1C:
          case 0x26:
          case 0x27:
          case 0x35:
          case 0x36:
            // Atom family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_ATOM;

          case 0x37:
          case 0x4A:
          case 0x4D:
          case 0x5A:
          case 0x5D:
            // 2nd Gen Atom family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_2ND_ATOM;

          case 0x4C:
            // 3rd Gen Atom family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_3RD_ATOM;

          case 0x5C:
          case 0x5F:
            // 4th Gen Atom family
            mSystemInformation.Packages[Index].Features.Mobile = TRUE;
            return CPU_FAMILY_4TH_ATOM;

#  endif

          default:
            // Unknown CPU family
            break;

        }
      } else

# endif

# if defined(MDE_CPU_IA32)

      if (Family == 0x7) {
        // Itanium families
        return CPU_FAMILY_ITANIUM;
      }

# endif

# if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

      if ((Family == 0xF) || (Family == 0)) {
        // Check extended family
        Family += (UINT32)(UINTN)CPUBitmask(mSystemInformation.Packages[Index].Model, 20, 27);
        ExtendedModel |= ((mSystemInformation.Packages[Index].Model >> 12) & 0xF0);

#  if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

        if (Family == 0xF) {
          // Netburst architecture families
          switch (ExtendedModel) {

            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
              // Pentium 4 family
              return CPU_FAMILY_PENTIUM_4;

            case 6:
              // Pentium D family
              return CPU_FAMILY_PENTIUM_D;

            default:
              // Unknown CPU family
              break;

          }
        }

#  endif

# endif

# if defined(MDE_CPU_IA32)

        if ((Family == 0x10) ||
            (Family == 0x20) ||
            (Family == 0x21)) {
          // Itanium 2 families
          return CPU_FAMILY_ITANIUM2;
        }

# endif

      }
      break;
  }

  return CPU_FAMILY_UNKNOWN;
}

#endif

#if defined(CPU_MDE_IPF)

// TODO: Itanium CPU detection

#endif

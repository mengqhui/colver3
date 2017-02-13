//
/// @file Include/Library/PlatformLib.h
///
/// Platform information library
///

#pragma once
#ifndef __CPU_INFORMATION_LIBRARY_HEADER__
#define __CPU_INFORMATION_LIBRARY_HEADER__

#include <Library/LogLib.h>

#include <Protocol/PciIo.h>

#include "CPU/Intel.h"
#include "CPU/AMD.h"

// PACKAGE_COUNT
/// The maximum count of physical packages
#define PACKAGE_COUNT 4

// CPU_TURBO_COUNT
/// The maximum count of turbo multipliers
#define CPU_TURBO_COUNT 8
// CPU_DESCRIPTION_SIZE
/// The size of CPU description string in characters
#define CPU_DESCRIPTION_SIZE 48

// SLOT_COUNT
/// The maximum count of physical RAM slots
#define SLOT_COUNT 24

// FIND_DEVICE_VENDOR_ID
/// Find devices by vendor id
#define FIND_DEVICE_VENDOR_ID 0x1
// FIND_DEVICE_DEVICE_ID
/// Find devices by device id
#define FIND_DEVICE_DEVICE_ID 0x2
// FIND_DEVICE_BASE_CLASS
/// Find devices by base class
#define FIND_DEVICE_BASE_CLASS 0x4
// FIND_DEVICE_SUB_CLASS
/// Find devices by sub class
#define FIND_DEVICE_SUB_CLASS 0x8
// FIND_DEVICE_ALL
/// Find devices by vendor id, device id, base class and sub class
#define FIND_DEVICE_ALL (FIND_DEVICE_VENDOR_ID | FIND_DEVICE_DEVICE_ID | FIND_DEVICE_BASE_CLASS | FIND_DEVICE_SUB_CLASS)

// CPU_VENDOR
/// CPU vendor information
typedef enum _CPU_VENDOR CPU_VENDOR;
enum _CPU_VENDOR {

  // CPU_VENDOR_UNKNOWN
  /// Unknown CPU vendor
  CPU_VENDOR_UNKNOWN = 0,

  //
  // Physical CPU vendors
  //

  // CPU_VENDOR_INTEL
  /// Intel CPU vendor
  CPU_VENDOR_INTEL,
  // CPU_VENDOR_AMD
  /// AMD CPU vendor
  CPU_VENDOR_AMD,

  //
  // Virtual CPU vendors
  //

  // CPU_VENDOR_VIRTUAL_KVM
  /// KVM virtual machine
  CPU_VENDOR_VIRTUAL_KVM,
  // CPU_VENDOR_VIRTUAL_MSHVM
  /// Microsoft Hyper-V/Windows Virtual PC virtual machine
  CPU_VENDOR_VIRTUAL_MICROSOFT,
  // CPU_VENDOR_VIRTUAL_PARALLELS
  /// Parallels virtual machine
  CPU_VENDOR_VIRTUAL_PARALLELS,
  // CPU_VENDOR_VIRTUAL_VMWARE
  /// VMWare virtual machine
  CPU_VENDOR_VIRTUAL_VMWARE,
  // CPU_VENDOR_VIRTUAL_XENHVM
  /// Xen virtual machine
  CPU_VENDOR_VIRTUAL_XEN,

};

// CPU_FAMILY
/// CPU family information
typedef enum _CPU_FAMILY CPU_FAMILY;
enum _CPU_FAMILY {

  // CPU_FAMILY_UNKNOWN
  /// Unknown CPU family
  CPU_FAMILY_UNKNOWN = 0,

#if defined(MDE_CPU_IA32)

  // CPU_FAMILY_486
  /// 486 compatible CPU family
  CPU_FAMILY_486,

  // CPU_FAMILY_PENTIUM
  /// Intel Pentium compatible CPU family
  CPU_FAMILY_PENTIUM,
  // CPU_FAMILY_MOBILE_PENTIUM
  /// Intel Mobile Pentium compatible CPU family
  CPU_FAMILY_MOBILE_PENTIUM,
  // CPU_FAMILY_PENTIUM_OVERDRIVE
  /// Intel Pentium compatible CPU family
  CPU_FAMILY_PENTIUM_OVERDRIVE,

  // CPU_FAMILY_PENTIUM_MMX
  /// Intel Pentium MMX compatible CPU family
  CPU_FAMILY_PENTIUM_MMX,
  // CPU_FAMILY_MOBILE_PENTIUM_MMX
  /// Intel Mobile Pentium MMX compatible CPU family
  CPU_FAMILY_MOBILE_PENTIUM_MMX,
  // CPU_FAMILY_PENTIUM_MMX_OVERDRIVE
  /// Intel Pentium MMX compatible CPU family
  CPU_FAMILY_PENTIUM_MMX_OVERDRIVE,

  // CPU_FAMILY_PENTIUM_PRO
  /// Intel Pentium Pro compatible CPU family
  CPU_FAMILY_PENTIUM_PRO,

  // CPU_FAMILY_PENTIUM_II
  /// Intel Pentium II compatible CPU family
  CPU_FAMILY_PENTIUM_II,
  // CPU_FAMILY_MOBILE_PENTIUM_II
  /// Intel Mobile Pentium II compatible CPU family
  CPU_FAMILY_MOBILE_PENTIUM_II,
  // CPU_FAMILY_PENTIUM_II_OVERDRIVE
  /// Intel Pentium II Overdrive compatible CPU family
  CPU_FAMILY_PENTIUM_II_OVERDRIVE,

  // CPU_FAMILY_PENTIUM_III
  /// Intel Pentium III compatible CPU family
  CPU_FAMILY_PENTIUM_III,

  // CPU_FAMILY_XEON_PENTIUM_III
  /// Intel Pentium III Xeon compatible CPU family
  CPU_FAMILY_XEON_PENTIUM_III,

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

  // CPU_FAMILY_PENTIUM_4
  /// Intel Pentium 4 compatible CPU family
  CPU_FAMILY_PENTIUM_4,

  // CPU_FAMILY_PENTIUM_D
  /// Intel Pentium D compatible CPU family
  CPU_FAMILY_PENTIUM_D,

  // CPU_FAMILY_PENTIUM_M
  /// Intel Pentium M compatible CPU family
  CPU_FAMILY_PENTIUM_M,

  // CPU_FAMILY_CELERON
  /// Intel Celeron compatible CPU family
  CPU_FAMILY_CELERON,

  // CPU_FAMILY_CELERON_D
  /// Intel Celeron D compatible CPU family
  CPU_FAMILY_CELERON_D,

  // CPU_FAMILY_CORE
  /// Intel Core compatible CPU family
  CPU_FAMILY_CORE,

  // CPU_FAMILY_CORE2
  /// Intel Core2 compatible CPU family
  CPU_FAMILY_CORE2,
  // CPU_FAMILY_CORE2_MOBILE
  /// Intel Core2 Mobile compatible CPU family
  CPU_FAMILY_CORE2_MOBILE,

  // CPU_FAMILY_CORE_i
  /// Intel Core i compatible CPU family
  CPU_FAMILY_CORE_i,
  // CPU_FAMILY_2ND_CORE_i
  /// Intel 2nd Gen Core i compatible CPU family
  CPU_FAMILY_2ND_CORE_i,
  // CPU_FAMILY_3RD_CORE_i
  /// Intel 3rd Gen Core i compatible CPU family
  CPU_FAMILY_3RD_CORE_i,
  // CPU_FAMILY_4TH_CORE_i
  /// Intel 4th Gen Core i compatible CPU family
  CPU_FAMILY_4TH_CORE_i,
  // CPU_FAMILY_5TH_CORE_i
  /// Intel 5th Gen Core i compatible CPU family
  CPU_FAMILY_5TH_CORE_i,
  // CPU_FAMILY_6TH_CORE_i
  /// Intel 6th Gen Core i compatible CPU family
  CPU_FAMILY_6TH_CORE_i,
  // CPU_FAMILY_7TH_CORE_i
  /// Intel 7th Gen Core i compatible CPU family
  CPU_FAMILY_7TH_CORE_i,

  // CPU_FAMILY_CORE_i_MOBILE
  /// Intel Core i Mobile compatible CPU family
  CPU_FAMILY_CORE_i_MOBILE,
  // CPU_FAMILY_2ND_CORE_i_MOBILE
  /// Intel2nd Gen Core i Mobile compatible CPU family
  CPU_FAMILY_2ND_CORE_i_MOBILE,
  // CPU_FAMILY_3RD_CORE_i_MOBILE
  /// Intel 3rd Gen Core i Mobile compatible CPU family
  CPU_FAMILY_3RD_CORE_i_MOBILE,
  // CPU_FAMILY_4TH_CORE_i_MOBILE
  /// Intel 4th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_4TH_CORE_i_MOBILE,
  // CPU_FAMILY_5TH_CORE_i_MOBILE
  /// Intel 5th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_5TH_CORE_i_MOBILE,
  // CPU_FAMILY_6TH_CORE_i_MOBILE
  /// Intel 6th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_6TH_CORE_i_MOBILE,
  // CPU_FAMILY_7TH_CORE_i_MOBILE
  /// Intel 7th Gen Core i Mobile compatible CPU family
  CPU_FAMILY_7TH_CORE_i_MOBILE,

  // CPU_FAMILY_XEON
  /// Intel Xeon compatible CPU family
  CPU_FAMILY_XEON,
  // CPU_FAMILY_XEON_MP
  /// Intel Xeon MP compatible CPU family
  CPU_FAMILY_XEON_MP,
  // CPU_FAMILY_XEON_CORE2
  /// Intel Xeon Core2 compatible CPU family
  CPU_FAMILY_XEON_CORE2,
  // CPU_FAMILY_XEON_CORE_i
  /// Intel Xeon Core i compatible CPU family
  CPU_FAMILY_XEON_CORE_i,
  // CPU_FAMILY_XEON_2ND_CORE_i
  /// Intel Xeon 2nd Gen Core i compatible CPU family
  CPU_FAMILY_XEON_2ND_CORE_i,
  // CPU_FAMILY_XEON_3RD_CORE_i
  /// Intel Xeon 3rd Gen Core i compatible CPU family
  CPU_FAMILY_XEON_3RD_CORE_i,
  // CPU_FAMILY_XEON_4TH_CORE_i
  /// Intel Xeon 4th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_4TH_CORE_i,
  // CPU_FAMILY_XEON_5TH_CORE_i
  /// Intel Xeon 5th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_5TH_CORE_i,
  // CPU_FAMILY_XEON_6TH_CORE_i
  /// Intel Xeon 6th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_6TH_CORE_i,
  // CPU_FAMILY_XEON_7TH_CORE_i
  /// Intel Xeon 7th Gen Core i compatible CPU family
  CPU_FAMILY_XEON_7TH_CORE_i,

  // CPU_FAMILY_XEON_PHI
  /// Intel Xeon Phi compatible CPU family
  CPU_FAMILY_XEON_PHI,

  // CPU_FAMILY_ATOM
  /// Intel Atom compatible CPU family
  CPU_FAMILY_ATOM,
  // CPU_FAMILY_2ND_ATOM
  /// Intel 2nd Gen Atom compatible CPU family
  CPU_FAMILY_2ND_ATOM,
  // CPU_FAMILY_3RD_ATOM
  /// Intel 3rd Gen Atom compatible CPU family
  CPU_FAMILY_3RD_ATOM,
  // CPU_FAMILY_4TH_ATOM
  /// Intel 4th Gen Atom compatible CPU family
  CPU_FAMILY_4TH_ATOM,

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_IPF)

  // CPU_FAMILY_ITANIUM
  /// Intel Itanium compatible CPU family
  CPU_FAMILY_ITANIUM,
  // CPU_FAMILY_ITANIUM2
  /// Intel Itanium 2 compatible CPU family
  CPU_FAMILY_ITANIUM2,

#endif

};

// CPU_MODEL
/// CPU model information
typedef UINT32 CPU_MODEL;

// CPU_MODEL_UNKNOWN
/// Unknown CPU model
#define CPU_MODEL_UNKNOWN 0

// CPU_FREQUENCY
/// CPU frequency information
typedef struct _CPU_FREQUENCY CPU_FREQUENCY;
struct _CPU_FREQUENCY {

  // Clock
  /// Base clock frequency in Hz
  UINT64 Clock;
  // Minimum
  /// Minimum multiplier
  UINT8  Minimum;
  // Maximum
  /// Maximum multiplier
  UINT8  Maximum;
  // Step
  /// Multiplier step
  UINT8  Step;
  // TurboCount
  /// Turbo multiplier count
  UINT8  TurboCount;
  // Turbo
  /// Turbo multipliers
  UINT8  Turbo[CPU_TURBO_COUNT];

};

// CPU_FEATURES
/// CPU features information
typedef struct _CPU_FEATURES CPU_FEATURES;
struct _CPU_FEATURES {

  // FPUOnChip
  /// Floating Point Unit On-Chip, the CPU contains an FPU
  BOOLEAN FPUOnChip                  : 1;
  // VirtualModeEnhancements
  /// Virtual 8086 mode enhancements, including CR4.VME for controlling the feature, CR4.PVI for
  /// protected mode virtual interrupts, software interrupt indirection, expansion of the TSS
  /// with the software indirection bitmap, and EFLAGS.VIF and EFLAGS.VIP flags
  BOOLEAN VirtualModeEnhancements    : 1;
  // DebugExtensions
  /// Support for I/O breakpoints, including CR4.DE for controlling the feature, and optional trapping of accesses to DR4 and DR5
  BOOLEAN DebugExtensions            : 1;
  // PageSizeExtension
  /// Large pages of size 4 MByte are supported, including CR4.PSE for controlling the feature, the defined
  /// dirty bit in PDE (Page Directory Entries), optional reserved bit trapping in CR3, PDEs, and PTEs
  BOOLEAN PageSizeExtension          : 1;
  // TimeStampCounter
  /// The RDTSC instruction is supported, including CR4.TSD for controlling privilege
  BOOLEAN TimeStampCounter           : 1;
  // ModelSpecificRegisters
  /// The RDMSR and WRMSR instructions are supported
  BOOLEAN ModelSpecificRegisters     : 1;
  // PhysicalAddressExtension
  /// Physical addresses greater than 32 bits are supported: extended page table entry formats, an extra level
  /// in the page translation tables is defined, 2-MByte pages are supported instead of 4-Mbyte pages
  BOOLEAN PhysicalAddressExtension   : 1;
  // MachineCheckException
  /// Exception 18 is defined for Machine Checks, including CR4.MCE for controlling the feature
  BOOLEAN MachineCheckException      : 1;
  // CMPXCHG8BInstruction
  /// The compare-and-exchange 8 bytes (64 bits) instruction is supported (implicitly locked and atomic)
  BOOLEAN CMPXCHG8BInstruction       : 1;
  // APICOnChip
  /// The processor contains an Advanced Programmable Interrupt Controller (APIC), responding to
  /// memory mapped commands in the physical address range FFFE0000H to FFFE0FFFH (by default)
  BOOLEAN APICOnChip                 : 1;
  // SystemExtensions
  /// The SYSENTER and SYSEXIT and associated MSRs are supported
  BOOLEAN SystemExtensions           : 1;
  // MemoryTypeRangeRegisters
  /// MTRRs are supported, the MTRRcap MSR contains feature bits that describe what memory types
  /// are supported, how many variable MTRRs are supported, and whether fixed MTRRs are supported
  BOOLEAN MemoryTypeRangeRegisters   : 1;
  // PageGlobal
  /// The global bit is supported in paging-structure entries that map a page, indicating TLB entries that
  /// are common to different processes and need not be flushed. The CR4.PGE bit controls this feature
  BOOLEAN PageGlobal                 : 1;
  // MachineCheckArchitecture
  /// Machine Check Architecture of reporting machine errors is supported. The MCG_CAP MSR
  /// contains feature bits describing how many banks of error reporting MSRs are supported
  BOOLEAN MachineCheckArchitecture   : 1;
  // ConditionalMove
  /// The conditional move instruction CMOV is supported, if FPU is present then the FCOMI and FCMOV instructions are supported
  BOOLEAN ConditionalMove            : 1;
  // PageAttributeTable
  /// Page Attribute Table is supported, this feature augments the Memory Type Range Registers (MTRRs), allowing
  /// an operating system to specify attributes of memory accessed through a linear address on a 4KB granularity
  BOOLEAN PageAttributeTable         : 1;
  // PageAddressExtension
  /// 4-MByte pages addressing physical memory beyond 4 GBytes are supported with 32-bit paging. This feature
  /// indicates that upper bits of the physical address of a 4-MByte page are encoded in bits 20:13 of the
  /// page-directory entry. Such physical addresses are limited by MAXPHYADDR and may be up to 40 bits in size
  BOOLEAN PageAddressExtension       : 1;
  // ProcessorSerialNumber
  /// The processor supports the 96-bit processor identification number feature and the feature is enabled
  BOOLEAN ProcessorSerialNumber      : 1;
  // CLFLUSHInstruction
  /// CLFLUSH Instruction is supported
  BOOLEAN CLFLUSHInstruction         : 1;
  // DebugStore
  /// The processor supports the ability to write debug information into a memory resident buffer
  BOOLEAN DebugStore                 : 1;
  // ACPIThermalMonitor
  /// The processor implements internal MSRs that allow processor temperature to be monitored and
  /// processor performance to be modulated in predefined duty cycles under software control
  BOOLEAN ACPIThermalMonitor         : 1;
  // MMXInstructions
  /// The processor supports the Intel MMX technology
  BOOLEAN MMXInstructions            : 1;
  // FastInstructions
  /// The FXSAVE and FXRSTOR instructions are supported for fast save and restore of the
  /// floating point context. Presence of this bit also indicates that CR4.OSFXSR is available
  /// for an operating system to indicate that it supports the FXSAVE and FXRSTOR instructions
  BOOLEAN FastInstructions           : 1;
  // SSEInstructions
  /// The processor supports the SSE extensions
  BOOLEAN SSEInstructions            : 1;
  // SSE2Instructions
  /// The processor supports the SSE2 extensions
  BOOLEAN SSE2Instructions           : 1;
  // SelfSnoop
  /// The processor supports the management of conflicting memory types by performing
  /// a snoop of its own cache structure for transactions issued to the bus
  BOOLEAN SelfSnoop                  : 1;
  // HardwareMultiThread
  /// The processor supports hardware multithreading
  BOOLEAN HardwareMultiThread        : 1;
  // ThermalMonitor
  /// The processor implements the thermal monitor automatic thermal control circuitry (TCC)
  BOOLEAN ThermalMonitor             : 1;
  // PendingBreakEnable
  /// The processor supports the use of the FERR#/PBE# pin when the processor is in the stop-clock state (STPCLK# is asserted)
  /// to signal the processor that an interrupt is pending and that the processor should return to normal
  /// operation to handle the interrupt. Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability
  BOOLEAN PendingBreakEnable         : 1;
  // SSE3Instructions
  /// Streaming SIMD Extensions 3 (SSE3)
  BOOLEAN SSE3Instructions           : 1;
  // PCLMULQDQInstruction
  /// Indicates the processor supports the PCLMULQDQ instruction
  BOOLEAN PCLMULQDQInstruction       : 1;
  // DebugStoreArea64Bit
  /// Indicates the processor supports DS area using 64-bit layout
  BOOLEAN DebugStoreArea64Bit        : 1;
  // MonitorWait
  /// Indicates the processor supports MONITOR/MWAIT
  BOOLEAN MonitorWait                : 1;
  // DebugStoreCPL
  /// Indicates the processor supports the extensions to the Debug Store feature to allow for branch message storage qualified by CPL
  BOOLEAN DebugStoreCPL              : 1;
  // VMExtensions
  /// Indicates that the processor supports this virtual machine extensions
  BOOLEAN VMExtensions               : 1;
  // SaferModeExtensions
  /// Indicates that the processor supports Safer Mode
  BOOLEAN SaferModeExtensions        : 1;
  // Dynamic
  /// Whether CPU supports dynamic frequency
  BOOLEAN Dynamic                    : 1;
  // ThermalMonitor2
  /// Indicates whether the processor supports Thermal Monitor 2
  BOOLEAN ThermalMonitor2            : 1;
  // SSSE3Instructions
  /// Indicates the presence of the Supplemental Streaming SIMD Extensions 3 (SSSE3)
  BOOLEAN SSSE3Instructions          : 1;
  // L1ContextID
  /// Indicates the L1 data cache mode can be set to either adaptive mode or shared mode
  BOOLEAN L1ContextID                : 1;
  // DebugInterface
  /// The processor supports IA32_DEBUG_INTERFACE MSR for silicon debug
  BOOLEAN DebugInterface             : 1;
  // FMAExtensions
  /// Indicates the processor supports FMA extensions using YMM state
  BOOLEAN FMAExtensions              : 1;
  // CMPXCHG16BInstruction
  /// The compare-and-exchange 16 bytes (128 bits) instruction is supported (implicitly locked and atomic)
  BOOLEAN CMPXCHG16BInstruction      : 1;
  // xTPRUpdateControl
  /// Indicates that the processor supports changing IA32_MISC_ENABLE[bit 23]
  BOOLEAN xTPRUpdateControl          : 1;
  // PerfmonDebug
  /// Indicates the processor supports the performance and debug feature indication MSR IA32_PERF_CAPABILITIES
  BOOLEAN PerfmonDebug               : 1;
  // ProcessContextIdentifiers
  /// Indicates that the processor supports PCIDs and that software may set CR4.PCIDE to 1
  BOOLEAN ProcessContextIdentifiers  : 1;
  // Prefetch
  /// Indicates the processor supports the ability to prefetch data from a memory mapped device
  BOOLEAN Prefetch                   : 1;
  // SSE41Instructions
  /// Indicates that the processor supports SSE4.1
  BOOLEAN SSE41Instructions          : 1;
  // SSE42Instructions
  /// Indicates that the processor supports SSE4.2
  BOOLEAN SSE42Instructions          : 1;
  // x2APIC
  /// Indicates that the processor supports x2APIC feature
  BOOLEAN x2APIC                     : 1;
  // MOVBEInstruction
  /// Indicates that the processor supports MOVBE instruction
  BOOLEAN MOVEBEInstruction          : 1;
  // POPCNTInstruction
  /// Indicates that the processor supports the POPCNT instruction
  BOOLEAN POPCNTInstruction          : 1;
  // TimeStampCounterDeadline
  /// Indicates that the processor’s local APIC timer supports one-shot operation using a TSC deadline value
  BOOLEAN TimeStampCounterDeadline   : 1;
  // AESNIInstructions
  /// Indicates that the processor supports the AESNI instruction extensions
  BOOLEAN AESNIInstructions          : 1;
  // ExtFastInstructions
  /// Indicates that the processor supports the XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV instructions, and XCR0
  BOOLEAN ExtFastInstructions        : 1;
  /// Indicates that the OS has set CR4.OSXSAVE[bit 18] to enable XSETBV/XGETBV instructions to access XCR0
  /// and to support processor extended state management using XSAVE/XRSTOR
  BOOLEAN OSExtFastInstructions      : 1;
  // AVXInstructions
  /// Indicates the processor supports the AVX instruction extensions
  BOOLEAN AVXInstructions            : 1;
  // FP16BitConversion
  /// Indicates that processor supports 16-bit floating-point conversion instructions
  BOOLEAN FP16BitConversion          : 1;
  // RDRANDInstruction
  /// Indicates that processor supports RDRAND instruction
  BOOLEAN RDRANDInstruction          : 1;

  //
  // Extended features
  //

  // FSGSBASEInstructions
  /// Indicates that processor supports RDFSBASE/RDGSBASE/WRFSBASE/WRGSBASE instructions
  BOOLEAN FSGSBASEInstructions       : 1;
  // TimeStampCounterAdjust
  /// Indicates that processor supports IA32_TSC_ADJUST machine specific register
  BOOLEAN TimeStampCounterAdjust     : 1;
  // SoftwareGuardExtensions
  /// Indicates that processor supports supports Intel Software Guard Extensions
  BOOLEAN SoftwareGuardExtensions    : 1;
  // BitManip1Instructions
  /// Indicates that processor supports supports BMI1 instructions
  BOOLEAN BitManip1Instructions      : 1;
  // HardwareLockElision
  /// Indicates that processor supports supports HLE instructions
  BOOLEAN HardwareLockElision        : 1;
  // AVX2Instructions
  /// Indicates that processor supports supports AVX2 instructions
  BOOLEAN AVX2Instructions           : 1;
  // FPDPExceptionOnly
  /// Indicates floating point unit data pointer updated only on floating point exceptions
  BOOLEAN FPDPExceptionOnly          : 1;
  // SupervisorExecutePrevent
  /// Indicates that processor supports Supervisor-Mode Execution Prevention
  BOOLEAN SupervisorExecutePrevent   : 1;
  // BitManip2Instructions
  /// Indicates that processor supports supports BMI2 instructions
  BOOLEAN BitManip2Instructions      : 1;
  // ERMSBInstructions
  /// Indicates that processor supports Enhanced REP MOVSB/STOSB instructions
  BOOLEAN ERMSBInstructions          : 1;
  // INVPCIDInstruction
  /// Indicates that processor supports INVPCID instruction for system software that manages process-context identifiers
  BOOLEAN INVPCIDInstruction         : 1;
  // TransactSyncExtensions
  /// Indicates that processor supports supports RTM extensions
  BOOLEAN TransactSyncExtensions     : 1;
  // ResourceDirectorMonitor
  /// Indicates that processor supports Intel Resource Director Technology Monitoring capability
  BOOLEAN ResourceDirectorMonitor    : 1;
  // FPUDeprecated
  /// Indicates that processor deprecates FPU CS and FPU DS
  BOOLEAN FPUDeprecated              : 1;
  // MemoryProtectionExtensions
  /// Indicates that processor supports Intel Memory Protection Extensions
  BOOLEAN MemoryProtectionExtensions : 1;
  // ResourceDirectorAllocation
  /// Indicates that processor supports Intel Resource Director Technology Allocation
  BOOLEAN ResourceDirectorAllocation : 1;
  // RDSEEDInstruction
  /// Indicates that processor supports RDSEED instruction
  BOOLEAN RDSEEDInstruction          : 1;
  // ArithmeticExtensions
  /// Indicates that processor supports arbitrary precision arithmetic extensions
  BOOLEAN ArithmeticExtensions       : 1;
  // SupervisorAccessPrevent
  /// Indicates that processor supports Supervisor-Mode Access Prevention (and the CLAC/STAC instructions)
  BOOLEAN SupervisorAccessPrevent    : 1;
  // CLFLUSHOPT
  /// Indicates that processor supports CLFLUSHOPT instruction
  BOOLEAN CLFLUSHOPTInstruction      : 1;
  // CLWBInstruction
  /// Indicates that processor supports CLWB instruction
  BOOLEAN CLWBInstruction            : 1;
  // ProcessorTrace
  /// Indicates that processor supports Intel Processor Trace
  BOOLEAN ProcessorTrace             : 1;
  // SHAExtensions
  /// Indicates that processor supports Intel Secure Hash Algorithm Extensions
  BOOLEAN SHAExtensions              : 1;
  // PREFETCHWT1Instruction
  /// Indicates that processor supports PREFETCHWT1 instruction
  BOOLEAN PREFETCHWT1Instruction     : 1;
  // UserModeIntructionPrevent
  /// Indicates that processor supports user-mode instruction prevention
  BOOLEAN UserModeIntructionPrevent  : 1;
  // UserModeProtectKeys
  /// Indicates that processor supports protection keys for user-mode pages
  BOOLEAN UserModeProtectKeys        : 1;
  // OSProtectKeys
  /// Indicates that OS has set CR4.PKE to enable protection keys (and the RDPKRU/WRPKRU instructions)
  BOOLEAN OSProtectKeys              : 1; 
  // RDPIDInstruction
  /// Indicates that processor supports RDPID instruction
  BOOLEAN RDPIDInstruction           : 1;
  // SoftwareGuardLaunchConfig
  /// Indicates that processor supports Software Guard Launch Configuration
  BOOLEAN SoftwareGuardLaunchConfig  : 1;

  //
  // Other features
  //

  // AHFromFlags64Bit
  /// Indicates LAHF/SAHF available in 64-bit mode
  BOOLEAN AHFromFlags64Bit          : 1;
  // LZCNTInstruction
  /// Indicates the processor supports the LZCNT instruction
  BOOLEAN LZCNTInstruction          : 1;
  // PREFETCHWInstruction
  /// Indicates the processor supports the PREFETCHW instruction
  BOOLEAN PREFETCHWInstruction      : 1;
  // SystemExtensions64Bit
  /// Indicates SYSCALL/SYSRET available in 64-bit mode
  BOOLEAN SystemExtensions64Bit     : 1;
  // ExecuteDisableBit
  /// Indicates Execute Disable Bit available
  BOOLEAN ExecuteDisableBit         : 1;
  // GBPageExtension
  /// Indicates 1-GByte pages are available
  BOOLEAN GBPageExtension           : 1;
  // RDTSCPInstruction
  /// Indicates RDTSCP instruction and IA32_TSC_AUX machine specific register are available
  BOOLEAN RDTSCPInstruction         : 1;
  // X64
  /// Indicates Intel64 or AMD64 Architecture available
  BOOLEAN X64                       : 1;

  //
  // Additional features
  //

  // Mobile
  /// Mobile CPU
  BOOLEAN Mobile                    : 1;
  // Turbo
  /// Whether CPU supports turbo frequency
  BOOLEAN Turbo                     : 1;

};

// PACKAGE_INFORMATION
/// CPU information such as type, family, frequency, and features
typedef struct _PACKAGE_INFORMATION PACKAGE_INFORMATION;
struct _PACKAGE_INFORMATION {

  // Vendor
  /// CPU vendor information
  CPU_VENDOR    Vendor;
  // Family
  /// CPU family information
  CPU_FAMILY    Family;
  // Model
  /// CPU model information
  CPU_MODEL     Model;
  // LogicalCount
  /// CPU package logical threads count
  UINT32        LogicalCount;
  // CoreCount
  /// CPU package physical cores count
  UINT32        CoreCount;
  // Frequency
  /// CPU frequency information
  CPU_FREQUENCY Frequency;
  // Features
  /// CPU features information
  CPU_FEATURES  Features;
  // Description
  /// CPU description information
  CHAR8         Description[CPU_DESCRIPTION_SIZE + 1];

};

// SLOT_INFORMATION
/// RAM slot information
typedef struct _SLOT_INFORMATION SLOT_INFORMATION;
struct _SLOT_INFORMATION {

  // Populated
  /// Whether this slot is populated or not
  BOOLEAN Populated;
  // Type
  /// The type of RAM module in the slot
  UINT8   Type;
  // Size
  /// The size in bytes of the RAM module in the slot
  UINT64  Size;

};

// SYSTEM_INFORMATION
/// System information such as CPU and RAM
typedef struct _SYSTEM_INFORMATION SYSTEM_INFORMATION;
struct _SYSTEM_INFORMATION {

  // LogicalCount
  /// CPU total logical threads count
  UINT32              LogicalCount;
  // CoreCount
  /// CPU total physical cores count
  UINT32              CoreCount;
  // PackageCount
  /// CPU total physical package count
  UINT32              PackageCount;
  // Packages
  /// CPU physical packages
  PACKAGE_INFORMATION Packages[PACKAGE_COUNT];
  // SlotCount
  /// RAM physical slots count
  UINT32              SlotCount;
  // Slots
  /// RAM physicak slots
  SLOT_INFORMATION    Slots[SLOT_COUNT];

};

// GetSystemInformation
/// Get system information
/// @param Information On output, the system information, needs freeing
/// @return Whether the ssytem information was returned successfully or not
/// @retval EFI_INVALID_PARAMETER If Information is NULL or *Information is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the system information was returned
EFI_STATUS
EFIAPI
GetSystemInformation (
  OUT SYSTEM_INFORMATION **Information
);

// GetCPUVendor
/// Get CPU vendor information
/// @return The CPU vendor information
CPU_VENDOR
EFIAPI
GetCPUVendor (
  VOID
);
// GetCPUFamily
/// Get CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
GetCPUFamily (
  VOID
);
// GetCPUModel
/// Get CPU model information
/// @return The CPU model information
CPU_MODEL
EFIAPI
GetCPUModel (
  VOID
);
// GetCPUFrequency
/// Get CPU frequency in Hz
/// @return The CPU frequency in Hz
UINT64
EFIAPI
GetCPUFrequency (
  VOID
);
// IsCPUMobile
/// Check if CPU is mobile
BOOLEAN
EFIAPI
IsCPUMobile (
  VOID
);
// IsCPUBigEndian
/// Check if CPU is little-endian byte order
BOOLEAN
EFIAPI
IsCPUBigEndian (
  VOID
);
// IsCPULittleEndian
/// Check if CPU is little-endian byte order
BOOLEAN
EFIAPI
IsCPULittleEndian (
  VOID
);

// FindDevices
/// Find PCI devices
/// @param VendorId  The vendor id of the PCI device
/// @param DeviceId  The device id of the PCI device
/// @param BaseClass The base class of the PCI device
/// @param SubClass  The sub class of the PCI device
/// @param Options   The options for matching the PCI devices
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
STATIC EFI_STATUS
EFIAPI
FindDevices (
  IN  UINT16                 VendorId,
  IN  UINT16                 DeviceId,
  IN  UINT8                  BaseClass,
  IN  UINT8                  SubClass,
  IN  UINTN                  Options,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesById
/// Find PCI devices by base and sub classes
/// @param VendorId  The vendor id of the PCI device
/// @param DeviceId  The device id of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesById (
  IN  UINT16                 VendorId,
  IN  UINT16                 DeviceId,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesByVendorId
/// Find PCI devices by base classes
/// @param VendorId The vendor id of the PCI device
/// @param Count    On output, the count of PCI devices
/// @param Devices  On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesByVendorId (
  IN  UINT16                 VendorId,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesByClass
/// Find PCI devices by base and sub classes
/// @param BaseClass The base class of the PCI device
/// @param SubClass  The sub class of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesByClass (
  IN  UINT8                  BaseClass,
  IN  UINT8                  SubClass,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);
// FindDevicesByBaseClass
/// Find PCI devices by base classes
/// @param BaseClass The base class of the PCI device
/// @param Count     On output, the count of PCI devices
/// @param Devices   On output, the PCI devices, which needs freed
/// @return Whether any devices were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Devices is NULL or *Devices is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If no devices were found
/// @retval EFI_SUCCESS           If devices were found successfully
EFI_STATUS
EFIAPI
FindDevicesByBaseClass (
  IN  UINT8                  BaseClass,
  OUT UINTN                 *Count,
  OUT EFI_PCI_IO_PROTOCOL ***Devices
);

#endif // __CPU_INFORMATION_LIBRARY_HEADER__

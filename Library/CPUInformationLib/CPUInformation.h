//
/// @file Library/CPUInformationLib/CPUInformation.h
///
/// CPU information library
///

#pragma once
#ifndef __CPU_INFORMATION_HEADER__
#define __CPU_INFORMATION_HEADER__

#include <Library/CPUInformationLib.h>

// CPUID
/// Perform CPUID
#define CPUID(Index) AsmCpuid(Index, mCPUIDRegisters, mCPUIDRegisters + 1, mCPUIDRegisters + 2, mCPUIDRegisters + 3)
// CPUIDEx
/// Perform extended CPUID
#define CPUIDEx(Index, SubIndex) AsmCpuidEx(Index, SubIndex, mCPUIDRegisters, mCPUIDRegisters + 1, mCPUIDRegisters + 2, mCPUIDRegisters + 3)
// CPUBitmask
/// Get a CPU bit mask
/// @param Bits  The bits to mask
/// @param Start The mask starting offset from 0 to 64
/// @param End   The mask ending offset from 0 to 64
/// @return The bit mask
#define CPUBitmask(Bits, Start, End) (RShiftU64((UINT64)Bits, (UINTN)Start) & (LShiftU64(1ULL, (((UINTN)End - (UINTN)Start) + 1)) - 1))

// mCPUIDRegisters
/// Registers for CPUID results
extern UINT32          mCPUIDRegisters[4];
// mCPUMaxIndex
/// CPU maximum CPUID index
extern UINT32          mCPUMaxIndex;
// mCPUMaxExtIndex
/// CPU maximum extended CPUID index
extern UINT32          mCPUMaxExtIndex;
// mCPUInformation
/// CPU information
extern CPU_INFORMATION mCPUInformation;

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

// DetermineCPUIntelModelAndFeatures
/// Determine CPU model and features information
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUIntelModelAndFeatures (
  VOID
);
// DetermineCPUIntelFamily
/// Determine CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUIntelFamily (
  VOID
);
// DetermineCPUIntelFrequency
/// Determine CPU frequency information
VOID
EFIAPI
DetermineCPUIntelFrequency (
  VOID
);

#endif

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64)

// DetermineCPUAMDModelAndFeatures
/// Determine CPU model and features information
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUAMDModelAndFeatures (
  VOID
);
// DetermineCPUAMDFamily
/// Determine CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUAMDFamily (
  VOID
);
// DetermineCPUAMDFrequency
/// Determine CPU frequency information
VOID
EFIAPI
DetermineCPUAMDFrequency (
  VOID
);

#endif

#endif // __CPU_INFORMATION_HEADER__

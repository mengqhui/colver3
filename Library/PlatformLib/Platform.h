//
/// @file Library/PlatformLib/Platform.h
///
/// Platform information library
///

#pragma once
#ifndef __CPU_INFORMATION_HEADER__
#define __CPU_INFORMATION_HEADER__

#include <Library/PlatformLib.h>

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
extern UINT32             mCPUIDRegisters[4];
// mCPUMaxIndex
/// CPU maximum CPUID index
extern UINT32             mCPUMaxIndex;
// mCPUMaxExtIndex
/// CPU maximum extended CPUID index
extern UINT32             mCPUMaxExtIndex;
// mCPUInformation
/// CPU information
extern SYSTEM_INFORMATION mSystemInformation;

// DetectMemoryInformation
/// Detect memory information from SPD
VOID
EFIAPI
DetectMemoryInformation (
  VOID
);
// PrintMemoryInformation
/// Print memory information
VOID
EFIAPI
PrintMemoryInformation (
  VOID
);

// UpdatePackageInformation
/// Update package information
/// @param Index The physical package index
EFI_STATUS
EFIAPI
UpdatePackageInformation (
  IN UINT32 Index
);
// UpdatePackagesInformation
/// Update all packages information
VOID
EFIAPI
UpdatePackagesInformation (
  VOID
);
// PrintCPUInformation
/// Print CPU package information
VOID
EFIAPI
PrintCPUInformation (
  VOID
);

#endif // __CPU_INFORMATION_HEADER__

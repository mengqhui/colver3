///
/// @file Library/PlatformLib/AMD.c
///
/// Platform library AMD CPUs
///

#include "Platform.h"

// DetermineCPUAMDModelAndFeatures
/// TODO: Determine CPU model and features information
/// @param Index The physical package index
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUAMDModelAndFeatures (
  IN UINT32 Index
) {
  return CPU_MODEL_UNKNOWN;
}
// DetermineCPUAMDFamily
/// TODO: Determine CPU family information
/// @param Index The physical package index
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUAMDFamily (
  IN UINT32 Index
) {
  return CPU_FAMILY_UNKNOWN;
}
// DetermineCPUAMDFrequency
/// TODO: Determine CPU frequency information
/// @param Index The physical package index
VOID
EFIAPI
DetermineCPUAMDFrequency (
  IN UINT32 Index
) {
}

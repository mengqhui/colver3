///
/// @file Library/CPUInformationLib/AMD.c
///
/// CPU information library AMD CPUs
///

#include "CPUInformation.h"

// DetermineCPUAMDModelAndFeatures
/// Determine CPU model and features information
/// @return The CPU model information
CPU_MODEL
EFIAPI
DetermineCPUAMDModelAndFeatures (
  VOID
) {
  return CPU_MODEL_UNKNOWN;
}
// DetermineCPUAMDFamily
/// Determine CPU family information
/// @return The CPU family information
CPU_FAMILY
EFIAPI
DetermineCPUAMDFamily (
  VOID
) {
  return CPU_FAMILY_UNKNOWN;
}
// DetermineCPUAMDFrequency
/// Determine CPU frequency information
VOID
EFIAPI
DetermineCPUAMDFrequency (
  VOID
) {
}

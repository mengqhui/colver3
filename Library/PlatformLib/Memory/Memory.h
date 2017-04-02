//
/// @file Library/PlatformLib/Memory/Memory.h
///
/// Platform memory information library
///

#pragma once
#ifndef __PLATOFRM_LIBRARY_MEMORY_HEADER__
#define __PLATOFRM_LIBRARY_MEMORY_HEADER__

#include "../Platform.h"

#include <IndustryStandard/SdramSpd.h>

#include <Protocol/PciIo.h>

#include <Library/TimerLib.h>

// MEMORY_SPD_SIZE
/// The size of the SPD information in bytes
#define MEMORY_SPD_SIZE 512
// MEMORY_SPD_TIMEOUT
/// The timeout waiting for the SPD in microseconds (1ms)
#define MEMORY_SPD_TIMEOUT 1000
// MEMORY_SPD_INTERVAL
/// The interval checking the SPD for changes in microseconds (10us)
#define MEMORY_SPD_INTERVAL 10

// GetMemorySPDSize
/// Get the size of the SPD information in bytes
/// @param Type  The SPD module type
/// @param Byte0 Byte zero from the SPD information
/// @return The size of the SPD information in bytes or zero
UINTN
EFIAPI
GetMemorySPDSize (
  IN UINT8 Type,
  IN UINT8 Byte0
);

// GetMemoryManufacturer
/// Get a memory manufacturer string from a JEDEC identifier
/// @param Bank The manufacturer identifier bank
/// @param Id   The manufacturer identifier
/// @return The memory manufacturer string which must be freed or NULL if there was an error
CHAR16 *
EFIAPI
GetMemoryManufacturer (
  IN UINT8 Bank,
  IN UINT8 Id
);
// GetMemoryManufacturerByString
/// Get a memory manufacturer string from a JEDEC identifier
/// @param ManufacturerString The manufacturer string to convert to unicode or to check is a four digit hex identifier string for the manufacturer
/// @return The memory manufacturer string which must be freed or NULL if there was an error
CHAR16 *
EFIAPI
GetMemoryManufacturerByString (
  IN CHAR8 *ManufacturerId
);

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

// DetectMemoryInformationFromIntelDevice
/// Detect memory information from Intel SMBus device
/// @param Device The PCI device
/// @param Index  The index of the device
/// @return The count of detected slots
UINTN
EFIAPI
DetectMemoryInformationFromIntelDevice (
  IN EFI_PCI_IO_PROTOCOL *Device,
  IN UINTN                Index
);

#endif

#endif // __PLATOFRM_LIBRARY_MEMORY_HEADER__

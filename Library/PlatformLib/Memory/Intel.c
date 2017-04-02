///
/// @file Library/PlatformLib/Memory/Intel.c
///
/// Intel SMBus
///

#include "Memory.h"

#include <Library/ConfigLib.h>

#include <Library/IoLib.h>

// SetMemoryPageOnIntelDevice
/// Set the read page
/// @param Address The base address of the I/O memory
/// @param Page    The page (0 or 1)
/// @retval TRUE  The page was changed
/// @retval FALSE The page change failed
STATIC BOOLEAN
EFIAPI
SetMemoryPageOnIntelDevice (
  IN UINTN Address,
  IN UINTN Page
) {
  UINTN Counter;

  // Check parameters
  if ((Address == 0) || (Page > 1)) {
    return FALSE;
  }
  // Reset the SMBUS
  IoWrite8(Address, 0xFF);
  // Wait until the SMBus is ready
  Counter = 0;
  while (((IoRead8(Address) & 1) != 0) && (++Counter < (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL))) {
    MicroSecondDelay(MEMORY_SPD_INTERVAL);
  }
  // Check for time out
  if (Counter >= (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL)) {
    return FALSE;
  }
  // Send the command to change page
  IoWrite8(Address + 5, 0);
  IoWrite8(Address + 4, (UINT8)(((0x36 + Page) << 1) & 0xFF));
  IoWrite8(Address + 3, 0);
  IoWrite8(Address + 2, 0x40);
  // Wait for the command to finish
  Counter = 0;
  while (((IoRead8(Address) & 6) == 0) && (++Counter < (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL))) {
    MicroSecondDelay(MEMORY_SPD_INTERVAL);
  }
  // Check for time out or error
  if ((Counter >= (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL)) || ((IoRead8(Address) & 2) == 0)) {
    return FALSE;
  }
  return TRUE;
}

// ReadMemoryFromIntelDevice
/// Read a byte from an Intel SMBus device
/// @param Address The base address of the I/O memory
/// @param Index   The index of the SMBus command (0x50 - 0x57 for SPD for memory)
/// @param Offset  The offset of the byte to read from the SMBus command memory
/// @return The byte read from the device or zero if there was an error
STATIC UINT8
EFIAPI
ReadMemoryFromIntelDevice (
  IN UINTN  Address,
  IN UINT8  Index,
  IN UINT16 Offset
) {
  UINTN Counter;

  // Check parameters
  if ((Address == 0) || (Index < 0x50) || (Index > 0x57) || (Offset > MEMORY_SPD_SIZE)) {
    return 0;
  }
  // Reset the SMBUS
  IoWrite8(Address, 0xFF);
  // Wait until the SMBus is ready
  Counter = 0;
  while (((IoRead8(Address) & 1) != 0) && (++Counter < (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL))) {
    MicroSecondDelay(MEMORY_SPD_INTERVAL);
  }
  // Check for time out
  if (Counter >= (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL)) {
    LOG3(LOG_PREFIX_WIDTH - LOG(L"      Slot %u", Index - 0x50), L":", L"Timed out on reset\n");
    return 0;
  }
  // Send the command to retrieve a byte
  IoWrite8(Address + 5, 0);
  IoWrite8(Address + 4, (Index << 1) | 1);
  IoWrite8(Address + 3, (UINT8)(Offset & 0xFF));
  IoWrite8(Address + 2, 0x48);
  // Wait for the command to finish
  Counter = 0;
  while (((IoRead8(Address) & 6) == 0) && (++Counter < (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL))) {
    MicroSecondDelay(MEMORY_SPD_INTERVAL);
  }
  // Check for time out
  if (Counter >= (MEMORY_SPD_TIMEOUT / MEMORY_SPD_INTERVAL)) {
    LOG3(LOG_PREFIX_WIDTH - LOG(L"      Slot %u", Index - 0x50), L":", L"Timed out on read\n");
    return 0;
  }
  // Check for an error
  if ((IoRead8(Address) & 2) == 0) {
    LOG3(LOG_PREFIX_WIDTH - LOG(L"      Slot %u", Index - 0x50), L":", L"Error on read\n");
    return 0;
  }
  // Store the retrieved byte
  return IoRead8(Address + 5);
}

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
) {
  EFI_STATUS Status;
  UINTN      Count = 0;
  UINTN      Address = 0;
  UINT16     PciStatus = 0;
  UINT8      HostStatus = 0;
  UINT8      Slot;
  UINT8      Spd[MEMORY_SPD_SIZE];
  // Check parameters
  if (Device == NULL) {
    return 0;
  }
  // Make sure I/O space access from base address is enabled 
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint16, 4, 1, &PciStatus);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  PciStatus |= 1;
  Status = Device->Pci.Write(Device, EfiPciIoWidthUint16, 4, 1, &PciStatus);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint16, 4, 1, &PciStatus);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  LOG2(L"      Status:", L"0x%04X\n", PciStatus);
  // Make sure host command interface is enabled
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint8, 0x40, 1, &HostStatus);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  HostStatus = ((HostStatus | 0x01) & 0xEF);
  Status = Device->Pci.Write(Device, EfiPciIoWidthUint8, 0x40, 1, &HostStatus);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint8, 0x40, 1, &HostStatus);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  LOG2(L"      Control:", L"0x%02X\n", HostStatus);
  // Read base address from configuration space
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint32, 0x20, 1, &Address);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  // Get the base address from the configuration value
  if (Address == 0) {
    return 0;
  }
  Address &= 0xFFF0;
  LOG2(L"      Address:", L"0x%08X\n", Address);
  // Iterate through each slot
  for (Slot = 0x50; Slot < 0x58; ++Slot) {
    UINT16 Offset;
    // Set the first page
    SetMemoryPageOnIntelDevice(Address, 0);
    // Get the memory module type key byte
    Spd[SPD_MEMORY_TYPE] = ReadMemoryFromIntelDevice(Address, Slot, SPD_MEMORY_TYPE);
    // Check for invalid module type
    if ((Spd[SPD_MEMORY_TYPE] != 0) && (Spd[SPD_MEMORY_TYPE] != 0x0D) && (Spd[SPD_MEMORY_TYPE] <= 0x11)) {
      UINTN Size;
      // Get the SPD size
      Spd[0] = ReadMemoryFromIntelDevice(Address, Slot, 0);
      Size = GetMemorySPDSize(Spd[SPD_MEMORY_TYPE], Spd[0]);
      if (Size != 0) {
        LOG3(LOG_PREFIX_WIDTH - LOG(L"      Slot %u", Slot - 0x50), L":", L"0x%02X\n", Spd[SPD_MEMORY_TYPE]);
        // Iterate through the map to read
        for (Offset = 1; Offset < SPD_MEMORY_TYPE; ++Offset) {
          // Store the retrieved byte
          Spd[Offset] = ReadMemoryFromIntelDevice(Address, Slot, Offset);
        }
        // Skip the type since we already read it
        for (Offset = SPD_MEMORY_TYPE + 1; Offset < MEMORY_SPD_SIZE; ++Offset) {
          if ((Offset == 0x100) && !SetMemoryPageOnIntelDevice(Address, 1)) {
            break;
          }
          // Store the retrieved byte
          Spd[Offset] = ReadMemoryFromIntelDevice(Address, Slot, Offset);
        }
        // Set raw SPD data for this slot
        ConfigSetData(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Raw", Size, Spd, TRUE, Index, Slot - 0x50);
        // Update the new count of SPD slots
        Count = Slot - 0x4F;
      }
    }
  }
  return Count;
}

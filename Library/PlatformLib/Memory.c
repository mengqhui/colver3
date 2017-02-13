///
/// @file Library/PlatformLib/Memory.c
///
/// Memory information
///

#include "Platform.h"

#include <Library/ConfigLib.h>
#include <Library/SmBiosLib.h>

#include <IndustryStandard/Pci.h>
#include <IndustryStandard/SdramSpd.h>

#include <Protocol/PciIo.h>

#include <Library/IoLib.h>
#include <Library/TimerLib.h>

// MEMORY_MAX_SLOTS
/// The maximum memory slots (24 is a multiple of 1, 2, 3, and 4 channels)
#define MEMORY_MAX_SLOTS 24
// MEMORY_SPD_SIZE
/// The size of the SPD information in bytes
#define MEMORY_SPD_SIZE 512
// MEMORY_SPD_TIMEOUT
/// The timeout waiting for the SPD in microseconds (5ms)
#define MEMORY_SPD_TIMEOUT 5000
// MEMORY_SPD_INTERVAL
/// The interval checking the SPD for changes in microseconds (100us)
#define MEMORY_SPD_INTERVAL 100

// mSpdCount
/// The SPD count
STATIC UINT8 mSpdCount = 0;
// mSpd
/// The SPD information for memory
STATIC UINT8 mSpd[MEMORY_MAX_SLOTS][MEMORY_SPD_SIZE] = { 0 };

// PrintMemoryInformation
/// Print memory information
VOID
EFIAPI
PrintMemoryInformation (
  VOID
) {
  UINTN Index;
  Log2(L"Memory modules:", L"%u\n", mSpdCount);
  for (Index = 0; Index < mSpdCount; ++Index) {
    Log2(L"  Memory module:", L"0x%02X\n", mSpd[Index][SPD_MEMORY_TYPE]);
  }
}

// ReadMemoryFromIntelDevice
/// Read a byte from an Intel SMBus device
/// @param Address The base address of the I/O memory
/// @param Index   The index of the SMBus command (0x50 - 0x57 for SPD for memory)
/// @param Offset  The offset of the byte to read from the SMBus command memory
/// @return The byte read from the device or zero if there was an error
STATIC UINT8
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
    LOG3(LOG_PREFIX_WIDTH - LOG(L"  %u", Index - 0x50), L":", L"Timed out on reset\n");
    return 0;
  }
  // Send the command to retrieve a byte
  IoWrite8(Address + 5, 0xFF);
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
    LOG3(LOG_PREFIX_WIDTH - LOG(L"  %u", Index - 0x50), L":", L"Timed out on read\n");
    return 0;
  }
  // Check for an error
  if ((IoRead8(Address) & 2) == 0) {
    LOG3(LOG_PREFIX_WIDTH - LOG(L"  %u", Index - 0x50), L":", L"Error on read\n");
    return 0;
  }
  // Store the retrieved byte
  return IoRead8(Address + 5);
}
// DetectMemoryInformationFromIntelDevice
/// Detect memory information from Intel SMBus device
/// @param Device The PCI device
/// @return Whether the memory was detected or not
STATIC EFI_STATUS
EFIAPI
DetectMemoryInformationFromIntelDevice (
  IN EFI_PCI_IO_PROTOCOL *Device
) {
  EFI_STATUS Status;
  UINTN      Address = 0;
  UINT16     PciStatus = 0;
  UINT8      HostStatus = 0;
  UINT8      Index;
  // Check parameters
  if (Device == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Make sure I/O space access from base address is enabled 
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint16, 4, 1, &PciStatus);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  PciStatus |= 1;
  Status = Device->Pci.Write(Device, EfiPciIoWidthUint16, 4, 1, &PciStatus);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint16, 4, 1, &PciStatus);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  LOG2(L"  PCI status:", L"0x%04X\n", PciStatus);
  // Make sure host command interface is enabled
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint8, 0x40, 1, &HostStatus);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  HostStatus = ((HostStatus | 0x01) & 0xEF);
  Status = Device->Pci.Write(Device, EfiPciIoWidthUint8, 0x40, 1, &HostStatus);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint8, 0x40, 1, &HostStatus);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  LOG2(L"  Host status:", L"0x%02X\n", HostStatus);
  // Read base address from configuration space
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint32, 0x20, 1, &Address);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Get the base address from the configuration value
  Address = Address & 0xFFF0;
  if (Address == 0) {
    return EFI_UNSUPPORTED;
  }
  LOG2(L"  Base address:", L"0x%08X\n", Address);
  // Iterate through each slot
  if (mSpdCount < MEMORY_MAX_SLOTS) {
    for (Index = 0x50; Index < 0x58; ++Index) {
      UINT16 Offset;
      UINT8  MemoryType;
      // Get the memory module type key byte
      MemoryType = ReadMemoryFromIntelDevice(Address, Index, SPD_MEMORY_TYPE);
      // Check for invalid module type
      if ((MemoryType == 0x00) || (MemoryType == 0x0D) || (MemoryType > 0x11)) {
        mSpd[mSpdCount][SPD_MEMORY_TYPE] = (MemoryType == 0xFF) ? 0xFF : 0x00;
        break;
      }
      // Iterate through the map to read
      for (Offset = 0; Offset < MEMORY_SPD_SIZE; ++Offset) {
        // Store the retrieved byte
        mSpd[mSpdCount][Offset] = ReadMemoryFromIntelDevice(Address, Index, Offset);
      }
      // Incrememnt the slot count
      if (++mSpdCount >= MEMORY_MAX_SLOTS) {
        break;
      }
    }
  }
  return EFI_SUCCESS;
}

// DetectMemoryInformationFromDevice
/// Detect memory information from SMBus device
/// @param Device The PCI device
/// @return Whether the memory was detected or not
STATIC EFI_STATUS
EFIAPI
DetectMemoryInformationFromDevice (
  IN EFI_PCI_IO_PROTOCOL *Device
) {
  EFI_STATUS Status;
  UINT16     Ids[2] = { 0, 0 };

  // Check parameters
  if (Device == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  // Get the vendor id of the SMBus device
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint16, 0, ARRAY_SIZE(Ids), Ids);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  LOG2(L"SMBus:", L"0x%04X, 0x%04X\n", Ids[0], Ids[1]);
  switch (Ids[0]) {
    case 0x8086:
      // Intel SMBus
      return DetectMemoryInformationFromIntelDevice(Device);

    case 0x1106:
      // TODO: VIA SMBus
      break;

    case 0x10DE:
      // TODO: NVIDIA SMBus
      break;

    case 0x1039:
      // TODO: SiS SMBus
      break;

    case 0x1002:
      // TODO: AMD SMBus
      break;

    case 0x0446:
      // TODO: LSI SMBus
      break;

    default:
      break;
  }
  // Unknown device
  return EFI_UNSUPPORTED;
}

// PopulateMemoryInformationFromSPD
/// TODO: Populate information for slot from SPD information
/// @param Spd  The SPD information
/// @param Slot The slot information
/// @retval TRUE  The slot is populated and the slot information was populated
/// @retval FALSE The slot is not populated or there was an error
STATIC BOOLEAN
PopulateMemoryInformationFromSPD (
  IN     UINT8            *Spd,
  IN OUT SLOT_INFORMATION *Slot
) {
  //SPD_DDR3  *Ddr3;
  //SPD_DDR4  *Ddr4;
  //SPD_LPDDR *Lpddr;
  // Check parameters
  if ((Slot == NULL) || (Spd == NULL)) {
    return FALSE;
  }
  // Get the memory by type
  switch (Spd[SPD_MEMORY_TYPE]) {
    case SPD_VAL_SDR_TYPE:
      // SD RAM
      return TRUE;

    case SPD_VAL_DDR_TYPE:
      // DDR RAM
      return TRUE;

    case SPD_VAL_DDR2_TYPE:
      // DDR2 RAM
      return TRUE;

    case SPD_VAL_DDR3_TYPE:
      // DDR3 RAM
      return TRUE;

    case SPD_VAL_DDR4_TYPE:
      // DDR4 RAM
      return TRUE;

    case SPD_VAL_LPDDR3_TYPE:
      // LPDDR3 RAM
    case SPD_VAL_LPDDR4_TYPE:
      // LPDDR4_RAM
      return TRUE;

    default:
      break;
  }
  // Unknown memory or unpopulated
  return FALSE;
}
// PopulateMemoryInformation
/// Populate memory slot information from SPD information
STATIC VOID
PopulateMemoryInformation (
  VOID
) {
  UINT8 Index;

  // Populate the slot information from the SPD information
  for (Index = 0; Index < mSpdCount; ++Index) {
    if (PopulateMemoryInformationFromSPD(mSpd[Index], mSystemInformation.Slots + Index)) {
      mSystemInformation.SlotCount = Index + 1;
    }
  }
  // Fix slot count and assume there is an even count except for one
  if ((mSystemInformation.SlotCount > 1) && ((mSystemInformation.SlotCount & 1) != 0)) {
    ++mSystemInformation.SlotCount;
  }
}

// DetectMemoryInformation
/// Detect memory information from SMBus devices
VOID
EFIAPI
DetectMemoryInformation (
  VOID
) {
  EFI_PCI_IO_PROTOCOL **Devices = NULL;
  SMBIOS_STRUCTURE    **Tables = NULL;
  UINTN                 Count = 0;
  UINTN                 Index;

  // Check if memory information should be detected from SMBus SPD commands
  if (ConfigGetBooleanWithDefault(L"\\Memory\\Detect", TRUE)) {
    // Get all SMBus devices
    Count = 0;
    Devices = NULL;
    if (!EFI_ERROR(FindDevicesByClass(0x0C, 0x05, &Count, &Devices)) && (Devices != NULL)) {
      // Populate the SPD information from each device
      for (Index = 0; Index < Count; ++Index) {
        DetectMemoryInformationFromDevice(Devices[Index]);
      }
      FreePool(Devices);
    }
    // Populate memory slot information with SPD information
    PopulateMemoryInformation();
  }
  // Check if Memory information should be updated by SMBIOS
  if ((mSystemInformation.SlotCount == 0) ||
      !ConfigGetBooleanWithDefault(L"\\Memory\\Detect", TRUE) ||
       ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override", FALSE) ||
       ConfigGetBooleanWithDefault(L"\\SMBIOS\\Override\\Memory", TRUE)) {
    // Update memory information with information from SMBIOS
    Count = 0;
    if (!EFI_ERROR(FindSmBiosTables(SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY, &Count, &Tables)) && (Tables != NULL)) {
      UINT32 SlotIndex = 0;
      // Iterate through the physical memory arrays
      LOG2(L"Memory arrays:", L"%u\n", Count);
      for (Index = 0; Index < Count; ++Index) {
        SMBIOS_STRUCTURE    *Table;
        // Get the physical memory array table
        SMBIOS_TABLE_TYPE16 *Type16 = (SMBIOS_TABLE_TYPE16 *)Tables[Index];
        if ((Type16 == NULL) || (Type16->Use != MemoryArrayUseSystemMemory)) {
          continue;
        }
        LOG2(L"  Memory array:", L"%u\n", Index);
        LOG2(L"    Device count:", L"%u\n", Type16->NumberOfMemoryDevices);
        // Get the first memory slot table for this memory array
        Table = GetNextSmBiosTable((SMBIOS_STRUCTURE *)Type16);
        while ((Table != NULL) && (Table->Type == SMBIOS_TYPE_MEMORY_DEVICE)) {
          //SMBIOS_TABLE_TYPE17 *Type17 = (SMBIOS_TABLE_TYPE17 *)Table;
          // TODO: Override memory slot information from this memory slot table

          // Get the next memory slot table for this memory array
          Table = GetNextSmBiosTable(Table);
          ++SlotIndex;
        }
      }
      // Set the slot count
      if (SlotIndex > mSystemInformation.SlotCount) {
        mSystemInformation.SlotCount = SlotIndex;
      }
    }
  }
}

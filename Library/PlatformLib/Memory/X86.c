///
/// @file Library/PlatformLib/Memory/X86.c
///
/// Memory information
///

#include "Memory.h"

#include <Library/ConfigLib.h>
#include <Library/SmBiosLib.h>

// MEMORY_SLOT_PREFIX_WIDTH
/// The prefix width for memory slot log prefixes, in characters
#define MEMORY_SLOT_PREFIX_WIDTH 24

// PrintMemoryInformation
/// Print memory information
VOID
EFIAPI
PrintMemoryInformation (
  VOID
) {
  UINTN   Count;
  UINTN   SlotCount;
  UINTN   Index;
  UINTN   SlotIndex;
  UINTN   Unsigned;
  CHAR16 *String;
  VOID   *Data;
  // Get the count of memory arrays
  Count = ConfigGetUnsignedWithDefault(L"\\Memory\\Count", 0);
  if (Count != 0) {
    // Iterate through each memory array
    Log2(L"  Memory arrays:", L"%u\n", Count);
    for (Index = 0; Index < Count; ++Index) {
      // Get count of slots in this memory array
      SlotCount = ConfigGetUnsignedWithDefault(L"\\Memory\\Array\\%u\\Count", 0, Index);
      Log2(L"    Memory array:", L"%u\n", Index);
      if (SlotCount != 0) {
        // Iterate through each slot in this array
        Log2(L"      Slot count:", L"%u\n", SlotCount);
        for (SlotIndex = 0; SlotIndex < SlotCount; ++SlotIndex) {
          // Output module size
          Unsigned = ConfigGetUnsignedWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\Size", 0, Index, SlotIndex);
          if (Unsigned != 0) {
            Log2(L"        Slot:", L"%u\n", SlotIndex);
            if (Unsigned < 1000000) {
              Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Size:", L"%u kB\n", Unsigned / 1000);
            } else {
              Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Size:", L"%u MB\n", Unsigned / 1000000);
            }
            // Output module speed
            Unsigned = ConfigGetUnsignedWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\Speed", 0, Index, SlotIndex);
            if (Unsigned != 0) {
              Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Speed:", L"%u MT/s\n", Unsigned / 1000000);
            }
            // Output module manufacturer
            String = ConfigGetStringWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\Manufacturer", NULL, Index, SlotIndex);
            if (String != NULL) {
              Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Manufacturer:", L"%s\n", String);
            }
            // Output module serial number
            String = ConfigGetStringWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\SerialNumber", NULL, Index, SlotIndex);
            if (String != NULL) {
              Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Serial number:", L"%s\n", String);
            }
            // Output module part number
            String = ConfigGetStringWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\PartNumber", NULL, Index, SlotIndex);
            if (String != NULL) {
              Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Part number:", L"%s\n", String);
            }
            // Output module SPD raw data
            Unsigned = 0;
            Data = ConfigGetDataWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Raw", 0, NULL, &Unsigned, Index, SlotIndex);
            if ((Unsigned != 0) && (Data != NULL)) {
              String = NULL;
              if (!EFI_ERROR(ToBase64(Unsigned, Data, NULL, &String)) && (String != NULL)) {
                Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          SPD:", L"(%u) %s\n", Unsigned, String);
                FreePool(String);
              }
            }
            // Output module SMBIOS raw data
            Unsigned = 0;
            Data = ConfigGetDataWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Raw", 0, NULL, &Unsigned, Index, SlotIndex);
            if ((Unsigned != 0) && (Data != NULL)) {
              String = NULL;
              if (!EFI_ERROR(ToBase64(Unsigned, Data, NULL, &String)) && (String != NULL)) {
                Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          SMBIOS:", L"(%u) %s\n", Unsigned, String);
                FreePool(String);
              }
            }
          }
        }
      }
    }
  }
}

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
) {
  // Get the memory by type
  switch (Type) {
    case SPD_VAL_SDR_TYPE:
      // SD RAM
    case SPD_VAL_DDR_TYPE:
      // DDR RAM
    case SPD_VAL_DDR2_TYPE:
      // DDR2 RAM
      return (UINTN)Byte0;

    case SPD_VAL_DDR3_TYPE:
      // DDR3 RAM
      switch ((Byte0 >> 4) & 0x7) {
        case 1:
          return 256;

        default:
          break;
      }
      switch (Byte0 & 0xF) {
        case 1:
          return 128;

        case 2:
          return 176;

        case 3:
          return 256;

        default:
          break;
      }
      break;

    case SPD_VAL_DDR4_TYPE:
      // DDR4 RAM
      switch ((Byte0 >> 4) & 0x7) {
        case 1:
          return 256;

        case 2:
          return 512;

        default:
          break;
      }
      switch (Byte0 & 0xF) {
        case 1:
          return 128;

        case 2:
          return 256;

        case 3:
          return 384;

        case 4:
          return 512;

        default:
          break;
      }
      break;

    case SPD_VAL_LPDDR3_TYPE:
      // LPDDR3 RAM
    case SPD_VAL_LPDDR4_TYPE:
      // LPDDR4_RAM
      break;

    default:
      // Unknown memory or unpopulated
      break;
  }
  // Unknown size
  return 0;
}

// PopulateMemoryInformationFromSPD
/// Populate information for slot from SPD information
/// @param Index The index of the array
/// @param Slot  The slot number
STATIC VOID
EFIAPI
PopulateMemoryInformationFromSPD (
  IN UINTN  Index,
  IN UINTN  Slot
) {
  UINTN   Unsigned;
  UINTN   Size = 0;
  UINT8  *Data = NULL;
  CHAR8  *Ascii;
  CHAR16 *String;

  // Get SPD data for this slot
  Data = (UINT8 *)ConfigGetDataWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Raw", 0, NULL, &Size, Index, Slot);
  if ((Data != NULL) && (Size > SPD_MEMORY_TYPE)) {

    // Get the memory by type
    switch (Data[SPD_MEMORY_TYPE]) {
      case SPD_VAL_SDR_TYPE:
        // SD RAM
        break;

      case SPD_VAL_DDR_TYPE:
        // DDR RAM
        break;

      case SPD_VAL_DDR2_TYPE:
        // DDR2 RAM
        break;

      case SPD_VAL_DDR3_TYPE:
        // DDR3 RAM
        LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          Type:", L"0x%02X\n", SPD_VAL_DDR3_TYPE);
        // Set memory module type
        ConfigSetUnsigned(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Type", (UINTN)SPD_VAL_DDR3_TYPE, TRUE, Index, Slot);
        if (Size >= OFFSET_OF(SPD_DDR3, Module)) {
          SPD_DDR3 *Ddr3 = (SPD_DDR3 *)Data;
          // Get module size
          if (Ddr3->General.ModuleOrganization.Bits.SdramDeviceWidth != 0) {
            // Size = (1 << (Density + 5)) * (1 << (BusWidth + 3)) / (1 << (DeviceWidth + 2)) * (Ranks + 1)
            Unsigned = (((UINTN)1) << (((UINTN)Ddr3->General.SdramDensityAndBanks.Bits.Density) + 5));
            Unsigned *= (((UINTN)1) << (((UINTN)Ddr3->General.ModuleMemoryBusWidth.Bits.PrimaryBusWidth) + 3));
            Unsigned /= (((UINTN)1) << (((UINTN)Ddr3->General.ModuleOrganization.Bits.SdramDeviceWidth) + 2));
            Unsigned *= (((UINTN)Ddr3->General.ModuleOrganization.Bits.RankCount) + 1);
            // Set module size
            if (Unsigned != 0) {
              LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          Size:", L"%u MB\n", Unsigned);
              Unsigned *= 1000000;
              ConfigSetUnsigned(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Size", Unsigned, TRUE, Index, Slot);
            }
          }
          // Get module speed
          if (Ddr3->General.MediumTimebase.Divisor.Bits.Divisor != 0) {
            // Set the medium timebase clock
            UINT64 Speed = DivU64x32(MultU64x32(MultU64x32(1000000ULL, (UINT32)Ddr3->General.tCKmin.Bits.tCKmin),
                                                (UINT32)Ddr3->General.MediumTimebase.Dividend.Bits.Dividend),
                                     (UINT32)Ddr3->General.MediumTimebase.Divisor.Bits.Divisor);
            if (Ddr3->General.FineTimebase.Bits.Divisor != 0) {
              // Modify the clock by the fine timebase offset
              Speed += DivU64x32(MultU64x32(MultU64x32(1000ULL, (UINT32)Ddr3->General.tCKminFine.Bits.tCKminFine),
                                            (UINT32)Ddr3->General.FineTimebase.Bits.Dividend),
                                 (UINT32)Ddr3->General.FineTimebase.Bits.Divisor);
            }
            // Output the base speed of the module
            if (Speed != 0) {
              Unsigned = (UINTN)DivU64x64Remainder(2000000000000000ULL, Speed, NULL);
              if (Unsigned != 0) {
                LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          Speed:", L"%u MT/s\n", Unsigned / 1000000);
                ConfigSetUnsigned(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Speed", Unsigned, TRUE, Index, Slot);
              }
            }
            // TODO: Determine if module supports EPP
            if ((Size >= (OFFSET_OF(SPD_DDR3, ManufacturerSpecificData) + sizeof(Ddr3->ManufacturerSpecificData))) &&
                (Ddr3->ManufacturerSpecificData.ManufacturerSpecificData[0] == 0x9C)) {
              LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          EPP support:", L"true\n");
              ConfigSetBoolean(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\EPP\\Support", TRUE, TRUE, Index, Slot);
            } else {
              // No EPP support
              LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          EPP support:", L"false\n");
            }
            // Determine if module supports XMP
            if ((Size >= (OFFSET_OF(SPD_DDR3, Reserved) + sizeof(Ddr3->Reserved))) &&
                (Ddr3->Reserved[0] == 0x0C) && (Ddr3->Reserved[1] == 0x4A) && ((Ddr3->Reserved[2] & 3) != 0)) {
              LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          XMP support:", L"true\n");
              ConfigSetBoolean(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\XMP\\Support", TRUE, TRUE, Index, Slot);
              // Get medium timebase from XMP profile 1 (enthusiast)
              if ((Ddr3->Reserved[2] & 1) != 0) {
                Speed = DivU64x32(MultU64x32(MultU64x32(1000000ULL, (UINT32)Ddr3->Reserved[10]),
                                             (UINT32)Ddr3->Reserved[4]), (UINT32)Ddr3->Reserved[5]);
              }
              // Output the speed of XMP profile 1 of the module
              if (Speed != 0) {
                Unsigned = (UINTN)DivU64x64Remainder(2000000000000000ULL, Speed, NULL);
                if (Unsigned != 0) {
                  LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          XMP1 speed:", L"%u MT/s\n", Unsigned / 1000000);
                  ConfigSetUnsigned(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\XMP\\Profile\\0\\Speed", Unsigned, TRUE, Index, Slot);
                }
              }
              // Get medium timebase from XMP profile 2 (extreme)
              if ((Ddr3->Reserved[2] & 2) != 0) {
                Speed = DivU64x32(MultU64x32(MultU64x32(1000000ULL, (UINT32)Ddr3->Reserved[45]),
                                             (UINT32)Ddr3->Reserved[6]), (UINT32)Ddr3->Reserved[7]);
              }
              // Output the speed of XMP profile 2 of the module
              if (Speed != 0) {
                Unsigned = (UINTN)DivU64x64Remainder(2000000000000000ULL, Speed, NULL);
                if (Unsigned != 0) {
                  LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          XMP2 speed:", L"%u MT/s\n", Unsigned / 1000000);
                  ConfigSetUnsigned(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\XMP\\Profile\\1\\Speed", Unsigned, TRUE, Index, Slot);
                }
              }
            } else {
              // No XMP support
              LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          XMP support:", L"false\n");
            }
            // Determine if module identifiers available
            if (Size >= (OFFSET_OF(SPD_DDR3, ModuleId) + sizeof(Ddr3->ModuleId))) {
              // Get memory module manufacturer
              String = GetMemoryManufacturer(Ddr3->ModuleId.IdCode.Data8[0], Ddr3->ModuleId.IdCode.Data8[1]);
              if (String != NULL) {
                // Set memory module manufacturer
                ConfigSetString(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Manufacturer", String, TRUE, Index, Slot);
                LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          Manufacturer:", L"%s\n", String);
                FreePool(String);
              }
              // Get memory module serial number
              String = CatSPrint(NULL, L"%08X", Ddr3->ModuleId.SerialNumber.Data);
              if (String != NULL) {
                // Set memory module serial number
                ConfigSetString(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\SerialNumber", String, TRUE, Index, Slot);
                LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          Serial number:", L"%s\n", String);
                FreePool(String);
              }
              // Get memory module part number
              if (Size >= (OFFSET_OF(SPD_DDR3, ModulePartNumber) + sizeof(Ddr3->ModulePartNumber))) {
                // Trim any trailing white space or nulls
                UINTN Offset = ARRAY_SIZE(Ddr3->ModulePartNumber.ModulePartNumber);
                while (Offset > 0) {
                  if ((Ddr3->ModulePartNumber.ModulePartNumber[Offset - 1] != '\0') &&
                      (Ddr3->ModulePartNumber.ModulePartNumber[Offset - 1] != ' ')) {
                    break;
                  }
                  --Offset;
                }
                // Set memory module part number
                if (Offset > 0) {
                  Ascii = AsciiStrnDup((CHAR8 *)Ddr3->ModulePartNumber.ModulePartNumber, Offset);
                  if (Ascii != NULL) {
                    String = FromAscii(Ascii);
                    if (String != NULL) {
                      ConfigSetString(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\PartNumber", String, TRUE, Index, Slot);
                      FreePool(String);
                    }
                    LOG3(MEMORY_SLOT_PREFIX_WIDTH, L"          Part number:", L"%a\n", Ascii);
                    FreePool(Ascii);
                  }
                }
              }
            }
          }
        }
        break;

      case SPD_VAL_DDR4_TYPE:
        // DDR4 RAM
        //SPD_DDR4  *Ddr4;
        break;

      case SPD_VAL_LPDDR3_TYPE:
        // LPDDR3 RAM
      case SPD_VAL_LPDDR4_TYPE:
        // LPDDR4_RAM
        //SPD_LPDDR *Lpddr;
        break;

      default:
        // Unknown memory or unpopulated
        break;
    }
  }
}
// DetectMemoryInformationFromDevice
/// Detect memory information from SMBus device
/// @param Device The PCI device
/// @param Index  The index of the device
/// @return The count of detected slots
STATIC UINTN
EFIAPI
DetectMemoryInformationFromDevice (
  IN EFI_PCI_IO_PROTOCOL *Device,
  IN UINTN                Index
) {
  EFI_STATUS Status;
  UINT16     Ids[2] = { 0, 0 };

  // Check parameters
  if (Device == NULL) {
    return 0;
  }

  // Get the vendor id of the SMBus device
  Status = Device->Pci.Read(Device, EfiPciIoWidthUint16, 0, ARRAY_SIZE(Ids), Ids);
  if (EFI_ERROR(Status)) {
    return 0;
  }
  LOG2(L"    SMBus:", L"0x%04X, 0x%04X\n", Ids[0], Ids[1]);
  switch (Ids[0]) {
    case 0x8086:
      // Intel SMBus
      return DetectMemoryInformationFromIntelDevice(Device, Index);

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
  return 0;
}

// PopulateMemoryInformationFromSMBIOS
/// Populate information for slot from SMBIOS information
/// @param Index   The index of the array
/// @param Slot    The slot number
/// @param Version The SMBIOS version
STATIC VOID
EFIAPI
PopulateMemoryInformationFromSMBIOS (
  IN UINTN       Index,
  IN UINTN       Slot,
  IN VERSIONCODE Version
) {
  SMBIOS_STRUCTURE_POINTER  Table;
  SMBIOS_STRUCTURE_POINTER  Table2;
  CHAR16                   *String;
  CHAR8                    *Ascii;
  UINTN                     Size;
  UINTN                     Unsigned;

  // Get SMBIOS data for this slot
  Size = 0;
  Table.Hdr = (SMBIOS_STRUCTURE *)ConfigGetDataWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Raw", 0, NULL, &Size, Index, Slot);
  if ((Table.Hdr != NULL) && (Size != 0)) {
    // Check to make sure this module does not have an unrecoverable error
    if (Table.Type17->MemoryErrorInformationHandle < 0xFFFE) {
      Table2 = FindSmBiosTableByHandle(Table.Type17->MemoryErrorInformationHandle);
      if ((Table2.Hdr == NULL) ||
          ((Table2.Hdr->Type != SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION) &&
            (Table2.Hdr->Type != SMBIOS_TYPE_64BIT_MEMORY_ERROR_INFORMATION))) {
        LOG2(L"          Error:", L"Invalid information, skipping table\n");
        return;
      }
      // 32bit error information
      if (Table2.Hdr->Type == SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION) {
        if (Table2.Type18->ErrorType != MemoryErrorOk) {
          LOG2(L"          Error:", L"%u %u %u 0x%08X 0x%08X 0x%08X 0x%08X\n", Table2.Type18->ErrorType, Table2.Type18->ErrorGranularity, Table2.Type18->ErrorOperation,
                Table2.Type18->VendorSyndrome, Table2.Type18->MemoryArrayErrorAddress, Table2.Type18->DeviceErrorAddress, Table2.Type18->ErrorResolution);
        }
        // If not OK or corrected then do not use this table
        if ((Table2.Type18->ErrorType != MemoryErrorOk) &&
            (Table2.Type18->ErrorType != MemoryErrorCorrectSingleBit) &&
            (Table2.Type18->ErrorType != MemoryErrorCorrected)) {
          LOG2(L"          Error:", L"Uncorrectable, skipping table\n");
          return;
        }
      } else {
        // 64bit error information
        if (Table2.Type33->ErrorType != MemoryErrorOk) {
          LOG2(L"          Error:", L"%u %u %u 0x%08X 0x%016X 0x%016X 0x%08X\n", Table2.Type33->ErrorType, Table2.Type33->ErrorGranularity, Table2.Type33->ErrorOperation,
                Table2.Type33->VendorSyndrome, Table2.Type33->MemoryArrayErrorAddress, Table2.Type33->DeviceErrorAddress, Table2.Type33->ErrorResolution);
        }
        // If not OK or corrected then do not use this table
        if ((Table2.Type33->ErrorType != MemoryErrorOk) &&
            (Table2.Type33->ErrorType != MemoryErrorCorrectSingleBit) &&
            (Table2.Type33->ErrorType != MemoryErrorCorrected)) {
          LOG2(L"    Error:", L"Uncorrectable, skipping table\n");
          return;
        }
      }
    }
    // Get memory module width
    Unsigned = (UINTN)(Table.Type17->TotalWidth);
    // Set memory module width
    if (Unsigned != 0) {
      Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Width:", L"0x%04X\n", Unsigned);
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Width", Unsigned, TRUE, Index, Slot);
    }
    // Get memory module data width
    Unsigned = (UINTN)(Table.Type17->DataWidth);
    // Set memory module data width
    if (Unsigned != 0) {
      Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Data width:", L"0x%04X\n", Unsigned);
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\DataWidth", Unsigned, TRUE, Index, Slot);
    }
    // Get memory module size
    Unsigned = 0;
    if ((Table.Type17->Size != 0) && (Table.Type17->Size != 0xFFFF)) {
      if (Table.Type17->Size == 0x7FFF) {
        // Extended size in megabytes
        if ((Version >= PACK_VERSIONCODE(2, 7)) &&
            (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, ExtendedSize) + sizeof(Table.Type17->ExtendedSize))) &&
            (Table.Type17->ExtendedSize != 0)) {
          Unsigned = (UINTN)(Table.Type17->ExtendedSize) * 1000000;
        }
      } else if ((Table.Type17->Size & 0x8000) != 0) {
        // Size in kilobytes
        Unsigned = (UINTN)(Table.Type17->Size & 0x7FFF) * 1000;
      } else {
        // Size in megabytes
        Unsigned = (UINTN)(Table.Type17->Size) * 1000000;
      }
    }
    // Set module size
    if (Unsigned != 0) {
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Size", Unsigned, TRUE, Index, Slot);
      if (Unsigned < 1000000) {
        Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Size:", L"%u kB\n", Unsigned / 1000);
      } else {
        Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Size:", L"%u MB\n", Unsigned / 1000000);
      }
    }
    // Get memory module form factor
    Unsigned = (UINTN)(Table.Type17->FormFactor);
    // Set memory module channel form factor
    if (Unsigned != 0) {
      Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Form factor:", L"0x%02X\n", Unsigned);
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\FormFactor", Unsigned, TRUE, Index, Slot);
    }
    // Get memory module channel set
    Unsigned = (UINTN)(Table.Type17->DeviceSet);
    // Set memory module channel set
    if (Unsigned != 0) {
      Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Channel:", L"%u\n", Unsigned);
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Channel", Unsigned, TRUE, Index, Slot);
    }
    // Get memory module device locator
    if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, DeviceLocator) + sizeof(Table.Type17->DeviceLocator))) {
        Ascii = GetStringFromSmBiosTable(Table, Table.Type17->DeviceLocator);
        // Set memory module device locator
        if (Ascii != NULL) {
          String = FromAscii(Ascii);
          if (String != NULL) {
            ConfigSetString(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\DeviceLocator", String, TRUE, Index, Slot);
            FreePool(String);
          }
          Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Device:", L"%a\n", Ascii);
        }
      }
    // Get memory module bank locator
    if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, BankLocator) + sizeof(Table.Type17->BankLocator))) {
        Ascii = GetStringFromSmBiosTable(Table, Table.Type17->BankLocator);
        // Set memory module bank locator
        if (Ascii != NULL) {
          String = FromAscii(Ascii);
          if (String != NULL) {
            ConfigSetString(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\BankLocator", String, TRUE, Index, Slot);
            FreePool(String);
          }
          Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Bank:", L"%a\n", Ascii);
        }
      }
    // Get memory module type
    Unsigned = (UINTN)(Table.Type17->MemoryType);
    // Set memory module type
    if (Unsigned != 0) {
      Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Type:", L"0x%02X\n", Unsigned);
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Type", Unsigned, TRUE, Index, Slot);
    }
    // TODO: Get memory module type details
    // TODO: Set memory module type details
    // Some features only available 2.3+
    if (Version >= PACK_VERSIONCODE(2, 3)) {
      // Get memory module speed
      if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, Speed) + sizeof(Table.Type17->Speed))) {
        Unsigned = 0;
        // Configured module speed is 2.7+
        if ((Version >= PACK_VERSIONCODE(2, 7)) &&
            (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, ConfiguredMemoryClockSpeed) + sizeof(Table.Type17->ConfiguredMemoryClockSpeed))) &&
            (Table.Type17->ConfiguredMemoryClockSpeed != 0)) {
          // Configured speed in MHz
          Unsigned = (UINTN)(Table.Type17->ConfiguredMemoryClockSpeed) * 1000000;
        } else if (Table.Type17->Speed != 0) {
          // Speed in MHz
          Unsigned = (UINTN)(Table.Type17->Speed) * 1000000;
        }
        // Set module speed
        if (Unsigned != 0) {
          ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Speed", Unsigned, TRUE, Index, Slot);
          Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Speed:", L"%u MT/s\n", Unsigned / 1000000);
        }
      }
      // Get memory module manufacturer
      if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, Manufacturer) + sizeof(Table.Type17->Manufacturer))) {
        Ascii = GetStringFromSmBiosTable(Table, Table.Type17->Manufacturer);
        // Check if this manufacturer name needs translated from a hex string identifier to a manufacturer string
        if (Ascii != NULL) {
          String = GetMemoryManufacturerByString(Ascii);
          // Set memory module manufacturer
          if (String != NULL) {
            Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Manufacturer:", L"%s\n", String);
            ConfigSetString(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Manufacturer", String, TRUE, Index, Slot);
            FreePool(String);
          }
        }
      }
      // Get memory module serial number
      if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, SerialNumber) + sizeof(Table.Type17->SerialNumber))) {
        Ascii = GetStringFromSmBiosTable(Table, Table.Type17->SerialNumber);
        // Set memory module serial number
        if (Ascii != NULL) {
          String = FromAscii(Ascii);
          if (String != NULL) {
            ConfigSetString(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\SerialNumber", String, TRUE, Index, Slot);
            FreePool(String);
          }
          Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Serial number:", L"%a\n", Ascii);
        }
      }
      // Get memory module asset tag
      if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, AssetTag) + sizeof(Table.Type17->AssetTag))) {
        Ascii = GetStringFromSmBiosTable(Table, Table.Type17->AssetTag);
        // Set memory module asset tag
        if (Ascii != NULL) {
          String = FromAscii(Ascii);
          if (String != NULL) {
            ConfigSetString(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\AssetTag", String, TRUE, Index, Slot);
            FreePool(String);
          }
          Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Asset tag:", L"%a\n", Ascii);
        }
      }
      // Get memory module part number
      if (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE17, PartNumber) + sizeof(Table.Type17->PartNumber))) {
        Ascii = GetStringFromSmBiosTable(Table, Table.Type17->PartNumber);
        // Set memory module part number
        if (Ascii != NULL) {
          String = FromAscii(Ascii);
          if (String != NULL) {
            ConfigSetString(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\PartNumber", String, TRUE, Index, Slot);
            FreePool(String);
          }
          Log3(MEMORY_SLOT_PREFIX_WIDTH, L"          Part number:", L"%a\n", Ascii);
        }
      }
      if (Version >= PACK_VERSIONCODE(2, 6)) {
        // TODO: Get memory module attributes
        // TODO: Set memory module attributes

        // NOTE: Populate the voltage information from 2.8+?
      }
    }
  }
}

// DetectMemoryInformation
/// Detect memory information from SMBus devices
VOID
EFIAPI
DetectMemoryInformation (
  VOID
) {
  EFI_PCI_IO_PROTOCOL      **Devices = NULL;
  SMBIOS_STRUCTURE_POINTER  *Tables = NULL;
  SMBIOS_STRUCTURE_POINTER   Table;
  SMBIOS_STRUCTURE_POINTER   Table2;
  VERSIONCODE                SmBiosVersion;
  UINTN                      Count;
  UINTN                      SlotCount;
  UINTN                      Index;
  UINTN                      SlotIndex;
  UINTN                      Unsigned;

  // Check if memory information should be detected from SMBus SPD commands
  if (ConfigGetBooleanWithDefault(L"\\Memory\\Detect", TRUE)) {
    // Get all SMBus devices
    Count = 0;
    Devices = NULL;
    if (!EFI_ERROR(FindDevicesByClass(0x0C, 0x05, &Count, &Devices)) && (Devices != NULL)) {
      // Set the count of SPD memory arrays
      if (Count != 0) {
        ConfigSetUnsigned(L"\\Memory\\SPD\\Count", Count, TRUE);
        LOG2(L"  SPD arrays:", L"%u\n", Count);
        // Populate the SPD information from each device
        for (Index = 0; Index < Count; ++Index) {
          LOG2(L"    Memory array:", L"%u\n", Index);
          SlotCount = DetectMemoryInformationFromDevice(Devices[Index], Index);
          // Set the count of SPD memory slots in this array
          if (SlotCount != 0) {
            LOG2(L"      Slot count:", L"%u\n", SlotCount);
            ConfigSetUnsigned(L"\\Memory\\SPD\\Array\\%u\\Count", SlotCount, TRUE, Index);
            // Populate the configuration from SPD information
            for (SlotIndex = 0; SlotIndex < SlotCount; ++SlotIndex) {
              LOG2(L"        Slot:", L"%u\n", SlotIndex);
              PopulateMemoryInformationFromSPD(Index, SlotIndex);
            }
          }
        }
      }
      FreePool(Devices);
    }
  }
  // Update memory information with information from SMBIOS
  Count = 0;
  if (!EFI_ERROR(FindSmBiosTables(SMBIOS_TYPE_PHYSICAL_MEMORY_ARRAY, &Count, &Tables)) && (Tables != NULL)) {
    if (Count != 0) {
      // Get the SMBIOS version
      SmBiosVersion = GetSmBiosVersion();
      // Iterate through the physical memory arrays
      ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Count", Count, TRUE);
      LOG2(L"  SMBIOS arrays:", L"%u\n", Count);
      for (Index = 0; Index < Count; ++Index) {
        Table = Tables[Index];
        // Get the physical memory array table]
        LOG2(L"    Memory array:", L"%u\n", Index);
        if (Table.Hdr == NULL) {
          continue;
        }
        // Check to make sure this array does not have an unrecoverable error
        if (Table.Type16->MemoryErrorInformationHandle < 0xFFFE) {
          Table2 = FindSmBiosTableByHandle(Table.Type16->MemoryErrorInformationHandle);
          if ((Table2.Hdr == NULL) ||
              ((Table2.Hdr->Type != SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION) &&
               (Table2.Hdr->Type != SMBIOS_TYPE_64BIT_MEMORY_ERROR_INFORMATION))) {
            LOG2(L"      Error:", L"Invalid information, skipping table\n");
            continue;
          }
          // 32bit error information
          if (Table2.Hdr->Type == SMBIOS_TYPE_32BIT_MEMORY_ERROR_INFORMATION) {
            if (Table2.Type18->ErrorType != MemoryErrorOk) {
              LOG2(L"      Error:", L"%u %u %u 0x%08X 0x%08X 0x%08X 0x%08X\n", Table2.Type18->ErrorType, Table2.Type18->ErrorGranularity, Table2.Type18->ErrorOperation,
                   Table2.Type18->VendorSyndrome, Table2.Type18->MemoryArrayErrorAddress, Table2.Type18->DeviceErrorAddress, Table2.Type18->ErrorResolution);
            }
            // If not OK or corrected then do not use this table
            if ((Table2.Type18->ErrorType != MemoryErrorOk) &&
                (Table2.Type18->ErrorType != MemoryErrorCorrectSingleBit) &&
                (Table2.Type18->ErrorType != MemoryErrorCorrected)) {
              LOG2(L"      Error:", L"Uncorrectable, skipping table\n");
              continue;
            }
          } else {
            // 64bit error information
            if (Table2.Type33->ErrorType != MemoryErrorOk) {
              LOG2(L"      Error:", L"%u %u %u 0x%08X 0x%016X 0x%016X 0x%08X\n", Table2.Type33->ErrorType, Table2.Type33->ErrorGranularity, Table2.Type33->ErrorOperation,
                   Table2.Type33->VendorSyndrome, Table2.Type33->MemoryArrayErrorAddress, Table2.Type33->DeviceErrorAddress, Table2.Type33->ErrorResolution);
            }
            // If not OK or corrected then do not use this table
            if ((Table2.Type33->ErrorType != MemoryErrorOk) &&
                (Table2.Type33->ErrorType != MemoryErrorCorrectSingleBit) &&
                (Table2.Type33->ErrorType != MemoryErrorCorrected)) {
              LOG2(L"      Error:", L"Uncorrectable, skipping table\n");
              continue;
            }
          }
        }
        // Get memory array location
        Unsigned = (UINTN)(Table.Type16->Location);
        LOG2(L"      Location:", L"%u\n", Unsigned);
        // Set memory array location
        if (Unsigned != 0) {
          ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Location", Unsigned, TRUE, Index);
        }
        // Get memory array use
        Unsigned = (UINTN)(Table.Type16->Use);
        LOG2(L"      Use:", L"%u\n", Unsigned);
        // Set memory array use
        if (Unsigned != 0) {
          ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Use", Unsigned, TRUE, Index);
        }
        // Get memory array error correction
        Unsigned = (UINTN)(Table.Type16->MemoryErrorCorrection);
        LOG2(L"      Correction:", L"%u\n", Unsigned);
        // Set memory array error correction
        if (Unsigned != 0) {
          ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\ErrorCorrection", Unsigned, TRUE, Index);
        }
        // Get memory array maximum capacity
        Unsigned = 0;
        if ((Table.Type16->MaximumCapacity == 0x80000000) && (SmBiosVersion >= PACK_VERSIONCODE(2, 7)) &&
            (Table.Hdr->Length >= (OFFSET_OF(SMBIOS_TABLE_TYPE16, ExtendedMaximumCapacity) + sizeof(Table.Type16->ExtendedMaximumCapacity)))) {
          if (Table.Type16->ExtendedMaximumCapacity < 1024) {
            Unsigned = (UINTN)(Table.Type16->ExtendedMaximumCapacity) * 1000;
          } else {
            // Convert from kB to MB
            Unsigned = ((UINTN)(Table.Type16->ExtendedMaximumCapacity) >> 10) * 1000000;
          }
        } else if (Table.Type16->MaximumCapacity < 1024) {
          Unsigned = (UINTN)(Table.Type16->MaximumCapacity) * 1000;
        } else {
          // Convert from kB to MB
          Unsigned = ((UINTN)(Table.Type16->MaximumCapacity) >> 10) * 1000000;
        }
        // Set memory array maximum capacity
        if (Unsigned != 0) {
          if (Unsigned < 1000000) {
            LOG2(L"      Capacity:", L"%u kB\n", Unsigned / 1000);
          } else {
            LOG2(L"      Capacity:", L"%u MB\n", Unsigned / 1000000);
          }
          ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Size", Unsigned, TRUE, Index);
        }
        // Get the slot count in the array
        LOG2(L"      Slot count:", L"%u\n", Table.Type16->NumberOfMemoryDevices);
        if (Table.Type16->NumberOfMemoryDevices != 0) {
          ConfigSetUnsigned(L"\\Memory\\SMBIOS\\Array\\%u\\Count", Table.Type16->NumberOfMemoryDevices, TRUE, Index);
          // Traverse each memory slot table for this memory array
          Table2.Hdr = NULL;
          for (SlotIndex = 0; SlotIndex < Table.Type16->NumberOfMemoryDevices;) {
            // Get the next table
            Table2 = FindSmBiosTable(Table2, SMBIOS_TYPE_MEMORY_DEVICE);
            if (Table2.Hdr == NULL) {
              // Ran out of tables
              break;
            }
            // Override memory slot information from this memory slot table
            if (Table2.Type17->MemoryArrayHandle == Table.Type16->Hdr.Handle) {
              // Check if slot is populated by checking size is not zero
              if (Table2.Type17->Size != 0) {
                LOG2(L"        Slot:", L"%u\n", SlotIndex);
                // Populate the SMBIOS information
                if (!EFI_ERROR(ConfigSetData(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Raw", GetSmBiosTableSize(Table2), (VOID *)Table2.Type16, TRUE, Index, SlotIndex))) {
                  PopulateMemoryInformationFromSMBIOS(Index, SlotIndex, SmBiosVersion);
                }
              }
              ++SlotIndex;
            }
          }
        }
      }
    }
    FreePool(Tables);
  }
  // Get the count of memory arrays
  Count = ConfigGetUnsignedWithDefault(L"\\Memory\\Count",
                                       ConfigGetUnsignedWithDefault(L"\\Memory\\SMBIOS\\Count",
                                                                    ConfigGetUnsignedWithDefault(L"\\Memory\\SPD\\Count", 0)));
  if (Count != 0) {
    // Set the count and iterate through each array
    ConfigSetUnsigned(L"\\Memory\\Count", Count, FALSE);
    for (Index = 0; Index < Count; ++Index) {
      // Get the count of slots in this memory array
      SlotCount = ConfigGetUnsignedWithDefault(L"\\Memory\\Array\\%u\\Count",
                                                ConfigGetUnsignedWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Count",
                                                                              ConfigGetUnsignedWithDefault(L"\\Memory\\SPD\\Array\\%u\\Count", 0, Index),
                                                                              Index),
                                                Index);
      if (SlotCount != 0) {
        // Set the count and iterate through each slot
        ConfigSetUnsigned(L"\\Memory\\Array\\%u\\Count", SlotCount, FALSE, Index);
        for (SlotIndex = 0; SlotIndex < SlotCount; ++SlotIndex) {
          CHAR16 *String;
          // Get the size of the module in this slot
          Unsigned = ConfigGetUnsignedWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\Size",
                                                   ConfigGetUnsignedWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Size",
                                                                                 ConfigGetUnsignedWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Size", 0, Index, SlotIndex),
                                                                                 Index, SlotIndex),
                                                   Index, SlotIndex);
          if (Unsigned != 0) {
            // Set the size of the module in this slot
            ConfigSetUnsigned(L"\\Memory\\Array\\%u\\Slot\\%u\\Size", Unsigned, FALSE, Index, SlotIndex);
          }
          // Get the speed of the module in this slot
          Unsigned = ConfigGetUnsignedWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\Speed",
                                                   ConfigGetUnsignedWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Speed",
                                                                                 ConfigGetUnsignedWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Speed", 0, Index, SlotIndex),
                                                                                 Index, SlotIndex),
                                                   Index, SlotIndex);
          if (Unsigned != 0) {
            // Set the speed of the module in this slot
            ConfigSetUnsigned(L"\\Memory\\Array\\%u\\Slot\\%u\\Speed", Unsigned, FALSE, Index, SlotIndex);
          }
          // Get the manufacturer of the module in this slot
          String = ConfigGetStringWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\Manufacturer",
                                               ConfigGetStringWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\Manufacturer",
                                                                           ConfigGetStringWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\Manufacturer", NULL, Index, SlotIndex),
                                                                           Index, SlotIndex),
                                               Index, SlotIndex);
          if (String != NULL) {
            // Set the manufacturer of the module in this slot
            ConfigSetString(L"\\Memory\\Array\\%u\\Slot\\%u\\Manufacturer", String, FALSE, Index, SlotIndex);
          }
          // Get the serial number of the module in this slot
          String = ConfigGetStringWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\SerialNumber",
                                               ConfigGetStringWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\SerialNumber",
                                                                           ConfigGetStringWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\SerialNumber", NULL, Index, SlotIndex),
                                                                           Index, SlotIndex),
                                               Index, SlotIndex);
          if (String != NULL) {
            // Set the serial number of the module in this slot
            ConfigSetString(L"\\Memory\\Array\\%u\\Slot\\%u\\SerialNumber", String, FALSE, Index, SlotIndex);
          }
          // Get the part number of the module in this slot
          String = ConfigGetStringWithDefault(L"\\Memory\\Array\\%u\\Slot\\%u\\PartNumber",
                                               ConfigGetStringWithDefault(L"\\Memory\\SMBIOS\\Array\\%u\\Slot\\%u\\PartNumber",
                                                                           ConfigGetStringWithDefault(L"\\Memory\\SPD\\Array\\%u\\Slot\\%u\\PartNumber", NULL, Index, SlotIndex),
                                                                           Index, SlotIndex),
                                               Index, SlotIndex);
          if (String != NULL) {
            // Set the part number of the module in this slot
            ConfigSetString(L"\\Memory\\Array\\%u\\Slot\\%u\\PartNumber", String, FALSE, Index, SlotIndex);
          }
        }
      }
    }
  }
}

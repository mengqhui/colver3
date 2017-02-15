//
/// @file Library/SmbiosLib/SmbiosLib.c
///
/// SMBIOS library
///

#include <Library/SmBiosLib.h>

#include <Library/LogLib.h>

#include <Guid/SmBios.h>

#include <Library/UefiBootServicesTableLib.h>

// SMBIOS_MAX_TABLE_TYPE
/// SMBIOS max valid table type
#define SMBIOS_MAX_TABLE_TYPE 43

// mSmBiosTable
/// 32bit SMBIOS table
STATIC SMBIOS_TABLE_ENTRY_POINT     *mSmBiosTable = NULL;
// mSmBios3Table
/// 64bit SMBIOS table
STATIC SMBIOS_TABLE_3_0_ENTRY_POINT *mSmBios3Table = NULL;

// GetNextSmBiosTable
/// Get the next SMBIOS table
/// @param Previous The previous SMBIOS structure or NULL to get the first structure
/// @return The next SMBIOS structure or NULL if there are no more
SMBIOS_STRUCTURE *
EFIAPI
GetNextSmBiosTable (
  IN SMBIOS_STRUCTURE *Previous OPTIONAL
) {
  // Check parameters
  if (Previous == NULL) {
    if (mSmBios3Table != NULL) {
      // If SMBIOS 3 present use that
      Previous = (SMBIOS_STRUCTURE *)(UINTN)mSmBios3Table->TableAddress;
    } else if (mSmBiosTable != NULL) {
      // If SMBIOS present use that
      Previous = (SMBIOS_STRUCTURE *)(UINTN)mSmBiosTable->TableAddress;
    }
  } else {
    UINTN  Size = 0;
    UINT8 *Ptr;
    // Get the maximum length of searching just in case
    if (mSmBiosTable != NULL) {
      UINT32 Offset = (UINT32)Previous;
      if ((Offset >= mSmBiosTable->TableAddress) && (Offset < (mSmBiosTable->TableAddress + mSmBiosTable->TableLength))) {
        Size = (UINTN)mSmBiosTable->TableLength - (UINTN)(Offset - mSmBiosTable->TableAddress);
      } else {
        Size = 0;
      }
    } else if (mSmBios3Table != NULL) {
      UINT64 Offset = (UINT64)Previous;
      if ((Offset >= mSmBiosTable->TableAddress) && (Offset < (mSmBiosTable->TableAddress + mSmBiosTable->TableLength))) {
        Size = (UINTN)mSmBios3Table->TableMaximumSize - (UINTN)(Offset - mSmBios3Table->TableAddress);
      } else {
        Size = 0;
      }
    }
    if (Size == 0) {
      Previous = NULL;
    } else {
      UINTN TableSize = 0;
      // Find the string section after this structure
      Ptr = (UINT8 *)Previous;
      Ptr += Previous->Length;
      do {
        do {
          TableSize = Ptr - (UINT8 *)Previous;
        } while ((TableSize < Size) && (*Ptr++ != 0));
      } while ((TableSize < Size) && (*Ptr++ != 0));
      if (TableSize >= Size) {
        Previous = NULL;
      } else {
        Previous = (SMBIOS_STRUCTURE *)Ptr;
      }
    }
  }
  // Check if last entry
  if (Previous != NULL) {
    if (Previous->Type == 0x7F) {
      Previous = NULL;
    }
  }
  // Return next table or NULL if end
  return Previous;
}
// GetSmBiosTableCount
/// Get the count of SMBIOS tables with a specified type
/// @param Type The type of SMBIOS table to count
/// @return The count of SMBIOS tables
UINTN
GetSmBiosTableCount (
  IN UINT8 Type
) {
  UINTN             Count = 0;
  SMBIOS_STRUCTURE *Previous = NULL;
  // Count the tables
  do {
    Previous = FindSmBiosTable(Previous, Type);
    if (Previous == NULL) {
      break;
    }
    ++Count;
  } while (TRUE);
  return Count;
}
// FindSmBiosTable
/// Find the next SMBIOS structure with specified type
/// @param Previous The previous SMBIOS structure or NULL to get the first structure
/// @param Type     The type of SMBIOS table to find
/// @return The next SMBIOS structure or NULL if there are no more
SMBIOS_STRUCTURE *
EFIAPI
FindSmBiosTable (
  IN SMBIOS_STRUCTURE *Previous OPTIONAL,
  IN UINT8             Type
) {
  // Check parameters
  Previous = GetNextSmBiosTable(Previous);
  if ((Type == 0x7F) || (Previous == NULL)) {
    return NULL;
  }
  // Iterate through tables until match or end
  do {
    // Check if type matches
    if (Previous->Type == Type) {
      return Previous;
    }
    // Get next table
    Previous = GetNextSmBiosTable(Previous);
  } while (Previous != NULL);
  // Not found
  return NULL;
}
// FindSmBiosTables
/// Find all the SMBIOS tables with specified type
/// @param Type   The type of SMBIOS tables to find
/// @param Count  On output, the count of SMBIOS tables
/// @param Tables On output, the SMBIOS tables, which needs freed
/// @return Whether any SMBIOS tables were found or not
/// @retval EFI_INVALID_PARAMETER If Count or Tables is NULL or *Tables is not NULL or Type is invalid
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If not SMBIOS tables with specified type was found
/// @retval EFI_SUCCESS           If SMBIOS tables were found successfully
EFI_STATUS
FindSmBiosTables (
  IN  UINT8               Type,
  OUT UINTN              *Count,
  OUT SMBIOS_STRUCTURE ***Tables
) {
  SMBIOS_STRUCTURE **FoundTables = NULL;
  SMBIOS_STRUCTURE  *Previous = NULL;
  UINTN              TableCount;
  UINTN              Index;
  // Check parameters
  if ((Type > SMBIOS_MAX_TABLE_TYPE) || (Count == NULL) || (Tables == NULL) || (*Tables != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the count of tables with specified type
  TableCount = GetSmBiosTableCount(Type);
  if (TableCount == 0) {
    return EFI_NOT_FOUND;
  }
  // Allocate new buffer for tables list
  FoundTables = (SMBIOS_STRUCTURE **)AllocateZeroPool(TableCount * sizeof(SMBIOS_STRUCTURE *));
  if (FoundTables == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Iterate through SMBIOS tables with specified type
  for (Index = 0; Index < TableCount; ++Index) {
    // Get next SMBIOS table with specified type
    Previous = FindSmBiosTable(Previous, Type);
    if (Previous == NULL) {
      break;
    }
    FoundTables[Index] = Previous;
  }
  // Return the found tables
  if (Index == 0) {
    return EFI_NOT_FOUND;
  }
  *Count = Index;
  *Tables = FoundTables;
  return EFI_SUCCESS;
}

// GetStringFromSmBiosTable
/// Get a string from an SMBIOS table
/// @param Table The SMBIOS table
/// @param Index The string index
/// @return The string from the SMBIOS table or NULL if not found
CHAR8 *
EFIAPI
GetStringFromSmBiosTable (
  IN SMBIOS_STRUCTURE    *Table,
  IN SMBIOS_TABLE_STRING  Index
) {
  CHAR8 *Strings;
  UINTN  Size = 0;
  if ((Table == NULL) || (Index == 0)) {
    return NULL;
  }
  Strings = ((CHAR8 *)Table) + Table->Length;
  // Get the maximum length of searching just in case
  if (mSmBiosTable != NULL) {
    UINT32 Offset = (UINT32)Table;
    if ((Offset >= mSmBiosTable->TableAddress) && (Offset < (mSmBiosTable->TableAddress + mSmBiosTable->TableLength))) {
      Size = (UINTN)mSmBiosTable->TableLength - (UINTN)(Offset - mSmBiosTable->TableAddress);
    } else {
      Size = 0;
    }
  } else if (mSmBios3Table != NULL) {
    UINT64 Offset = (UINT64)Table;
    if ((Offset >= mSmBiosTable->TableAddress) && (Offset < (mSmBiosTable->TableAddress + mSmBiosTable->TableLength))) {
      Size = (UINTN)mSmBios3Table->TableMaximumSize - (UINTN)(Offset - mSmBios3Table->TableAddress);
    } else {
      Size = 0;
    }
  }
  if (Size != 0) {
    UINTN               TableSize = 0;
    SMBIOS_TABLE_STRING Count = 0;
    // Find the string section after this structure
    Strings = (CHAR8 *)Table;
    Strings += Table->Length;
    do {
      if ((Index == ++Count) && (*Strings != '\0')) {
        return Strings;
      }
      if (Count >= Index) {
        break;
      }
      do {
        TableSize = Strings - (CHAR8 *)Table;
      } while ((TableSize < Size) && (*Strings++ != '\0'));
    } while ((TableSize < Size) && (*Strings != '\0'));
  }
  return NULL;
}

// GetSmBiosManufacturer
/// Get the system manufacturer
/// @return The system manufacturer retrieved from SMBIOS or NULL if not found or there was an error
CHAR8 *
EFIAPI
GetSmBiosManufacturer (
  VOID
) {
  CHAR8 *Manufacturer = NULL;
  // Get the system table
  SMBIOS_STRUCTURE *Table = FindSmBiosTable(NULL, 1);
  if (Table != NULL) {
    SMBIOS_TABLE_TYPE1 *Type1 = (SMBIOS_TABLE_TYPE1 *)Table;
    // Get the system manufacturer name
    Manufacturer = GetStringFromSmBiosTable(Table, Type1->Manufacturer);
  }
  // Check the manufacturer is valid
  if ((Manufacturer == NULL) ||
      (AsciiStriCmp(Manufacturer, "To be filled by OEM") == 0) ||
      (AsciiStriCmp(Manufacturer, "To be filled by O.E.M.") == 0)) {
    Manufacturer = NULL;
    // Get the baseboard table
    Table = FindSmBiosTable(NULL, 2);
    if (Table != NULL) {
      SMBIOS_TABLE_TYPE2 *Type2 = (SMBIOS_TABLE_TYPE2 *)Table;
      // Get the system manufacturer name
      Manufacturer = GetStringFromSmBiosTable(Table, Type2->Manufacturer);
    }
  }
  return Manufacturer;
}
// GetSmBiosProductName
/// Get the system product name
/// @return The system product name retrieved from SMBIOS or NULL if not found or there was an error
CHAR8 *
EFIAPI
GetSmBiosProductName (
  VOID
) {
  CHAR8 *ProductName = NULL;
  // Get the system table
  SMBIOS_STRUCTURE *Table = FindSmBiosTable(NULL, 1);
  if (Table != NULL) {
    SMBIOS_TABLE_TYPE1 *Type1 = (SMBIOS_TABLE_TYPE1 *)Table;
    // Get the system ProductName name
    ProductName = GetStringFromSmBiosTable(Table, Type1->ProductName);
  }
  // Check the product name is valid
  if ((ProductName == NULL) ||
      (AsciiStriCmp(ProductName, "To be filled by OEM") == 0) ||
      (AsciiStriCmp(ProductName, "To be filled by O.E.M.") == 0)) {
    ProductName = NULL;
    // Get the baseboard table
    Table = FindSmBiosTable(NULL, 2);
    if (Table != NULL) {
      SMBIOS_TABLE_TYPE2 *Type2 = (SMBIOS_TABLE_TYPE2 *)Table;
      // Get the system ProductName name
      ProductName = GetStringFromSmBiosTable(Table, Type2->ProductName);
    }
  }
  return ProductName;
}
// GetSmBiosSystemName
/// Get the system name from system manufacturer and system product name
/// @return The system name retrieved from SMBIOS, which needs freed, or NULL if not found or there was an error
CHAR8 *
EFIAPI
GetSmBiosSystemName (
  VOID
) {
  // Get the system manufacturer name
  CHAR8 *Manufacturer = GetSmBiosManufacturer();
  // Get the system product name
  CHAR8 *ProductName = GetSmBiosProductName();
  // Create a product name string
  if ((Manufacturer != NULL) && (ProductName != NULL)) {
    // Create product name from system manufacturer and product name
    UINTN  Length1 = AsciiStrLen(Manufacturer);
    UINTN  Length2 = AsciiStrLen(ProductName);
    // Allocate the product name string
    CHAR8 *Result = (CHAR8 *)AllocateZeroPool((Length1 + Length2 + 2) * sizeof(CHAR8));
    if (Result != NULL) {
      // Copy the strings to the product name string
      AsciiStrCpyS(Result, Length1 + 1, Manufacturer);
      Result[Length1] = ' ';
      AsciiStrCpyS(Result + Length1 + 1, Length2 + 1, ProductName);
    }
    return Result;
  } else if (Manufacturer != NULL) {
    // Create product name from system manufacturer
    return AsciiStrDup(Manufacturer);
  } else if (ProductName != NULL) {
    // Create product name from system product name
    return AsciiStrDup(ProductName);
  }
  // Could not create product name
  return NULL;
}

// SmBiosLibInitialize
/// SMBIOS library initialize use
/// @return Whether the library initialized successfully or not
/// @retval EFI_UNSUPPORTED The CPU is unsupported or no boot services locate/install/uninstall protocol
/// @retval EFI_SUCCESS     The CPU successfully initialized
EFI_STATUS
EFIAPI
SmBiosLibInitialize (
  VOID
) {
  CHAR8 *SystemName = NULL;
  UINTN  Index;
  // Check parameters
  if ((gST == NULL) || (gST->NumberOfTableEntries == 0) || (gST->ConfigurationTable == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Iterate through the configuration tables looking for SMBIOS tables
  for (Index = 0; Index < gST->NumberOfTableEntries; ++Index) {
    if ((CompareGuid(&(gST->ConfigurationTable[Index].VendorGuid), &gEfiSmbios3TableGuid) == 0) ||
        (CompareGuid(&(gST->ConfigurationTable[Index].VendorGuid), &gEfiSmbiosTableGuid) == 0)) {
      if (gST->ConfigurationTable[Index].VendorTable != NULL) {
        if (AsciiStrnCmp((CHAR8 *)(gST->ConfigurationTable[Index].VendorTable), "_SM_", 4) == 0) {
          // SMBIOS table
          if (mSmBiosTable == NULL) {
            mSmBiosTable = (SMBIOS_TABLE_ENTRY_POINT *)(gST->ConfigurationTable[Index].VendorTable);
            if (mSmBios3Table != NULL) {
              break;
            }
          }
        } else if (AsciiStrnCmp((CHAR8 *)(gST->ConfigurationTable[Index].VendorTable), "_SM3_", 5) == 0) {
          // SMBIOS 3 table
          if (mSmBios3Table == NULL) {
            mSmBios3Table = (SMBIOS_TABLE_3_0_ENTRY_POINT *)(gST->ConfigurationTable[Index].VendorTable);
            if (mSmBiosTable != NULL) {
              break;
            }
          }
        }
      }
    }
  }
  // Check either table was found
  if ((mSmBios3Table == NULL) && (mSmBiosTable == NULL)) {
    return EFI_NOT_FOUND;
  }
  // Print firmware and system name
  Log2(L"Firmware:", L"%s %u.%u\n", gST->FirmwareVendor, ((gST->Hdr.Revision >> 16) & 0xFFFF), (gST->Hdr.Revision & 0xFFFF));
  SystemName = GetSmBiosSystemName();
  if (SystemName != NULL) {
    Log2(L"System name:", L"%a\n", SystemName);
    FreePool(SystemName);
  }
  return EFI_SUCCESS;
}

// SmbiosLibFinish
/// SMBIOS library finish use
/// @return Whether the library was finished successfully or not
/// @retval EFI_SUCCESS The SMBIOS library successfully finished
EFI_STATUS
EFIAPI
SmBiosLibFinish (
  VOID
) {
  // Set original tables to NULL
  mSmBiosTable = NULL;
  mSmBios3Table = NULL;
  return EFI_SUCCESS;
}

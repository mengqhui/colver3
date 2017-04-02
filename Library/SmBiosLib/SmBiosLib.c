//
/// @file Library/SmbiosLib/SmbiosLib.c
///
/// SMBIOS library
///

#include <Library/SmBiosLib.h>

#include <Library/ConfigLib.h>
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

// GetSmBiosVersion
/// Get the version of the SMBIOS
/// @return The SMBIOS version code
VERSIONCODE
EFIAPI
GetSmBiosVersion (
  VOID
) {
  if (mSmBios3Table != NULL) {
    return PACK_VERSIONCODE(mSmBios3Table->MajorVersion, mSmBios3Table->MinorVersion);
  } else if (mSmBiosTable != NULL) {
    return PACK_VERSIONCODE(mSmBiosTable->MajorVersion, mSmBiosTable->MinorVersion);
  }
  return VERSIONCODE_UNKNOWN;
}

// GetNextSmBiosTable
/// Get the next SMBIOS table
/// @param Previous The previous SMBIOS structure or NULL to get the first structure
/// @return The next SMBIOS structure or NULL if there are no more
SMBIOS_STRUCTURE_POINTER
EFIAPI
GetNextSmBiosTable (
  IN SMBIOS_STRUCTURE_POINTER Previous OPTIONAL
) {
  // Check parameters
  if (Previous.Hdr == NULL) {
    if (mSmBios3Table != NULL) {
      // If SMBIOS 3 present use that
      Previous.Hdr = (SMBIOS_STRUCTURE *)(UINTN)mSmBios3Table->TableAddress;
    } else if (mSmBiosTable != NULL) {
      // If SMBIOS present use that
      Previous.Hdr = (SMBIOS_STRUCTURE *)(UINTN)mSmBiosTable->TableAddress;
    }
  } else {
    UINTN  Size = 0;
    UINT8 *Ptr;
    // Get the maximum length of searching just in case
    if (mSmBiosTable != NULL) {
      UINT32 Offset = (UINT32)Previous.Hdr;
      if ((Offset >= mSmBiosTable->TableAddress) && (Offset < (mSmBiosTable->TableAddress + mSmBiosTable->TableLength))) {
        Size = (UINTN)mSmBiosTable->TableLength - (UINTN)(Offset - mSmBiosTable->TableAddress);
      } else {
        Size = 0;
      }
    } else if (mSmBios3Table != NULL) {
      UINT64 Offset = (UINT64)Previous.Hdr;
      if ((Offset >= mSmBios3Table->TableAddress) && (Offset < (mSmBios3Table->TableAddress + mSmBios3Table->TableMaximumSize))) {
        Size = (UINTN)mSmBios3Table->TableMaximumSize - (UINTN)(Offset - mSmBios3Table->TableAddress);
      } else {
        Size = 0;
      }
    }
    if ((Size == 0) || (Previous.Hdr->Length == 0)) {
      Previous.Hdr = NULL;
    } else {
      UINTN TableSize = 0;
      // Find the string section after this structure
      Ptr = (UINT8 *)Previous.Hdr;
      Ptr += Previous.Hdr->Length;
      do {
        do {
          TableSize = Ptr - (UINT8 *)Previous.Hdr;
        } while ((TableSize < Size) && (*Ptr++ != 0));
      } while ((TableSize < Size) && (*Ptr++ != 0));
      if (TableSize >= Size) {
        Previous.Hdr = NULL;
      } else {
        Previous.Hdr = (SMBIOS_STRUCTURE *)Ptr;
      }
    }
  }
  // Check if last entry
  if (Previous.Hdr != NULL) {
    if (Previous.Hdr->Type == 0x7F) {
      Previous.Hdr = NULL;
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
EFIAPI
GetSmBiosTableCount (
  IN UINT8 Type
) {
  UINTN                    Count = 0;
  SMBIOS_STRUCTURE_POINTER Previous = { NULL };
  // Count the tables
  do {
    Previous = FindSmBiosTable(Previous, Type);
    if (Previous.Hdr == NULL) {
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
SMBIOS_STRUCTURE_POINTER
EFIAPI
FindSmBiosTable (
  IN SMBIOS_STRUCTURE_POINTER Previous OPTIONAL,
  IN UINT8                    Type
) {
  // Iterate through tables until match or end
  do {
    // Get next table
    Previous = GetNextSmBiosTable(Previous);
    if (Previous.Hdr == NULL) {
      // Not found
      break;
    }
    // Check if type matches
  } while (Previous.Hdr->Type != Type);
  return Previous;
}
// FindSmBiosTableByHandle
/// Find an SMBIOS structure with the specified handle
/// @param Handle The handle of the SMBIOS table to find
/// @return The SMBIOS structure or NULL if a table with the given handle was not found
SMBIOS_STRUCTURE_POINTER
EFIAPI
FindSmBiosTableByHandle (
  IN UINT16 Handle
) {
  SMBIOS_STRUCTURE_POINTER Previous = { NULL };
  // Check parameters
  if (((GetSmBiosVersion() >= PACK_VERSIONCODE(2, 1)) && (Handle >= 0xFEFF)) || (Handle >= 0xFFFE)) {
    return Previous;
  }
  // Traverse all tables to find handle
  do {
    Previous = GetNextSmBiosTable(Previous);
    if (Previous.Hdr == NULL) {
      break;
    }
    // Check if handle matches
  } while (Previous.Hdr->Handle != Handle);
  // Return the handle if found
  return Previous;
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
EFIAPI
FindSmBiosTables (
  IN  UINT8                      Type,
  OUT UINTN                     *Count,
  OUT SMBIOS_STRUCTURE_POINTER **Tables
) {
  SMBIOS_STRUCTURE_POINTER *FoundTables = { NULL };
  SMBIOS_STRUCTURE_POINTER  Previous = { NULL };
  UINTN                     TableCount;
  UINTN                     Index;
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
  FoundTables = (SMBIOS_STRUCTURE_POINTER *)AllocateZeroPool(TableCount * sizeof(SMBIOS_STRUCTURE_POINTER));
  if (FoundTables == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Iterate through SMBIOS tables with specified type
  for (Index = 0; Index < TableCount; ++Index) {
    // Get next SMBIOS table with specified type
    Previous = FindSmBiosTable(Previous, Type);
    if (Previous.Hdr == NULL) {
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

// GetSmBiosTableSize
/// Get the size of an SMBIOS table and string section in bytes
/// @param Table The SMBIOS table
/// @return The size in bytes of the SMBIOS table and string section
UINTN
EFIAPI
GetSmBiosTableSize (
  IN SMBIOS_STRUCTURE_POINTER Table
) {
  CHAR8 *Start;
  CHAR8 *End;
  // Check parameters
  if (Table.Hdr == NULL) {
    return 0;
  }
  // Get the beginning of the structure
  End = Start = (CHAR8 *)Table.Hdr;
  // Get the beginning of the string section
  End += Table.Hdr->Length;
  // Find the end of the string section
  while (*End++ != L'\0') {
    while (*End++ != L'\0');
  }
  // Return the size of the entire table
  return (UINTN)(End - Start);
}

// GetStringFromSmBiosTable
/// Get a string from an SMBIOS table
/// @param Table The SMBIOS table
/// @param Index The string index
/// @return The string from the SMBIOS table or NULL if not found
CHAR8 *
EFIAPI
GetStringFromSmBiosTable (
  IN SMBIOS_STRUCTURE_POINTER Table,
  IN SMBIOS_TABLE_STRING      Index
) {
  CHAR8 *Strings;
  UINTN  Count = 0;
  if ((Table.Hdr == NULL) || (Index == 0)) {
    return NULL;
  }
  // Find the string section after this structure
  Strings = (CHAR8 *)Table.Hdr;
  Strings += Table.Hdr->Length;
  do {
    if ((Index == ++Count) && (*Strings != L'\0')) {
      return Strings;
    }
    // Skip the rest of this string
    while (*Strings++ != L'\0');
    // Check if end of strings
  } while (*Strings != L'\0');
  return NULL;
}

// mSocketStrings
/// The socket upgrade type strings
STATIC CHAR16 *mSocketStrings[] = {
  NULL,
  L"Other",
  L"Unknown",
  L"Daughter Board",
  L"ZIF Socket",
  L"Replaceable Piggy Back",
  L"None",
  L"LIF Socket",
  L"Slot 1",
  L"Slot 2",
  L"370-pin socket",
  L"Slot A",
  L"Slot M",
  L"Socket 423",
  L"Socket A (Socket 462)",
  L"Socket 478",
  L"Socket 754",
  L"Socket 940",
  L"Socket 939",
  L"Socket mPGA604",
  L"Socket LGA771",
  L"Socket LGA775",
  L"Socket S1",
  L"Socket AM2",
  L"Socket F (1207)",
  L"Socket LGA1366",
  L"Socket G34",
  L"Socket AM3",
  L"Socket C32",
  L"Socket LGA1156",
  L"Socket LGA1567",
  L"Socket PGA988A",
  L"Socket BGA1288",
  L"Socket rPGA988B",
  L"Socket BGA1023",
  L"Socket BGA1224",
  L"Socket LGA1155",
  L"Socket LGA1356",
  L"Socket LGA2011",
  L"Socket FS1",
  L"Socket FS2",
  L"Socket FM1",
  L"Socket FM2",
  L"Socket LGA2011-3",
  L"Socket LGA1356-3",
  L"Socket LGA1150",
  L"Socket BGA1168",
  L"Socket BGA1234",
  L"Socket BGA1364",
  L"Socket AM4",
  L"Socket LGA1151",
  L"Socket BGA1356",
  L"Socket BGA1440",
  L"Socket BGA1515",
  L"Socket LGA3647-1",
  L"Socket SP3",
  L"Socket SP3r2"
};

// GetSmBiosProcessorUpgrade
/// Get an SMBIOS socket upgrade type string (Type4->ProcessorUpgrade)
/// @param ProcessorUpgrade The processor socket upgrade type
/// @return The socket type string or NULL if there was an error
CHAR16 *
EFIAPI
GetSmBiosProcessorUpgrade (
  UINT8 ProcessorUpgrade
) {
  // Check parameters
  if (ProcessorUpgrade >= ARRAY_SIZE(mSocketStrings)) {
    return NULL;
  }
  return mSocketStrings[ProcessorUpgrade];
}

// SetSmBiosManufacturer
/// Set the system manufacturer
STATIC VOID
EFIAPI
SetSmBiosManufacturer (
  VOID
) {
  CHAR8                    *Manufacturer = NULL;
  SMBIOS_STRUCTURE_POINTER  Table = { NULL };
  // Get the system table
  Table = FindSmBiosTable(Table, 1);
  if (Table.Hdr != NULL) {
    // Get the system manufacturer name
    Manufacturer = GetStringFromSmBiosTable(Table, Table.Type1->Manufacturer);
  }
  // Check the manufacturer is valid
  if ((Manufacturer == NULL) ||
      (AsciiStriCmp(Manufacturer, "To be filled by OEM") == 0) ||
      (AsciiStriCmp(Manufacturer, "To be filled by O.E.M.") == 0)) {
    Manufacturer = NULL;
    // Get the baseboard table
    Table.Hdr = NULL;
    Table = FindSmBiosTable(Table, 2);
    if (Table.Hdr != NULL) {
      // Get the system manufacturer name
      Manufacturer = GetStringFromSmBiosTable(Table, Table.Type2->Manufacturer);
    }
  }
  if (Manufacturer != NULL) {
    CHAR16 *String = FromAscii(Manufacturer);
    if (String != NULL) {
      ConfigSetString(L"\\System\\Manufacturer", String, FALSE);
      FreePool(String);
    }
  }
}
// SetSmBiosProductName
/// Set the system product name
STATIC VOID
EFIAPI
SetSmBiosProductName (
  VOID
) {
  CHAR8                    *ProductName = NULL;
  SMBIOS_STRUCTURE_POINTER  Table = { NULL };
  // Get the system table
  Table = FindSmBiosTable(Table, 1);
  if (Table.Hdr != NULL) {
    // Get the system ProductName name
    ProductName = GetStringFromSmBiosTable(Table, Table.Type1->ProductName);
  }
  // Check the product name is valid
  if ((ProductName == NULL) ||
      (AsciiStriCmp(ProductName, "To be filled by OEM") == 0) ||
      (AsciiStriCmp(ProductName, "To be filled by O.E.M.") == 0)) {
    ProductName = NULL;
    // Get the baseboard table
    Table.Hdr = NULL;
    Table = FindSmBiosTable(Table, 2);
    if (Table.Hdr != NULL) {
      // Get the system ProductName name
      ProductName = GetStringFromSmBiosTable(Table, Table.Type2->ProductName);
    }
  }
  if (ProductName != NULL) {
    CHAR16 *String = FromAscii(ProductName);
    if (String != NULL) {
      ConfigSetString(L"\\System\\ProductName", String, FALSE);
      FreePool(String);
    }
  }
}
// SetSmBiosSystemName
/// Set the system name from system manufacturer and system product name
STATIC VOID
EFIAPI
SetSmBiosSystemName (
  VOID
) {
  // Get the system manufacturer name
  CHAR16 *Manufacturer = ConfigGetStringWithDefault(L"\\System\\Manufacturer", NULL);
  // Get the system product name
  CHAR16 *ProductName = ConfigGetStringWithDefault(L"\\System\\ProductName", NULL);
  // Create a product name string
  if ((Manufacturer != NULL) && (ProductName != NULL)) {
    // Create product name from system manufacturer and product name
    CHAR16 *SystemName = CatSPrint(NULL, L"%s %s", Manufacturer, ProductName);
    if (SystemName != NULL) {
      ConfigSetString(L"\\System\\Name", SystemName, FALSE);
      FreePool(SystemName);
    }
  } else if (Manufacturer != NULL) {
    // Create product name from system manufacturer
    ConfigSetString(L"\\System\\Name", Manufacturer, FALSE);
  } else if (ProductName != NULL) {
    // Create product name from system product name
    ConfigSetString(L"\\System\\Name", ProductName, FALSE);
  }
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
  UINTN Index;
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
  // Set manufacturer
  SetSmBiosManufacturer();
  // Set system name
  SetSmBiosProductName();
  // Set system name
  SetSmBiosSystemName();
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

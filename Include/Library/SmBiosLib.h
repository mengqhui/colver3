//
/// @file Include/Library/SmbiosLib.h
///
/// SMBIOS library
///

#pragma once
#ifndef __SMBIOS_LIBRARY_HEADER__
#define __SMBIOS_LIBRARY_HEADER__

#include <Library/StringLib.h>

#include <IndustryStandard/SmBios.h>

// GetNextSmBiosTable
/// Get the next SMBIOS table
/// @param Previous The previous SMBIOS structure or NULL to get the first structure
/// @return The next SMBIOS structure or NULL if there are no more
SMBIOS_STRUCTURE *
EFIAPI
GetNextSmBiosTable (
  IN SMBIOS_STRUCTURE *Previous OPTIONAL
);
// GetSmBiosTableCount
/// Get the count of SMBIOS tables with a specified type
/// @param Type The type of SMBIOS table to count
/// @return The count of SMBIOS tables
UINTN
GetSmBiosTableCount (
  IN UINT8 Type
);
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
);
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
);

// GetStringFromSmBiosTable
/// Get a string from an SMBIOS table
/// @param Table The SMBIOS table
/// @param Index The string index
/// @return The string from the SMBIOS table or NULL if not found
CHAR8 *
EFIAPI
GetStringFromSmBiosTable (
  IN SMBIOS_STRUCTURE *Table,
  IN UINT8             Index
);

// GetSmBiosManufacturer
/// Get the system manufacturer
/// @return The system manufacturer retrieved from SMBIOS or NULL if not found or there was an error
CHAR8 *
EFIAPI
GetSmBiosManufacturer (
  VOID
);
// GetSmBiosProductName
/// Get the system product name
/// @return The system product name retrieved from SMBIOS or NULL if not found or there was an error
CHAR8 *
EFIAPI
GetSmBiosProductName (
  VOID
);
// GetSmBiosSystemName
/// Get the system name from system manufacturer and system product name
/// @return The system name retrieved from SMBIOS, which needs freed, or NULL if not found or there was an error
CHAR8 *
EFIAPI
GetSmBiosSystemName (
  VOID
);

#endif // __SMBIOS_LIBRARY_HEADER__

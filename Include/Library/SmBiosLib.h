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

// GetSmBiosVersion
/// Get the version of the SMBIOS
/// @return The SMBIOS version code
VERSIONCODE
EFIAPI
GetSmBiosVersion (
  VOID
);

// GetNextSmBiosTable
/// Get the next SMBIOS table
/// @param Previous The previous SMBIOS structure or NULL to get the first structure
/// @return The next SMBIOS structure or NULL if there are no more
SMBIOS_STRUCTURE_POINTER
EFIAPI
GetNextSmBiosTable (
  IN SMBIOS_STRUCTURE_POINTER Previous OPTIONAL
);
// GetSmBiosTableCount
/// Get the count of SMBIOS tables with a specified type
/// @param Type The type of SMBIOS table to count
/// @return The count of SMBIOS tables
UINTN
EFIAPI
GetSmBiosTableCount (
  IN UINT8 Type
);
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
);
// FindSmBiosTableByHandle
/// Find an SMBIOS structure with the specified handle
/// @param Handle The handle of the SMBIOS table to find
/// @return The SMBIOS structure or NULL if a table with the given handle was not found
SMBIOS_STRUCTURE_POINTER
EFIAPI
FindSmBiosTableByHandle (
  IN UINT16 Handle
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
EFIAPI
FindSmBiosTables (
  IN  UINT8                      Type,
  OUT UINTN                     *Count,
  OUT SMBIOS_STRUCTURE_POINTER **Tables
);

// GetSmBiosTableSize
/// Get the size of an SMBIOS table and string section in bytes
/// @param Table The SMBIOS table
/// @return The size in bytes of the SMBIOS table and string section
UINTN
EFIAPI
GetSmBiosTableSize (
  IN SMBIOS_STRUCTURE_POINTER Table
);

// GetStringFromSmBiosTable
/// Get a string from an SMBIOS table
/// @param Table The SMBIOS table
/// @param Index The string index
/// @return The string from the SMBIOS table or NULL if not found
CHAR8 *
EFIAPI
GetStringFromSmBiosTable (
  IN SMBIOS_STRUCTURE_POINTER Table,
  IN UINT8                    Index
);

// GetSmBiosProcessorUpgrade
/// Get an SMBIOS socket upgrade type string (Type4->ProcessorUpgrade)
/// @param ProcessorUpgrade The processor socket upgrade type
/// @return The socket type string or NULL if there was an error
CHAR16 *
EFIAPI
GetSmBiosProcessorUpgrade (
  UINT8 ProcessorUpgrade
);

#endif // __SMBIOS_LIBRARY_HEADER__

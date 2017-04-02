//
/// @file Include/Library/GUILib.h
///
/// GUI library
///

#pragma once
#ifndef __GUI_LIBRARY_HEADER__
#define __GUI_LIBRARY_HEADER__

#include <Library/ConfigLib.h>
#include <Library/PlatformLib.h>

// GetKeyPress
/// Get a key press
/// @param Key On output, the pressed key information
/// @return Whether the key press information is valid or not
/// @retval EFI_INVALID_PARAMETER If Key is NULL
/// @retval EFI_NOT_READY         If there was no key press
/// @retval EFI_SUCCESS           If the key press information is valid
EFI_STATUS
EFIAPI
GetKeyPress (
  IN OUT EFI_INPUT_KEY *Key
);
// WaitForKeyPress
/// Wait for a key press
/// @param Key On output, the pressed key information
/// @return Whether the key press information is valid or not
/// @retval EFI_SUCCESS If the key press information is valid
EFI_STATUS
EFIAPI
WaitForKeyPress (
  IN OUT EFI_INPUT_KEY *Key OPTIONAL
);
// WaitForAnyKeyPress
/// Wait for any key press
/// @return Whether a key press occurred or not
/// @retval EFI_SUCCESS If a key press occurred
EFI_STATUS
EFIAPI
WaitForAnyKeyPress (
  VOID
);

// GUIRun
/// Execute the GUI
EFI_STATUS
EFIAPI
GUIRun (
  VOID
);

#endif // __GUI_LIBRARY_HEADER__

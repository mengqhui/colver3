///
/// @file Library/GUILib/GUILib.c
///
/// GUI library entry
///

#include <Library/GUILib.h>

// GUILibInitialize
/// GUI library initialize use
/// @return Whether the GUI initialized successfully or not
/// @retval EFI_SUCCESS The GUI successfully initialized
EFI_STATUS
EFIAPI
GUILibInitialize (
  VOID
) {
  return EFI_SUCCESS;
}

// GUILibFinish
/// GUI library finish use
/// @return Whether the GUI was finished successfully or not
/// @retval EFI_SUCCESS The GUI successfully finished
EFI_STATUS
EFIAPI
GUILibFinish (
  VOID
) {
  return EFI_SUCCESS;
}

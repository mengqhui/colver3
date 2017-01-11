///
/// @file Application/GUI/GUI.c
///
/// GUI entry
///

#include <Uefi.h>

// GUIMain
/// GUI entry point
/// @param ImageHandle The assigned image handle for this application
/// @param SystemTable The EFI system table
/// @return Whether the GUI initialized successfully or not
/// @retval EFI_SUCCESS The GUI successfully exited (upon user request)
EFI_STATUS
EFIAPI
GUIMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
) {
  return EFI_SUCCESS;
}

// GUIUnload
/// GUI image unload
/// @param ImageHandle The assigned image handle for this application
/// @return Whether the GUI was unloaded successfully or not
/// @retval EFI_SUCCESS The GUI successfully unloaded
EFI_STATUS
EFIAPI
GUIUnload (
  IN EFI_HANDLE ImageHandle
) {
  return EFI_SUCCESS;
}

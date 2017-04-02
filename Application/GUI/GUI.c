///
/// @file Application/GUI/GUI.c
///
/// GUI entry
///

#include <Library/GUILib.h>

#include <Library/SmBiosLib.h>
#include <Library/TimerLib.h>

#include <Library/UefiBootServicesTableLib.h>

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

#include <Protocol/LegacyBios.h>

#endif

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
  EFI_STATUS       Status;
  CHAR16          *Value;
  CHAR8           *Language;
  CHAR8          **Languages;
  UINTN            Count;
  UINTN            Index;

  // Clear the watchdog timer
  if ((gBS != NULL) && (gBS->SetWatchdogTimer != NULL)) {
    gBS->SetWatchdogTimer(0, 0, 0, NULL);
  }

  // Print languages
  Count = 0;
  Languages = NULL;
  if (!EFI_ERROR(GetLanguages(&Languages, &Count)) && (Languages != NULL) && (Count > 0)) {
    Log2(L"Languages:", NULL);
    for (Index = 0; Index < Count; ++Index) {
      Log(L"%a;", Languages[Index]);
    }
    Log(L"\n");
    AsciiStrListFree(Languages, Count);
  } else {
    Log2(L"Languages:", L"None\n");
  }
  // Get language from configuration
  Value = ConfigGetStringWithDefault(L"\\Language", NULL);
  if (Value != NULL) {
    // Allocate string
    Count = StrLen(Value) + 1;
    Language = (CHAR8 *)AllocateZeroPool(Count * sizeof(CHAR8));
    if (Language != NULL) {
      // Convert string
      Status = UnicodeStrToAsciiStrS(Value, Language, Count);
      if (!EFI_ERROR(Status)) {
        // Set language
        Status = SetLanguage(Language);
        Log2(L"Set language:", L"%a\n", Language);
      }
      FreePool(Language);
    }
  }
  // Print language
  Language = NULL;
  Status = GetLanguage(&Language);
  Log2(L"Language:", L"%a\n", (EFI_ERROR(Status) || (Language == NULL)) ? "None" : Language);

#if defined(MDE_CPU_IA32) || defined(MDE_CPU_X64) || defined(MDE_CPU_IPF)

  // Check if legacy CSM is available
  if ((gBS != NULL) && (gBS->LocateProtocol != NULL)) {
    VOID *Tmp = NULL;
    Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID **)&Tmp);
    if (!EFI_ERROR(Status) && (Tmp != NULL)) {
      ConfigSetBoolean(L"\\System\\Legacy", TRUE, TRUE);
    }
  }
  Log2(L"Legacy support:", L"%s\n", ConfigGetBooleanWithDefault(L"\\System\\Legacy", FALSE) ? L"true" : L"false");

#endif

  // Run GUI
  Status = GUIRun();
  Log2(L"GUI status:", L"%r\n", Status);

  // Print quit
  Log2(L"Runtime:", NULL);
  LogTimestamp();
  Log(L"\n");
  Log2(L"Quit:", NULL);
  LogDateAndTime();
  Log(L"\n\n");

  // Wait for key press
  WaitForAnyKeyPress();

  return Status;
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

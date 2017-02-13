///
/// @file Library/GUILib/GUILib.c
///
/// GUI library entry
///

#include <Library/GUILib.h>

#include <Library/UefiBootServicesTableLib.h>

// GUI_GRAPHICS
/// Graphics engine output
typedef struct _GUI_GRAPHICS GUI_GRAPHICS;
struct _GUI_GRAPHICS {

  // Next
  /// The next graphics output protocol
  GUI_GRAPHICS                 *Next;
  // Device
  /// The graphics device handle
  EFI_HANDLE                    Device;
  // DevicePath
  /// The graphics device path
  EFI_DEVICE_PATH              *DevicePath;
  // Path
  /// The graphics device path string
  CHAR16                       *Path;
  // Output
  /// The graphics output protocol
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Output;

};

// mGraphics
/// The graphics engine outputs
STATIC GUI_GRAPHICS    *mGraphics = NULL;
// mConOutOutputString
STATIC EFI_TEXT_STRING  mConOutOutputString = NULL;

// GUIGraphicsOutputString
/// 
STATIC EFI_STATUS
EFIAPI
GUIGraphicsOutputString (
  IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This,
  IN CHAR16                          *String
) {
  return EFI_SUCCESS;
}
// GUIGraphicsInitialize
/// Initialize the graphics engine
STATIC EFI_STATUS
EFIAPI
GUIGraphicsInitialize (
  VOID
) {
  EFI_GRAPHICS_OUTPUT_PROTOCOL *Output;
  EFI_DEVICE_PATH              *DevicePath;
  EFI_STATUS                    Status;
  EFI_HANDLE                   *Handles = NULL;
  CHAR16                       *Path;
  UINTN                         Count = 0;
  UINTN                         Index;

  // Check boot services
  if ((gBS == NULL) || (gBS->HandleProtocol == NULL) || (gBS->LocateHandleBuffer == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Replace console output
  if ((gST != NULL) && (gST->ConOut != NULL) && (gST->ConOut->OutputString != NULL)) {
    mConOutOutputString = gST->ConOut->OutputString;
    gST->ConOut->OutputString = GUIGraphicsOutputString;
  }
  // Locate all graphics output protocols
  Status = gBS->LocateHandleBuffer(ByProtocol, &gEfiGraphicsOutputProtocolGuid, NULL, &Count, &Handles);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Handles == NULL) {
    return EFI_NOT_FOUND;
  }
  if (Count == 0) {
    FreePool(Handles);
    return EFI_NOT_FOUND;
  }
  // Iterate through the handles to get each graphics output protocol
  for (Index = 0; Index < Count; ++Index) {
    UINT32 Best = 0;
    UINT32 BestWidth = 0;
    UINT32 BestHeight = 0;
    UINT32 Mode;

    // Get the graphics protocol from each handle
    Status = gBS->HandleProtocol(Handles[Index], &gEfiGraphicsOutputProtocolGuid, (VOID **)&Output);
    if (EFI_ERROR(Status) || (Output == NULL) || (Output->Mode == NULL)) {
      continue;
    }
    // Get device path
    DevicePath = DevicePathFromHandle(Handles[Index]);
    if (DevicePath == NULL) {
      continue;
    }
    Path = ConvertDevicePathToText(DevicePath, FALSE, FALSE);
    if (Path == NULL) {
      continue;
    }
    Log2(L"Graphics:", L"%s\n", Path);
    // Determine best display mode
    for (Mode = 0; Mode <= (UINT32)Output->Mode->MaxMode; ++Mode) {
      EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info = NULL;
      UINTN                                 Size = 0;
      // Query each graphics mode
      Status = Output->QueryMode(Output, Mode, &Size, &Info);
      if (EFI_ERROR(Status) || (Info == NULL)) {
        continue;
      }
      // Check to make sure this is valid mode information
      if (Size >= sizeof(EFI_GRAPHICS_OUTPUT_MODE_INFORMATION)) {
        // Check pixel format for direct access to framebuffer
        switch (Info->PixelFormat) {
          case PixelBlueGreenRedReserved8BitPerColor:
          case PixelRedGreenBlueReserved8BitPerColor:
            LOG2(L"  Mode:", L"%u (%u x %u)\n",  Mode, Info->HorizontalResolution, Info->VerticalResolution);
            // Direct access to draw in framebuffer
            if (((Info->HorizontalResolution > BestWidth) && (Info->VerticalResolution > BestHeight)) ||
                ((Info->HorizontalResolution * Info->VerticalResolution) > (BestWidth * BestHeight))) {
              Best = Mode;
              BestWidth = Info->HorizontalResolution;
              BestHeight = Info->VerticalResolution;
            }
            break;

          default:
            // Other formats not suitable for our use
            break;

        }
      }
      FreePool(Info);
    }
    // Set best mode
    if ((BestWidth != 0) && (BestHeight != 0)) {
      GUI_GRAPHICS *Graphics;

      Log2(L"  Best mode:", L"%u (%u x %u)\n", Best, BestWidth, BestHeight);
      if (Best != (UINT32)Output->Mode->Mode) {
        Log(L"    Change to best mode: %r\n", EFI_SUCCESS);//Output->SetMode(Output, Best));
      }
      // Add this graphics output to the list
      Graphics = (GUI_GRAPHICS *)AllocateZeroPool(sizeof(GUI_GRAPHICS));
      if (Graphics == NULL) {
        FreePool(Path);
        return EFI_OUT_OF_RESOURCES;
      }
      Graphics->Next = NULL;
      Graphics->Device = Handles[Index];
      Graphics->DevicePath = DuplicateDevicePath(DevicePath);
      Graphics->Output = Output;
      Graphics->Path = Path;
      if (mGraphics == NULL) {
        mGraphics = Graphics;
      } else {
        GUI_GRAPHICS *Ptr = mGraphics;
        while (Ptr->Next != NULL) {
          Ptr = Ptr->Next;
        }
        Ptr->Next = Graphics;
      }
    } else {
      FreePool(Path);
    }
  }
  return EFI_SUCCESS;
}
// GUIGraphicsFinish
/// Finish graphics engine
STATIC EFI_STATUS
EFIAPI
GUIGraphicsFinish (
  VOID
) {
  // Restore old console
  if (mConOutOutputString != NULL) {
    gST->ConOut->OutputString = mConOutOutputString;
    mConOutOutputString = NULL;
  }
  // Remove graphics outputs
  while (mGraphics != NULL) {
    GUI_GRAPHICS *Graphics = mGraphics;
    mGraphics = Graphics->Next;
    Graphics->Next = NULL;
    Graphics->Device = NULL;
    Graphics->Output = NULL;
    if (Graphics->DevicePath != NULL) {
      FreePool(Graphics->DevicePath);
      Graphics->DevicePath = NULL;
    }
    if (Graphics->Path != NULL) {
      FreePool(Graphics->Path);
      Graphics->Path = NULL;
    }
    FreePool(Graphics);
  }
  return EFI_SUCCESS;
}

// GUIRun
/// Execute the GUI
EFI_STATUS
EFIAPI
GUIRun (
  VOID
) {
  EFI_STATUS Status;

  // Initialize the GUI graphics engine
  Status = GUIGraphicsInitialize();

  // Finish the GUI graphics engine
  GUIGraphicsFinish();

  return Status;
}

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

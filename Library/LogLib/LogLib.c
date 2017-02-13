///
/// @file Library/LogLib/LogLib.c
///
/// Log library entry
///

#include <Library/LogLib.h>

#include <Library/SmBiosLib.h>

#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/TimerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

// LOG_CHUNK_SIZE
/// The size of a log chunk - 32KB
#define LOG_CHUNK_SIZE (0x8000 - OFFSET_OF(LOG_CHUNK, Text))
// LOG_DEFAULT_FILE
/// The default log file
#define LOG_DEFAULT_FILE PROJECT_ROOT_PATH L"\\" PROJECT_SAFE_NAME L"\\" PROJECT_SAFE_ARCH L"\\" PROJECT_SAFE_NAME L".log"

// LOG_CHUNK
/// A chunk of the log
typedef struct _LOG_CHUNK LOG_CHUNK;
struct _LOG_CHUNK {

  // Next
  /// The next chunk
  LOG_CHUNK *Next;
  // Size
  /// The in use size of the chunk
  UINTN      Size;
  // Text
  /// The chunk text
  CHAR16     Text[0];

};

// LOG_VPRINT
/// Print to the log with variable arguments list
/// @param Outputs The log output methods to use for printing
/// @param Path    The log file path for use file output method
/// @param Format  The format specifier for formatting the text
/// @param Args    The variable arguments list for formatting
/// @return The count of characters printed to the log
typedef UINTN
(EFIAPI
*LOG_VPRINT) (
  IN OUT UINT32  *Outputs,
  IN     CHAR16  *Path OPTIONAL,
  IN     CHAR16  *Format,
  IN     VA_LIST  Args
);
// LOG_GET_START
/// Get the performance counter start
/// @return The performance counter start
typedef UINT64
(EFIAPI
*LOG_GET_START) (
  VOID
);
// LOG_SAVE
/// Save log contents to file
/// @param Path The log file path
/// @return Whether the log file was saved or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_NOT_FOUND         The log file path could not be found
/// @retval EFI_SUCCESS           The log file was saved
typedef EFI_STATUS
(EFIAPI
*LOG_SAVE) (
  IN CHAR16 *Path
);

// LOG_PROTOCOL
/// Log protocol
typedef struct _LOG_PROTOCOL LOG_PROTOCOL;
struct _LOG_PROTOCOL {

  // VPrint
  /// Print to the log with variable arguments list
  LOG_VPRINT    VPrint;
  // GetStart
  /// Get the performance counter start
  LOG_GET_START GetStart;
  // Save
  /// Save the log to file
  LOG_SAVE      Save;

};

// mLogOutputs
/// Enabled log outputs
STATIC UINT32        mLogOutputs = LOG_OUTPUT_ALL;
// mLogPath
/// Log file path
STATIC CHAR16       *mLogPath = NULL;
// mLogCounterStart
/// Log performance counter start
STATIC UINT64        mLogCounterStart = 0;
// mLogChunks
/// Log contents chunks
STATIC LOG_CHUNK    *mLogChunks = NULL;
// mLogChunk
/// Log current contents chunk
STATIC LOG_CHUNK    *mLogChunk = NULL;
// mLogHandle
/// Log protocol handle
STATIC EFI_HANDLE    mLogHandle = NULL;
// mLog
/// Installed log protocol
STATIC LOG_PROTOCOL *mLog = NULL;
// mLogGuid
/// Log protocol GUID
STATIC EFI_GUID      mLogGuid = { 0x2E838A34, 0xDDA6, 0x4D65, { 0x8B, 0xB4, 0x04, 0xD4, 0x92, 0xA6, 0xAF, 0xF3 } };

// Log
/// Log formatted text to log and/or console
/// @param Format The format specifier for formatting the text
/// @param ...    The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
Log (
  CHAR16 *Format,
  ...
) {
  VA_LIST Args;
  UINTN   Count;
  VA_START(Args, Format);
  Count = VLog(Format, Args);
  VA_END(Args);
  return Count;
}
// Log2
/// Log formatted text to log and/or console
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param ...    The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
Log2 (
  CHAR16 *Prefix,
  CHAR16 *Format,
  ...
) {
  VA_LIST Args;
  UINTN   Count;
  VA_START(Args, Format);
  Count = VLog2(Prefix, Format, Args);
  VA_END(Args);
  return Count;
}
// Log3
/// Log formatted text to log and/or console
/// @param Width  The alignment width
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param ...    The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
Log3 (
  UINTN   Width,
  CHAR16 *Prefix,
  CHAR16 *Format,
  ...
) {
  VA_LIST Args;
  UINTN   Count;
  VA_START(Args, Format);
  Count = VLog3(Width, Prefix, Format, Args);
  VA_END(Args);
  return Count;
}
// LogVPrint
/// Print to the log with variable arguments list
/// @param Outputs The log output methods to use for printing
/// @param Path    The log file path for use file output method
/// @param Format  The format specifier for formatting the text
/// @param Args    The variable arguments list for formatting
/// @return The count of characters printed to the log
STATIC UINTN
EFIAPI
LogVPrint (
  IN OUT UINT32  *Outputs,
  IN     CHAR16  *Path OPTIONAL,
  IN     CHAR16  *Format,
  IN     VA_LIST  Args
) {
  UINTN Count = 0;
  // Get size needed for log output
  UINTN Size = SPrintLength(Format, Args);
  if ((Size++ > 0) && (Size <= (LOG_CHUNK_SIZE / (4 * sizeof(CHAR16))))) {
    Size *= sizeof(CHAR16);
    // Create a new chunk if needed
    if (mLogChunk == NULL) {
      if (mLogChunks != NULL) {
        // Get the last chunk
        for (mLogChunk = mLogChunks; mLogChunk->Next != NULL; mLogChunk = mLogChunk->Next);
      } else {
        // Create a new root chunk
        mLogChunk = mLogChunks = (LOG_CHUNK *)AllocatePages(EFI_SIZE_TO_PAGES(LOG_CHUNK_SIZE));
        if (mLogChunk == NULL) {
          return 0;
        }
        mLogChunk->Next = NULL;
        mLogChunk->Size = 0;
      }
    }
    // Check to make sure there is a valid chunk
    if ((mLogChunk->Size + Size + sizeof(CHAR16)) >= LOG_CHUNK_SIZE) {
      mLogChunk->Next = (LOG_CHUNK *)AllocatePages(EFI_SIZE_TO_PAGES(LOG_CHUNK_SIZE));
      if (mLogChunk->Next == NULL) {
        return 0;
      }
      mLogChunk = mLogChunk->Next;
      mLogChunk->Next = NULL;
      mLogChunk->Size = 0;
    }
    // Create formatted text
    Count = UnicodeVSPrint(mLogChunk->Text + mLogChunk->Size, LOG_CHUNK_SIZE - mLogChunk->Size, Format, Args);
    if (Count > 0) {
      // Print to log file
      if (((*Outputs) & LOG_OUTPUT_FILE) != 0) {
        // Can only output if there is valid path
        if (Path == NULL) {
          Path = mLogPath;
        }
        if (Path != NULL) {
          EFI_STATUS Status;
          // Append to log file
          Size = Count * sizeof(CHAR16);
          Status = FileHandleAppendByPath(NULL, Path, &Size, mLogChunk->Text + mLogChunk->Size);
          if (EFI_ERROR(Status)) {
            (*Outputs) &= ~LOG_OUTPUT_FILE;
          }
        } else {
          (*Outputs) &= ~LOG_OUTPUT_FILE;
        }
      }
      // Print to console
      if (((*Outputs) & LOG_OUTPUT_CONSOLE) != 0) {
        // Write to console
        if (Count >= 256) {
          CHAR16 Tmp[256];
          // We may need to break the print string down to be 256 characters since some firmware freak out about larger
          for (Size = 0; Size < Count; Size += 255) {
            StrnCpyS(Tmp, 256, mLogChunk->Text + mLogChunk->Size + Size, 255);
            Print(L"%s", Tmp);
          }
        } else {
          Print(L"%s", mLogChunk->Text + mLogChunk->Size);
        }
      }
      // Advance the log
      mLogChunk->Size += (Count * sizeof(CHAR16));
    }
  }
  return Count;
}
// VLog
/// Log formatted text to log and/or console
/// @param Format The format specifier for formatting the text
/// @param Args   The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
VLog (
  CHAR16  *Format,
  VA_LIST  Args
) {
  if ((mLog != NULL) && (mLog->VPrint != NULL)) {
    return mLog->VPrint(&mLogOutputs, mLogPath, Format, Args);
  }
  return LogVPrint(&mLogOutputs, mLogPath, Format, Args);
}
// VLog2
/// Log formatted text to log and/or console
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param Args   The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
VLog2 (
  CHAR16  *Prefix,
  CHAR16  *Format,
  VA_LIST  Args
) {
  return VLog3(LOG_PREFIX_WIDTH, Prefix, Format, Args);
}
// VLog3
/// Log formatted text to log and/or console
/// @param Width  The alignment width
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param Args   The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
VLog3 (
  UINTN    Width,
  CHAR16  *Prefix,
  CHAR16  *Format,
  VA_LIST  Args
) {
  UINTN Count = 0;
  if (Prefix != NULL) {
    Count += Log(L"%- *s ", Width, Prefix);
  }
  if (Format != NULL) {
    Count += VLog(Format, Args);
  }
  return Count;
}

// SetLogOutput
/// Set log output methods
/// @return The log output methods that are enabled
UINT32
EFIAPI
GetLogOutput (
  VOID
) {
  return mLogOutputs;
}
// SetLogOutput
/// Set log output methods
/// @param Outputs The log output methods to enable
VOID
EFIAPI
SetLogOutput (
  IN UINT32 Outputs
) {
  mLogOutputs = (Outputs & LOG_OUTPUT_ALL);
}

// GetLogPath
/// Get log file path for output
/// @param Path On output, the path to the log file which needs freed
/// @return Whether the log file path was retrieved or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  Could not allocate memory
/// @retval EFI_SUCCESS           Log file path allocated and returned
EFI_STATUS
EFIAPI
GetLogPath (
  OUT CHAR16 **Path
) {
  CHAR16 *Str;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Check log file path is valid
  if (mLogPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Duplicate log path
  Str = StrDup(mLogPath);
  if (Str == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Return log path
  *Path = Str;
  return EFI_SUCCESS;
}
// SetLogPath
/// Set log file path for output
/// @param Path The log file path
/// @param Save Whether to save the contents of the log or start empty
/// @return Whether the log file was opened or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_NOT_FOUND         The log file path could not be found
/// @retval EFI_SUCCESS           The log file path was opened (and contents saved)
EFI_STATUS
EFIAPI
SetLogPath (
  IN CHAR16  *Path,
  IN BOOLEAN  Save
) {
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Save log
  if (Save) {
    EFI_STATUS Status = SaveLog(Path);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
  // Set file name
  if (mLogPath != NULL) {
    FreePool(mLogPath);
  }
  mLogPath = StrDup(Path);
  mLogOutputs |= LOG_OUTPUT_FILE;
  return EFI_SUCCESS;
}

// LogSaveLog
/// Save log contents to file
/// @param Path The log file path
/// @return Whether the log file was saved or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_NOT_FOUND         The log file path could not be found
/// @retval EFI_SUCCESS           The log file was saved
STATIC EFI_STATUS
EFIAPI
LogSaveLog (
  IN CHAR16  *Path
) {
  EFI_STATUS       Status = EFI_NOT_FOUND;
  LOG_CHUNK       *Chunk = mLogChunks;
  EFI_FILE_HANDLE  Handle = NULL;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Nothing to do
  if (Chunk == NULL) {
    return EFI_SUCCESS;
  }
  // Open log file
  Status = FileHandleOpen(&Handle, NULL, Path, EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);
  if (!EFI_ERROR(Status)) {
    // Make sure the file size is zero ie truncate
    Status = FileHandleSetSize(Handle, 0);
    if (!EFI_ERROR(Status)) {
      // Write each chunk to log file
      while (Chunk != NULL) {
        // Write this chunk to log file
        UINTN Size = Chunk->Size;
        Status = FileHandleWrite(Handle, &Size, (VOID *)Chunk->Text);
        if (EFI_ERROR(Status)) {
          break;
        }
        // Get next chunk
        Chunk = Chunk->Next;
      }
    }
    // Close the file
    FileHandleClose(Handle);
  }
  // Return the last chunk status
  return Status;
}
// SaveLog
/// Save log contents to file
/// @param Path The log file path
/// @return Whether the log file was saved or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_NOT_FOUND         The log file path could not be found
/// @retval EFI_SUCCESS           The log file was saved
EFI_STATUS
EFIAPI
SaveLog (
  IN CHAR16  *Path
) {
  if ((mLog != NULL) && (mLog->Save != NULL)) {
    return mLog->Save(Path);
  }
  return LogSaveLog(Path);
}

// LogDateTime
/// Log the date and/or time
/// @param Date Log the date
/// @param Time Log the time
/// @return The number of characters logged
UINTN
EFIAPI
LogDateTime (
  IN EFI_TIME *Time OPTIONAL,
  IN UINT32    Flags
) {
  EFI_TIME Now;
  // Check parameters
  if ((Flags & LOG_DATE_TIME) == 0) {
    return 0;
  }
  // Get time
  if (Time == NULL) {
    if ((gRT == NULL) || (gRT->GetTime == NULL) ||
        EFI_ERROR(gRT->GetTime(&Now, NULL))) {
      return 0;
    }
    Time = &Now;
  }
  // Log date and/or time
  if ((Flags & LOG_DATE_TIME) == LOG_DATE_TIME) {
    return Log(L"%4.4u-%02.2u-%02.2u %02.2u:%02.2u:%02.2u", Time->Year, Time->Month, Time->Day, Time->Hour, Time->Minute, Time->Second);
  } else if (((Flags & LOG_DATE) != 0)) {
    return Log(L"%4.4u-%02.2u-%02.2u", Time->Year, Time->Month, Time->Day);
  }
  return Log(L"%02.2u:%02.2u:%02.2u", Time->Hour, Time->Minute, Time->Second);
}
// LogDateAndTime
// Log the current date and time
/// @return The number of characters logged
UINTN
EFIAPI
LogDateAndTime (
  VOID
) {
  return LogDateTime(NULL, LOG_DATE_TIME);
}
// LogDate
/// Log the current date
/// @return The number of characters logged
UINTN
EFIAPI
LogDate (
  VOID
) {
  return LogDateTime(NULL, LOG_DATE);
}
// LogTime
/// Log the current time
/// @return The number of characters logged
UINTN
EFIAPI
LogTime (
  VOID
) {
  return LogDateTime(NULL, LOG_TIME);
}
// LogTimestamp
/// Log a timestamp
/// @return The number of characters logged
UINTN
EFIAPI
LogTimestamp (
  VOID
) {
  UINT64 Start = ((mLog == NULL) || (mLog->GetStart == NULL)) ? mLogCounterStart : mLog->GetStart();
  UINT64 Counter = GetPerformanceCounter();
  UINT64 Nanoseconds = GetTimeInNanoSecond((Counter > Start) ? (Counter - Start) : (Start - Counter));
  UINT64 Seconds = DivU64x64Remainder(Nanoseconds, 1000000000, &Nanoseconds);
  UINT64 Minutes = DivU64x64Remainder(Seconds, 60, &Seconds);
  UINT64 Hours = DivU64x64Remainder(Minutes, 60, &Minutes);
  return Log(L"%u:%02u:%02u.%03u", (UINT32)Hours, (UINT32)Minutes, (UINT32)Seconds, (UINT32)DivU64x32(Nanoseconds, 1000000));
}

// PrintLogInformation
/// Print log information
STATIC VOID
EFIAPI
PrintLogInformation (
  VOID
) {
  // Print version information
  Log(L"%s %s\n", PROJECT_NAME, PROJECT_VERSION);
  Log2(L"Architecture:", L"%s\n", PROJECT_ARCH);
  Log2(L"Build date/time:", L"%s\n", PROJECT_DATETIME);
  Log2(L"Start:", NULL);
  LogDateAndTime();
  Log(L"\n");
  // Print log information
  if ((mLogPath != NULL) && ((mLogOutputs & LOG_OUTPUT_FILE) != 0)) {
    Log2(L"Log path:", L"\"%s\"\n", mLogPath);
  }
  if ((mLogOutputs & LOG_OUTPUT_ALL) == LOG_OUTPUT_ALL) {
    Log2(L"Log outputs:", L"File & Console\n");
  } else if ((mLogOutputs & LOG_OUTPUT_FILE) != 0) {
    Log2(L"Log outputs:", L"File\n");
  } else if ((mLogOutputs & LOG_OUTPUT_CONSOLE) != 0) {
    Log2(L"Log outputs:", L"Console\n");
  } else {
    Log2(L"Log outputs:", L"None\n");
  }
}

// SetBestConsoleMode
/// Set the best console mode available
STATIC EFI_STATUS
EFIAPI
SetBestConsoleMode (
  VOID
) {
  UINTN Columns = 0;
  UINTN Rows = 0;
  UINTN BestCols = 0;
  UINTN BestRows = 0;
  UINTN Best = 0;
  UINTN Index = 0;

  // Check for system table and con out
  if ((gST == NULL) || (gST->ConOut == NULL) || (gST->ConOut->Mode == NULL) ||
      (gST->ConOut->QueryMode == NULL) || (gST->ConOut->SetMode == NULL)) {
    return EFI_UNSUPPORTED;
  }

  // Hide console cursor
  if (gST->ConOut->EnableCursor != NULL) {
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  }
  // Query modes to find the best text mode
  while (Index <= (UINTN)(gST->ConOut->Mode->MaxMode)) {
    if (!EFI_ERROR(gST->ConOut->QueryMode(gST->ConOut, Index, &Columns, &Rows))) {
      // Set best mode
      if (((Columns > BestCols) && (Rows > BestRows)) || ((Columns * Rows) > (BestCols * BestRows))) {
        BestCols = Columns;
        BestRows = Rows;
        Best = Index;
      }
    }
    ++Index;
  }
  // Check if mode needs changed
  if ((BestCols != 0) && (BestRows != 0) && (Best != (UINTN)(gST->ConOut->Mode->Mode))) {
    // Change console mode
    return gST->ConOut->SetMode(gST->ConOut, Best);
  }
  // No mode change
  return EFI_SUCCESS;
}

// LogGetStart
/// Get the performance counter start
/// @return The performance counter start
STATIC UINT64
EFIAPI
LogGetStart (
  VOID
) {
  return mLogCounterStart;
}

// mLogProtocol
/// Log protocol
STATIC LOG_PROTOCOL mLogProtocol = {
  LogVPrint,
  LogGetStart,
  LogSaveLog
};

// LogLibInitialize
/// Log library initialize use
/// @return Whether the library initialized successfully or not
/// @retval EFI_SUCCESS The log successfully initialized
EFI_STATUS
EFIAPI
LogLibInitialize (
  VOID
) {
  // Check for boot services
  if ((gBS == NULL) || (gBS->LocateProtocol == NULL) ||
      (gBS->InstallMultipleProtocolInterfaces == NULL) ||
      (gBS->UninstallMultipleProtocolInterfaces == NULL)) {
    return EFI_UNSUPPORTED;
  }
  // Get the performance counter for the start
  mLogCounterStart = GetPerformanceCounter();
  // Check if CPU information already exists
  if ((gBS->LocateProtocol(&mLogGuid, NULL, (VOID **)&mLog) == EFI_SUCCESS) && (mLog != NULL)) {
    return EFI_SUCCESS;
  }

  // Create a default log path if needed
  if (mLogPath == NULL) {
    mLogPath = StrDup(LOG_DEFAULT_FILE);
    if (mLogPath == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  // Set the console mode
  if ((mLogOutputs & LOG_OUTPUT_CONSOLE) != 0) {
    SetBestConsoleMode();
  }
  // Print log information
  PrintLogInformation();
  // Install log protocol
  mLogHandle = NULL;
  mLogProtocol.VPrint = LogVPrint,
  mLogProtocol.GetStart = LogGetStart;
  mLogProtocol.Save = LogSaveLog;
  return gBS->InstallMultipleProtocolInterfaces(&mLogHandle, &mLogGuid, &mLogProtocol, NULL);
}
// LogLibFinish
/// Log library finish use
/// @return Whether the library was finished successfully or not
/// @retval EFI_SUCCESS The log successfully finished
EFI_STATUS
EFIAPI
LogLibFinish (
  VOID
) {
  // Uninstall log
  mLog = NULL;
  if (mLogHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces(mLogHandle, &mLogGuid, (VOID *)&mLogProtocol, NULL);
    mLogHandle = NULL;
  }
  // Free the log path
  if (mLogPath != NULL) {
    FreePool(mLogPath);
    mLogPath = NULL;
  }
  // Free all the log chunks
  while (mLogChunks != NULL) {
    mLogChunk = mLogChunks;
    mLogChunks = mLogChunk->Next;
    FreePages(mLogChunk, EFI_SIZE_TO_PAGES(LOG_CHUNK_SIZE));
  }
  mLogChunk = NULL;
  return EFI_SUCCESS;
}

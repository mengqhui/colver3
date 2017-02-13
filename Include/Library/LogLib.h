//
/// @file Include/Library/LogLib.h
///
/// Log library
///

#pragma once
#ifndef __LOG_LIBRARY_HEADER__
#define __LOG_LIBRARY_HEADER__

#include <Library/FileLib.h>

#include <Library/MpInitLib.h>

// LOG_PREFIX_WIDTH
/// Log prefix alignment width
#define LOG_PREFIX_WIDTH 16

// LOG_OUTPUT_FILE
/// Log file output method
#define LOG_OUTPUT_FILE 0x1
// LOG_OUTPUT_CONSOLE
/// Log console output method
#define LOG_OUTPUT_CONSOLE 0x2
// LOG_OUTPUT_ALL
/// Log all output methods
#define LOG_OUTPUT_ALL (LOG_OUTPUT_FILE | LOG_OUTPUT_CONSOLE)

#if defined(PROJECT_DEBUG)

#define LOG(...) Log(__VA_ARGS__)
#define LOG2(...) Log2(__VA_ARGS__)
#define LOG3(...) Log3(__VA_ARGS__)

#else

#define LOG(...)
#define LOG2(...)
#define LOG3(...)

#endif

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
);
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
);
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
);
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
);
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
);
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
);

// SetLogOutput
/// Set log output methods
/// @return The log output methods that are enabled
UINT32
EFIAPI
GetLogOutput (
  VOID
);
// SetLogOutput
/// Set log output methods
/// @param Outputs The log output methods to enable
VOID
EFIAPI
SetLogOutput (
  IN UINT32 Outputs
);

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
);
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
);

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
  IN CHAR16 *Path
);

// LOG_DATE
/// Flag to log date
#define LOG_DATE 0x1
// LOG_TIME
/// Flag to log time
#define LOG_TIME 0x2
// LOG_DATE_TIME
/// Flag to log date and time
#define LOG_DATE_TIME (LOG_DATE | LOG_TIME)

// LogDateTime
/// Log the date and/or time
/// @param Date Log the date
/// @param Time Log the time
/// @return The number of characters logged
STATIC UINTN
EFIAPI
LogDateTime (
  IN EFI_TIME *Time OPTIONAL,
  IN UINT32    Flags
);

// LogDateAndTime
// Log the current date and time
/// @return The number of characters logged
UINTN
EFIAPI
LogDateAndTime (
  VOID
);
// LogDate
/// Log the current date
/// @return The number of characters logged
UINTN
EFIAPI
LogDate (
  VOID
);
// LogTime
/// Log the current time
/// @return The number of characters logged
UINTN
EFIAPI
LogTime (
  VOID
);
// LogTimestamp
/// Log a timestamp
/// @return The number of characters logged
UINTN
EFIAPI
LogTimestamp (
  VOID
);

#endif // __LOG_LIBRARY_HEADER__

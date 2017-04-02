//
/// @file Include/Library/LogLib.h
///
/// Log library
///

#pragma once
#ifndef __LOG_LIBRARY_HEADER__
#define __LOG_LIBRARY_HEADER__

#include <Library/FileLib.h>

// LOG_PREFIX_WIDTH
/// Log prefix alignment width
#define LOG_PREFIX_WIDTH 20

// LOG_OUTPUT_CONSOLE
/// Log console output method
#define LOG_OUTPUT_CONSOLE 0x1
// LOG_OUTPUT_FILE
/// Log file output method
#define LOG_OUTPUT_FILE 0x2
// LOG_OUTPUT_SERIAL
/// Log serial output methof
#define LOG_OUTPUT_SERIAL 0x4
// LOG_OUTPUT_ALL
/// Log all output methods
#define LOG_OUTPUT_ALL (LOG_OUTPUT_FILE | LOG_OUTPUT_CONSOLE | LOG_OUTPUT_SERIAL)

// LOG_DEFAULT_FILE
/// The default log file
#define LOG_DEFAULT_FILE PROJECT_ROOT_PATH L"\\" PROJECT_SAFE_NAME L"\\" PROJECT_SAFE_NAME L".log"

// LOG_VERBOSE_LEVEL
/// Log default verbose level
#if defined(PROJECT_DEBUG)

#define LOG_VERBOSE_LEVEL LOG_VERBOSE_LEVEL_DEBUG

#else

#define LOG_VERBOSE_LEVEL LOG_VERBOSE_LEVEL_ALWAYS

#endif

// LOG_VERBOSE_LEVEL_ALWAYS
/// Log always verbose level
#define LOG_VERBOSE_LEVEL_ALWAYS 0
// LOG_VERBOSE_LEVEL_EXTRA
/// Log extra verbose level
#define LOG_VERBOSE_LEVEL_EXTRA 1
// LOG_VERBOSE_LEVEL_DEBUG
/// Log debug verbose level
#define LOG_VERBOSE_LEVEL_DEBUG 2

// LOG
/// Log at the debug verbose level
#define LOG(...) Verbose(LOG_VERBOSE_LEVEL_DEBUG, __VA_ARGS__)
// LOG2
/// Log at the debug verbose level
#define LOG2(...) Verbose2(LOG_VERBOSE_LEVEL_DEBUG, __VA_ARGS__)
// LOG3
/// Log at the debug verbose level
#define LOG3(...) Verbose3(LOG_VERBOSE_LEVEL_DEBUG, __VA_ARGS__)

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

// Verbose
/// Log formatted text to log and/or console
/// @param Level  The verbosity level, levels equal or below the current threshold will log which means a level of zero will always log
/// @param Format The format specifier for formatting the text
/// @param ...    The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
Verbose (
  UINTN   Level,
  CHAR16 *Format,
  ...
);
// Verbose2
/// Log formatted text to log and/or console
/// @param Level  The verbosity level, levels equal or below the current threshold will log which means a level of zero will always log
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param ...    The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
Verbose2 (
  UINTN   Level,
  CHAR16 *Prefix,
  CHAR16 *Format,
  ...
);
// Verbose3
/// Log formatted text to log and/or console
/// @param Level  The verbosity level, levels equal or below the current threshold will log which means a level of zero will always log
/// @param Width  The alignment width
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param ...    The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
Verbose3 (
  UINTN   Level,
  UINTN   Width,
  CHAR16 *Prefix,
  CHAR16 *Format,
  ...
);
// VVerbose
/// Log formatted text to log and/or console
/// @param Level  The verbosity level, levels equal or below the current threshold will log which means a level of zero will always log
/// @param Format The format specifier for formatting the text
/// @param Args   The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
VVerbose (
  UINTN    Level,
  CHAR16  *Format,
  VA_LIST  Args
);
// VVerbose2
/// Log formatted text to log and/or console
/// @param Level  The verbosity level, levels equal or below the current threshold will log which means a level of zero will always log
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param Args   The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
VVerbose2 (
  UINTN    Level,
  CHAR16  *Prefix,
  CHAR16  *Format,
  VA_LIST  Args
);
// VVerbose3
/// Log formatted text to log and/or console
/// @param Level  The verbosity level, levels equal or below the current threshold will log which means a level of zero will always log
/// @param Width  The alignment width
/// @param Prefix The aligned prefix
/// @param Format The format specifier for formatting the text
/// @param Args   The parameters for the format string
/// @return The count of characters written to the log output
UINTN
EFIAPI
VVerbose3 (
  UINTN    Level,
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
/// @param Path   The log file path
/// @param Save   Whether to save the contents of the log or start empty
/// @param Append Append the log to the file instead of overwrite
/// @return Whether the log file was opened or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_NOT_FOUND         The log file path could not be found
/// @retval EFI_SUCCESS           The log file path was opened (and contents saved)
EFI_STATUS
EFIAPI
SetLogPath (
  IN CHAR16  *Path,
  IN BOOLEAN  Save,
  IN BOOLEAN  Append
);

// SaveLog
/// Save log contents to file
/// @param Path   The log file path
/// @param Append Append the log to the file instead of overwrite
/// @return Whether the log file was saved or not
/// @retval EFI_INVALID_PARAMETER Path is NULL
/// @retval EFI_NOT_FOUND         The log file path could not be found
/// @retval EFI_SUCCESS           The log file was saved
EFI_STATUS
EFIAPI
SaveLog (
  IN CHAR16  *Path,
  IN BOOLEAN  Append
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

//
/// @file Include/Library/ConfigLib.h
///
/// Configuration library
///

#pragma once
#ifndef __CONFIG_LIBRARY_HEADER__
#define __CONFIG_LIBRARY_HEADER__

#include <Library/XmlLib.h>

#include <Protocol/SimpleFileSystem.h>

// DEFAULT_CONFIG_FILE
/// Default configuration file
#define DEFAULT_CONFIG_FILE PROJECT_ROOT_PATH L"\\" PROJECT_SAFE_NAME L"\\" PROJECT_SAFE_NAME L".XML"

// CONFIG_TYPE
/// Configuration value type
typedef enum _CONFIG_TYPE CONFIG_TYPE;
enum _CONFIG_TYPE {

  // CONFIG_TYPE_UNKNOWN
  /// Unknown configuration type (invalid value)
  CONFIG_TYPE_UNKNOWN = 0,
  // CONFIG_TYPE_LIST
  /// List configuration type, no value but child nodes
  CONFIG_TYPE_LIST,
  // CONFIG_TYPE_BOOLEAN
  /// Boolean configuration type
  CONFIG_TYPE_BOOLEAN,
  // CONFIG_TYPE_INTEGER
  /// Integer configuration type
  CONFIG_TYPE_INTEGER,
  // CONFIG_TYPE_UNSIGNED
  /// Unsigned integer configuration type
  CONFIG_TYPE_UNSIGNED,
  // CONFIG_TYPE_STRING
  /// String configuration type
  CONFIG_TYPE_STRING,
  // CONFIG_TYPE_DATA
  /// Data configuration type
  CONFIG_TYPE_DATA,

};
// CONFIG_DATA
/// Configuration data value
typedef struct _CONFIG_DATA CONFIG_DATA;
struct _CONFIG_DATA {

  // Size
  /// Size in bytes of the data
  UINTN  Size;
  // Data
  /// Data memory pointer
  VOID  *Data;

};
// CONFIG_VALUE
/// Configuration value
typedef union _CONFIG_VALUE CONFIG_VALUE;
union _CONFIG_VALUE {

  // Boolean
  /// Configuration boolean value
  BOOLEAN      Boolean;
  // Integer
  /// Configuration integer value
  INTN         Integer;
  // Unsigned
  /// Configuration unsigned integer value
  UINTN        Unsigned;
  // String
  /// Configuration string value
  CHAR16      *String;
  // Data
  /// Configuration data value
  CONFIG_DATA  Data;

};

// CONFIG_INSPECT
/// Configuration value inspector
/// @param Path    The configuration path
/// @param Type    The type of the configuration key
/// @param Value   The value of the configuration key if Type is a valid value type (not CONFIG_TYPE_UNKNOWN or CONFIG_TYPE_LIST)
/// @param Context The context passed when inspection was invoked
/// @retval TRUE  if inspection should continue
/// @retval FALSE If inspection should be aborted
typedef BOOLEAN
(EFIAPI
*CONFIG_INSPECT) (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value OPTIONAL,
  IN VOID         *Context
);

// ConfigLoad
/// Load configuration information from file
/// @param Root If Path is NULL the file handle to use to load, otherwise the root file handle
/// @param Path If Root is NULL the full device path string to the file, otherwise the root relative path
/// @return Whether the configuration was loaded successfully or not
/// @retval EFI_INVALID_PARAMETER If Root and Path are both NULL
/// @retval EFI_NOT_FOUND         If the configuration file could not be opened
/// @retval EFI_SUCCESS           If the configuration file was loaded successfully
EFI_STATUS
EFIAPI
ConfigLoad (
  IN EFI_FILE_HANDLE  Root OPTIONAL,
  IN CHAR16          *Path OPTIONAL
);
// ConfigParse
/// Parse configuration information from string
/// @param Size   The size, in bytes, of the configuration string
/// @param Config The configuration string to parse
/// @param Source The unique source name
/// @return Whether the configuration was parsed successfully or not
/// @retval EFI_INVALID_PARAMETER If Config is NULL or Size is zero
/// @retval EFI_SUCCESS           If the configuration string was parsed successfully
EFI_STATUS
EFIAPI
ConfigParse (
  IN UINTN   Size,
  IN VOID   *Config,
  IN CHAR16 *Source OPTIONAL
);
// ConfigParseXml
/// Parse configuration information from XML document tree
/// @param Tree   The XML document tree to parse
/// @param Source The unique source name
/// @return Whether the configuration was parsed successfully or not
/// @retval EFI_INVALID_PARAMETER If Tree is NULL
/// @retval EFI_SUCCESS           If the configuration was parsed successfully
EFI_STATUS
EFIAPI
ConfigParseXml (
  IN XML_TREE *Tree,
  IN CHAR16   *Source OPTIONAL
);

// ConfigFree
/// Free all configuration values
/// @return Whether the configuration values were freed or not
/// @retval EFI_SUCCESS If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigFree (
  VOID
);
// ConfigPartialFree
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free or NULL for root (same as ConfigFree)
/// @param ...  The argument list
/// @return Whether the configuration values were freed or not
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigPartialFree (
  IN CHAR16 *Path OPTIONAL,
  IN ...
);
// ConfigVPartialFree
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free or NULL for root (same as ConfigFree)
/// @param Args The argument list
/// @return Whether the configuration values were freed or not
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigVPartialFree (
  IN CHAR16  *Path OPTIONAL,
  IN VA_LIST  Args
);

// ConfigExists
/// Check if a configuration key exists
/// @param Path The configuration path
/// @param ...  The argument list
/// @return Whether the configuration key exists or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_NOT_FOUND         If the configuration key path does not exist
/// @retval EFI_SUCCESS           If the configuration key path exists
EFI_STATUS
EFIAPI
ConfigExists (
  IN CHAR16 *Path,
  ...
);
// ConfigVExists
/// Check if a configuration key exists
/// @param Path The configuration path
/// @param Args The argument list
/// @return Whether the configuration key exists or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_NOT_FOUND         If the configuration key path does not exist
/// @retval EFI_SUCCESS           If the configuration key path exists
EFI_STATUS
EFIAPI
ConfigVExists (
  IN CHAR16  *Path,
  IN VA_LIST  Args
);

// ConfigInspect
/// Inspect configuration values
/// @param Path      The configuration path or NULL for root
/// @param Inspector The inspection callback
/// @param Context   The context to pass to the callback
/// @param Recursive Whether the inspection recursively inspects children
/// @param ...       The argument list
/// @return Whether the inspection finished or not
/// @retval EFI_INVALID_PARAMETER If Inspector is NULL
/// @retval EFI_NOT_FOUND         If Path is not found
/// @retval EFI_ABORTED           If the inspection was aborted in the callback
/// @retval EFI_SUCCESSS          If the inspection finished successfully
EFI_STATUS
EFIAPI
ConfigInspect (
  IN CHAR16         *Path OPTIONAL,
  IN CONFIG_INSPECT  Inspector,
  IN VOID           *Context OPTIONAL,
  IN BOOLEAN         Recursive,
  ...
);
// ConfigVInspect
/// Inspect configuration values
/// @param Path      The configuration path or NULL for root
/// @param Inspector The inspection callback
/// @param Context   The context to pass to the callback
/// @param Recursive Whether the inspection recursively inspects children
/// @param Args      The argument list
/// @return Whether the inspection finished or not
/// @retval EFI_INVALID_PARAMETER If Inspector is NULL
/// @retval EFI_NOT_FOUND         If Path is not found
/// @retval EFI_ABORTED           If the inspection was aborted in the callback
/// @retval EFI_SUCCESSS          If the inspection finished successfully
EFI_STATUS
EFIAPI
ConfigVInspect (
  IN CHAR16         *Path OPTIONAL,
  IN CONFIG_INSPECT  Inspector,
  IN VOID           *Context OPTIONAL,
  IN BOOLEAN         Recursive,
  IN VA_LIST         Args
);

// ConfigGetList
/// Get a list of names for the children of the configuration path
/// @param Path  The configuration path of which to get child names or NULL for root
/// @param List  On output, the string list of names of children
/// @param Count On output, the count of strings in the list
/// @param ...   The argument list
/// @return Whether the string list was returned or not
/// @retval EFI_INVALID_PARAMETER If Path, List, or Count is NULL or *List is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the stirng list
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_ACCESS_DENIED     If the configuration path is protected
/// @retval EFI_SUCCESS           If the string list was returned successfully
EFI_STATUS
EFIAPI
ConfigGetList (
  IN  CHAR16   *Path OPTIONAL,
  OUT CHAR16 ***List,
  OUT UINTN    *Count,
  IN  ...
);
// ConfigVGetList
/// Get a list of names for the children of the configuration path
/// @param Path  The configuration path of which to get child names or NULL for root
/// @param List  On output, the string list of names of children
/// @param Count On output, the count of strings in the list
/// @param Args  The argument list
/// @return Whether the string list was returned or not
/// @retval EFI_INVALID_PARAMETER If Path, List, or Count is NULL or *List is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the stirng list
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_ACCESS_DENIED     If the configuration path is protected
/// @retval EFI_SUCCESS           If the string list was returned successfully
EFI_STATUS
EFIAPI
ConfigVGetList (
  IN  CHAR16    *Path OPTIONAL,
  OUT CHAR16  ***List,
  OUT UINTN     *Count,
  IN  VA_LIST    Args
);

// ConfigGetCount
/// Get the count of children of the configuration path
/// @param Path  The configuration path or NULL for root
/// @param Count On output, the count of children
/// @param ...   The argument list
/// @return Whether the count was returned or not
/// @retval EFI_INVALID_PARAMETER If Path or Count is NULL
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_SUCCESS           If the count was returned successfully
EFI_STATUS
EFIAPI
ConfigGetCount (
  IN  CHAR16 *Path OPTIONAL,
  OUT UINTN  *Count,
  IN  ...
);
// ConfigVGetCount
/// Get the count of children of the configuration path
/// @param Path  The configuration path or NULL for root
/// @param Count On output, the count of children
/// @param Args  The argument list
/// @return Whether the count was returned or not
/// @retval EFI_INVALID_PARAMETER If Path or Count is NULL
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_SUCCESS           If the count was returned successfully
EFI_STATUS
EFIAPI
ConfigVGetCount (
  IN  CHAR16  *Path OPTIONAL,
  OUT UINTN   *Count,
  IN  VA_LIST  Args
);

// ConfigGetType
/// Get the type of a configuration value
/// @param Path The path of the configuration value of which to get the type
/// @param Type On output, the configuration value type
/// @param ...  The argument list
/// @retval EFI_INVALID_PARAMETER If Path or Type is NULL
/// @retval EFI_NOT_FOUND         If the configuration value was not found
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetType (
  IN  CHAR16      *Path,
  OUT CONFIG_TYPE *Type,
  IN  ...
);
// ConfigVGetType
/// Get the type of a configuration value
/// @param Path The path of the configuration value of which to get the type
/// @param Type On output, the configuration value type
/// @param Args The argument list
/// @retval EFI_INVALID_PARAMETER If Path or Type is NULL
/// @retval EFI_NOT_FOUND         If the configuration value was not found
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetType (
  IN  CHAR16      *Path,
  OUT CONFIG_TYPE *Type,
  IN  VA_LIST      Args
);

// ConfigGetValue
/// Get a configuration value
/// @param Path  The path of the configuration value
/// @param Type  On output, the type of the configuration value
/// @param Value On output, the value of the configuration value
/// @param ...   The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, Type, or Value is NULL
/// @retval EFI_NOT_FOUND         If the configuration value was not found
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetValue (
  IN  CHAR16       *Path,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  ...
);
// ConfigVGetValue
/// Get a configuration value
/// @param Path  The path of the configuration value
/// @param Type  On output, the type of the configuration value
/// @param Value On output, the value of the configuration value
/// @param Args  The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, Type, or Value is NULL
/// @retval EFI_NOT_FOUND         If the configuration value was not found
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetValue (
  IN  CHAR16       *Path,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  VA_LIST       Args
);

// ConfigGetBoolean
/// Get a boolean configuration value
/// @param Path    The path of the configuration value
/// @param Boolean On output, the boolean configuration value
/// @param ...     The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or Boolean is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetBoolean (
  IN  CHAR16  *Path,
  OUT BOOLEAN *Boolean,
  IN  ...
);
// ConfigVGetBoolean
/// Get a boolean configuration value
/// @param Path    The path of the configuration value
/// @param Boolean On output, the boolean configuration value
/// @param Args    The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or Boolean is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetBoolean (
  IN  CHAR16  *Path,
  OUT BOOLEAN *Boolean,
  IN  VA_LIST  Args
);

// ConfigGetInteger
/// Get an integer configuration value
/// @param Path    The path of the configuration value
/// @param Integer On output, the integer configuration value
/// @param ...     The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or Integer is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetInteger (
  IN  CHAR16 *Path,
  OUT INTN   *Integer,
  IN  ...
);
// ConfigVGetInteger
/// Get an integer configuration value
/// @param Path    The path of the configuration value
/// @param Integer On output, the integer configuration value
/// @param Args    The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or Integer is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetInteger (
  IN  CHAR16  *Path,
  OUT INTN    *Integer,
  IN  VA_LIST  Args
);

// ConfigGetUnsigned
/// Get an unsigned integer configuration value
/// @param Path    The path of the configuration value
/// @param Unsigned On output, the unsigned integer configuration value
/// @param ...      The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or Unsigned is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetUnsigned (
  IN  CHAR16 *Path,
  OUT UINTN  *Unsigned,
  IN  ...
);
// ConfigVGetUnsigned
/// Get an unsigned integer configuration value
/// @param Path     The path of the configuration value
/// @param Unsigned On output, the unsigned integer configuration value
/// @param Args     The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or Unsigned is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetUnsigned (
  IN  CHAR16  *Path,
  OUT UINTN   *Unsigned,
  IN  VA_LIST  Args
);

// ConfigGetString
/// Get a string configuration value
/// @param Path   The path of the configuration value
/// @param String On output, the string configuration value
/// @param ...    The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetString (
  IN  CHAR16  *Path,
  OUT CHAR16 **String,
  IN  ...
);
// ConfigVGetString
/// Get a string configuration value
/// @param Path   The path of the configuration value
/// @param String On output, the string configuration value
/// @param Args   The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetString (
  IN  CHAR16   *Path,
  OUT CHAR16  **String,
  IN  VA_LIST   Args
);

// ConfigGetData
/// Get a data configuration value
/// @param Path The path of the configuration value
/// @param Size On output, the data configuration value size in bytes
/// @param Data On output, the data configuration value
/// @param ...  The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, Size, or Data is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetData (
  IN  CHAR16  *Path,
  OUT UINTN   *Size,
  OUT VOID   **Data,
  IN  ...
);
// ConfigVGetData
/// Get a data configuration value
/// @param Path The path of the configuration value
/// @param Size On output, the data configuration value size in bytes
/// @param Data On output, the data configuration value
/// @param Args The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, Size, or Data is NULL
/// @retval EFI_NOT_FOUND         If the configuration value path was not found
/// @retval EFI_ABORTED           If the configuration value type does not match
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetData (
  IN  CHAR16   *Path,
  OUT UINTN    *Size,
  OUT VOID    **Data,
  IN  VA_LIST   Args
);

// ConfigGetValueWithDefault
/// Get a configuration value with a default fallback value
/// @param Path         The path of the configuration value
/// @param DefaultType  The default type of the configuration value
/// @param DefaultValue The default value of the configuration value
/// @param Type         On output, the type of the configuration value
/// @param Value        On output, the value of the configuration value
/// @param ...          The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, DefaultValue, Type or Value is NULL or DefaultType is invalid
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetValueWithDefault (
  IN  CHAR16       *Path,
  IN  CONFIG_TYPE   DefaultType,
  IN  CONFIG_VALUE *DefaultValue,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  ...
);
// ConfigVGetValueWithDefault
/// Get a configuration value with a default fallback value
/// @param Path         The path of the configuration value
/// @param DefaultType  The default type of the configuration value
/// @param DefaultValue The default value of the configuration value
/// @param Type         On output, the type of the configuration value
/// @param Value        On output, the value of the configuration value
/// @param Args         The argument list
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, DefaultValue, Type or Value is NULL or DefaultType is invalid
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigVGetValueWithDefault (
  IN  CHAR16       *Path,
  IN  CONFIG_TYPE   DefaultType,
  IN  CONFIG_VALUE *DefaultValue,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  VA_LIST       Args
);

// ConfigGetBooleanWithDefault
/// Get a boolean configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultBoolean The default boolean configuration value
/// @param ...            The argument list
/// @return The boolean configuration value
BOOLEAN
EFIAPI
ConfigGetBooleanWithDefault (
  IN CHAR16  *Path,
  IN BOOLEAN  DefaultBoolean,
  IN ...
);
// ConfigVGetBooleanWithDefault
/// Get a boolean configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultBoolean The default boolean configuration value
/// @param Args           The argument list
/// @return The boolean configuration value
BOOLEAN
EFIAPI
ConfigVGetBooleanWithDefault (
  IN CHAR16  *Path,
  IN BOOLEAN  DefaultBoolean,
  IN VA_LIST  Args
);

// ConfigGetIntegerWithDefault
/// Get an integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultInteger The default integer configuration value
/// @param ...            The argument list
/// @return The integer configuration value
INTN
EFIAPI
ConfigGetIntegerWithDefault (
  IN CHAR16 *Path,
  IN INTN    DefaultInteger,
  IN ...
);
// ConfigVGetIntegerWithDefault
/// Get an integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultInteger The default integer configuration value
/// @param Args           The argument list
/// @return The integer configuration value
INTN
EFIAPI
ConfigVGetIntegerWithDefault (
  IN CHAR16  *Path,
  IN INTN     DefaultInteger,
  IN VA_LIST  Args
);

// ConfigGetUnsignedWithDefault
/// Get an unsigned integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaulUnsigned The default unsigned configuration value
/// @param ...            The argument list
/// @return The unsigned integer configuration value
UINTN
EFIAPI
ConfigGetUnsignedWithDefault (
  IN CHAR16 *Path,
  IN UINTN   DefaultUnsigned,
  IN ...
);
// ConfigVGetUnsignedWithDefault
/// Get an unsigned integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaulUnsigned The default unsigned configuration value
/// @param Args           The argument list
/// @return The unsigned integer configuration value
UINTN
EFIAPI
ConfigVGetUnsignedWithDefault (
  IN CHAR16  *Path,
  IN UINTN    DefaultUnsigned,
  IN VA_LIST  Args
);

// ConfigGetStringWithDefault
/// Get a string configuration value with default
/// @param Path          The path of the configuration value
/// @param DefaultString The default string configuration value
/// @param ...           The argument list
/// @return The string configuration value
CHAR16 *
EFIAPI
ConfigGetStringWithDefault (
  IN CHAR16 *Path,
  IN CHAR16 *DefaultString,
  IN ...
);
// ConfigVGetStringWithDefault
/// Get a string configuration value with default
/// @param Path          The path of the configuration value
/// @param DefaultString The default string configuration value
/// @param Args          The argument list
/// @return The string configuration value
CHAR16 *
EFIAPI
ConfigVGetStringWithDefault (
  IN CHAR16  *Path,
  IN CHAR16  *DefaultString,
  IN VA_LIST  Args
);

// ConfigGetDataWithDefault
/// Get a data configuration value with default
/// @param Path        The path of the configuration value
/// @param DefaultSize The default data configuration value size
/// @param DefaultData The default data configuration value
/// @param Size        On output, the data configuration value size
/// @param ...         The argument list
/// @return The data configuration value
VOID *
EFIAPI
ConfigGetDataWithDefault (
  IN  CHAR16 *Path,
  IN  UINTN   DefaultSize,
  IN  VOID   *DefaultData,
  OUT UINTN  *Size,
  IN  ...
);
// ConfigVGetDataWithDefault
/// Get a data configuration value with default
/// @param Path        The path of the configuration value
/// @param DefaultSize The default data configuration value size
/// @param DefaultData The default data configuration value
/// @param Size        On output, the data configuration value size
/// @param Args        The argument list
/// @return The data configuration value
VOID *
EFIAPI
ConfigVGetDataWithDefault (
  IN  CHAR16  *Path,
  IN  UINTN    DefaultSize,
  IN  VOID    *DefaultData,
  OUT UINTN   *Size,
  IN  VA_LIST  Args
);

// ConfigSetValue
/// Set a configuration value
/// @param Path      The path of the configuration value
/// @param Type      The configuration type to set
/// @param Value     The configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param ...       The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Value is NULL or Type is invalid
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetValue (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value,
  IN BOOLEAN       Overwrite,
  IN ...
);
// ConfigVSetValue
/// Set a configuration value
/// @param Path      The path of the configuration value
/// @param Type      The configuration type to set
/// @param Value     The configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param Args      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Value is NULL or Type is invalid
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSetValue (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value,
  IN BOOLEAN       Overwrite,
  IN VA_LIST       Args
);

// ConfigSetBoolean
/// Set a boolean configuration value
/// @param Path      The path of the configuration value
/// @param Boolean   The boolean configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param ...       The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetBoolean (
  IN CHAR16  *Path,
  IN BOOLEAN  Boolean,
  IN BOOLEAN  Overwrite,
  IN ...
);
// ConfigVSetBoolean
/// Set a boolean configuration value
/// @param Path      The path of the configuration value
/// @param Boolean   The boolean configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param Args      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSetBoolean (
  IN CHAR16  *Path,
  IN BOOLEAN  Boolean,
  IN BOOLEAN  Overwrite,
  IN VA_LIST  Args
);

// ConfigSetInteger
/// Set an integer configuration value
/// @param Path      The path of the configuration value
/// @param Integer   The integer configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param ...       The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetInteger (
  IN CHAR16  *Path,
  IN INTN     Integer,
  IN BOOLEAN  Overwrite,
  IN ...
);
// ConfigVSetInteger
/// Set an integer configuration value
/// @param Path      The path of the configuration value
/// @param Integer   The integer configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param Args      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSetInteger (
  IN CHAR16  *Path,
  IN INTN     Integer,
  IN BOOLEAN  Overwrite,
  IN VA_LIST  Args
);

// ConfigSetUnsigned
/// Set an unsigned integer configuration value
/// @param Path      The path of the configuration value
/// @param Unsigned  The unsigned integer configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param ...       The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetUnsigned (
  IN CHAR16  *Path,
  IN UINTN    Unsigned,
  IN BOOLEAN  Overwrite,
  IN ...
);
// ConfigVSetUnsigned
/// Set an unsigned integer configuration value
/// @param Path      The path of the configuration value
/// @param Unsigned  The unsigned integer configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param Args      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSetUnsigned (
  IN CHAR16  *Path,
  IN UINTN    Unsigned,
  IN BOOLEAN  Overwrite,
  IN VA_LIST  Args
);

// ConfigSetString
/// Set a string configuration value
/// @param Path      The path of the configuration value
/// @param String    The string configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param ...       The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetString (
  IN CHAR16  *Path,
  IN CHAR16  *String,
  IN BOOLEAN  Overwrite,
  IN ...
);
// ConfigVSetString
/// Set a string configuration value
/// @param Path      The path of the configuration value
/// @param String    The string configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param Args      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSetString (
  IN CHAR16  *Path,
  IN CHAR16  *String,
  IN BOOLEAN  Overwrite,
  IN VA_LIST  Args
);

// ConfigSetData
/// Set a data configuration value
/// @param Path      The path of the configuration value
/// @param Size      The size of the data configuration value
/// @param Data      The data configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param ...       The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Data is NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetData (
  IN CHAR16  *Path,
  IN UINTN    Size,
  IN VOID    *Data,
  IN BOOLEAN  Overwrite,
  IN ...
);
// ConfigVSetData
/// Set a data configuration value
/// @param Path      The path of the configuration value
/// @param Size      The size of the data configuration value
/// @param Data      The data configuration value to set
/// @param Overwrite Whether to overwrite a value if already present
/// @param Args      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Data is NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSetData (
  IN CHAR16  *Path,
  IN UINTN    Size,
  IN VOID    *Data,
  IN BOOLEAN  Overwrite,
  IN VA_LIST  Args
);

#endif // __CONFIG_LIBRARY_HEADER__

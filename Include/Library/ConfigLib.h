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
/// @return Whether the configuration was parsed successfully or not
/// @retval EFI_INVALID_PARAMETER If Config is NULL or Size is zero
/// @retval EFI_SUCCESS           If the configuration string was parsed successfully
EFI_STATUS
EFIAPI
ConfigParse (
  IN UINTN  Size,
  IN VOID  *Config
);
// ConfigParseXml
/// Parse configuration information from XML document tree
/// @param Tree The XML document tree to parse
/// @return Whether the configuration was parsed successfully or not
/// @retval EFI_INVALID_PARAMETER If Tree is NULL
/// @retval EFI_SUCCESS           If the configuration was parsed successfully
EFI_STATUS
EFIAPI
ConfigParseXml (
  IN XML_TREE *Tree
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
/// @return Whether the configuration values were freed or not
/// @retval EFI_SUCCESS If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigPartialFree (
  IN CHAR16 *Path OPTIONAL
);
// ConfigSPartialFree
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free
/// @param ...  The argument list
/// @return Whether the configuration values were freed or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigSPartialFree (
  IN CHAR16 *Path,
  IN ...
);
// ConfigVSPartialFree
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free
/// @param Args The argument list
/// @return Whether the configuration values were freed or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigVSPartialFree (
  IN CHAR16  *Path,
  IN VA_LIST  Args
);

// ConfigGetList
/// Get a list of names for the children of the configuration path
/// @param Path  The configuration path of which to get child names or NULL for root
/// @param List  On output, the string list of names of children
/// @param Count On output, the count of strings in the list
/// @return Whether the string list was returned or not
/// @retval EFI_INVALID_PARAMETER If List or Count is NULL or *List is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the stirng list
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_ACCESS_DENIED     If the configuration path is protected
/// @retval EFI_SUCCESS           If the string list was returned successfully
EFI_STATUS
EFIAPI
ConfigGetList (
  IN  CHAR16   *Path OPTIONAL,
  OUT CHAR16 ***List,
  OUT UINTN    *Count
);
// ConfigSGetList
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
ConfigSGetList (
  IN  CHAR16   *Path,
  OUT CHAR16 ***List,
  OUT UINTN    *Count,
  IN  ...
);
// ConfigVSGetList
/// Get a list of names for the children of the configuration path
/// @param Path  The configuration path of which to get child names
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
ConfigVSGetList (
  IN  CHAR16    *Path,
  OUT CHAR16  ***List,
  OUT UINTN     *Count,
  IN  VA_LIST    Args
);

// ConfigGetType
/// Get the type of a configuration value
/// @param Path The path of the configuration value of which to get the type
/// @param Type On output, the configuration value type
/// @retval EFI_INVALID_PARAMETER If Path or Type is NULL
/// @retval EFI_NOT_FOUND         If the configuration value was not found
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
EFI_STATUS
EFIAPI
ConfigGetType (
  IN  CHAR16      *Path,
  OUT CONFIG_TYPE *Type
);
// ConfigSGetType
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
ConfigSGetType (
  IN  CHAR16      *Path,
  OUT CONFIG_TYPE *Type,
  IN  ...
);
// ConfigVSGetType
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
ConfigVSGetType (
  IN  CHAR16      *Path,
  OUT CONFIG_TYPE *Type,
  IN  VA_LIST      Args
);

// ConfigGetValue
/// Get a configuration value
/// @param Path  The path of the configuration value
/// @param Type  On output, the type of the configuration value
/// @param Value On output, the value of the configuration value
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
  OUT CONFIG_VALUE *Value
);
// ConfigSGetValue
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
ConfigSGetValue (
  IN  CHAR16       *Path,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  ...
);
// ConfigVSGetValue
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
ConfigVSGetValue (
  IN  CHAR16       *Path,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  VA_LIST       Args
);

// ConfigGetBoolean
/// Get a boolean configuration value
/// @param Path    The path of the configuration value
/// @param Boolean On output, the boolean configuration value
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
  OUT BOOLEAN *Boolean
);
// ConfigSGetBoolean
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
ConfigSGetBoolean (
  IN  CHAR16  *Path,
  OUT BOOLEAN *Boolean,
  IN  ...
);
// ConfigVSGetBoolean
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
ConfigVSGetBoolean (
  IN  CHAR16  *Path,
  OUT BOOLEAN *Boolean,
  IN  VA_LIST  Args
);

// ConfigGetInteger
/// Get an integer configuration value
/// @param Path    The path of the configuration value
/// @param Integer On output, the integer configuration value
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
  OUT INTN   *Integer
);
// ConfigSGetInteger
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
ConfigSGetInteger (
  IN  CHAR16 *Path,
  OUT INTN   *Integer,
  IN  ...
);
// ConfigVSGetInteger
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
ConfigVSGetInteger (
  IN  CHAR16  *Path,
  OUT INTN    *Integer,
  IN  VA_LIST  Args
);

// ConfigGetUnsigned
/// Get an unsigned integer configuration value
/// @param Path    The path of the configuration value
/// @param Unsigned On output, the unsigned integer configuration value
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
  OUT UINTN  *Unsigned
);
// ConfigSGetUnsigned
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
ConfigSGetUnsigned (
  IN  CHAR16 *Path,
  OUT UINTN  *Unsigned,
  IN  ...
);
// ConfigVSGetUnsigned
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
ConfigVSGetUnsigned (
  IN  CHAR16  *Path,
  OUT UINTN   *Unsigned,
  IN  VA_LIST  Args
);

// ConfigGetString
/// Get a string configuration value
/// @param Path   The path of the configuration value
/// @param String On output, the string configuration value
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
  OUT CHAR16 **String
);
// ConfigSGetString
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
ConfigSGetString (
  IN  CHAR16  *Path,
  OUT CHAR16 **String,
  IN  ...
);
// ConfigVSGetString
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
ConfigVSGetString (
  IN  CHAR16   *Path,
  OUT CHAR16  **String,
  IN  VA_LIST   Args
);

// ConfigGetData
/// Get a data configuration value
/// @param Path The path of the configuration value
/// @param Size On output, the data configuration value size in bytes
/// @param Data On output, the data configuration value
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
  OUT VOID   **Data
);
// ConfigSGetData
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
ConfigSGetData (
  IN  CHAR16  *Path,
  OUT UINTN   *Size,
  OUT VOID   **Data,
  IN  ...
);
// ConfigVSGetData
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
ConfigVSGetData (
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
  OUT CONFIG_VALUE *Value
);
// ConfigSGetValueWithDefault
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
ConfigSGetValueWithDefault (
  IN  CHAR16       *Path,
  IN  CONFIG_TYPE   DefaultType,
  IN  CONFIG_VALUE *DefaultValue,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value,
  IN  ...
);
// ConfigVSGetValueWithDefault
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
ConfigVSGetValueWithDefault (
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
/// @return The boolean configuration value
BOOLEAN
EFIAPI
ConfigGetBooleanWithDefault (
  IN CHAR16  *Path,
  IN BOOLEAN  DefaultBoolean
);
// ConfigSGetBooleanWithDefault
/// Get a boolean configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultBoolean The default boolean configuration value
/// @param ...            The argument list
/// @return The boolean configuration value
BOOLEAN
EFIAPI
ConfigSGetBooleanWithDefault (
  IN CHAR16  *Path,
  IN BOOLEAN  DefaultBoolean,
  IN ...
);
// ConfigVSGetBooleanWithDefault
/// Get a boolean configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultBoolean The default boolean configuration value
/// @param Args           The argument list
/// @return The boolean configuration value
BOOLEAN
EFIAPI
ConfigVSGetBooleanWithDefault (
  IN CHAR16  *Path,
  IN BOOLEAN  DefaultBoolean,
  IN VA_LIST  Args
);

// ConfigGetIntegerWithDefault
/// Get an integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultInteger The default integer configuration value
/// @return The integer configuration value
INTN
EFIAPI
ConfigGetIntegerWithDefault (
  IN CHAR16 *Path,
  IN INTN    DefaultInteger
);
// ConfigSGetIntegerWithDefault
/// Get an integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultInteger The default integer configuration value
/// @param ...            The argument list
/// @return The integer configuration value
INTN
EFIAPI
ConfigSGetIntegerWithDefault (
  IN CHAR16 *Path,
  IN INTN    DefaultInteger,
  IN ...
);
// ConfigVSGetIntegerWithDefault
/// Get an integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaultInteger The default integer configuration value
/// @param Args           The argument list
/// @return The integer configuration value
INTN
EFIAPI
ConfigVSGetIntegerWithDefault (
  IN CHAR16  *Path,
  IN INTN     DefaultInteger,
  IN VA_LIST  Args
);

// ConfigGetUnsignedWithDefault
/// Get an unsigned integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaulUnsigned The default unsigned configuration value
/// @return The unsigned integer configuration value
UINTN
EFIAPI
ConfigGetUnsignedWithDefault (
  IN CHAR16 *Path,
  IN UINTN   DefaultUnsigned
);
// ConfigSGetUnsignedWithDefault
/// Get an unsigned integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaulUnsigned The default unsigned configuration value
/// @param ...            The argument list
/// @return The unsigned integer configuration value
UINTN
EFIAPI
ConfigSGetUnsignedWithDefault (
  IN CHAR16 *Path,
  IN UINTN   DefaultUnsigned,
  IN ...
);
// ConfigVSGetUnsignedWithDefault
/// Get an unsigned integer configuration value with default
/// @param Path           The path of the configuration value
/// @param DefaulUnsigned The default unsigned configuration value
/// @param Args           The argument list
/// @return The unsigned integer configuration value
UINTN
EFIAPI
ConfigVSGetUnsignedWithDefault (
  IN CHAR16  *Path,
  IN UINTN    DefaultUnsigned,
  IN VA_LIST  Args
);

// ConfigGetStringWithDefault
/// Get a string configuration value with default
/// @param Path          The path of the configuration value
/// @param DefaultString The default string configuration value
/// @return The string configuration value
CHAR16 *
EFIAPI
ConfigGetStringWithDefault (
  IN CHAR16 *Path,
  IN CHAR16 *DefaultString
);
// ConfigSGetStringWithDefault
/// Get a string configuration value with default
/// @param Path          The path of the configuration value
/// @param DefaultString The default string configuration value
/// @param ...           The argument list
/// @return The string configuration value
CHAR16 *
EFIAPI
ConfigSGetStringWithDefault (
  IN CHAR16 *Path,
  IN CHAR16 *DefaultString,
  IN ...
);
// ConfigVSGetStringWithDefault
/// Get a string configuration value with default
/// @param Path          The path of the configuration value
/// @param DefaultString The default string configuration value
/// @param Args          The argument list
/// @return The string configuration value
CHAR16 *
EFIAPI
ConfigVSGetStringWithDefault (
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
/// @return The data configuration value
VOID *
EFIAPI
ConfigGetDataWithDefault (
  IN  CHAR16 *Path,
  IN  UINTN   DefaultSize,
  IN  VOID   *DefaultData,
  OUT UINTN  *Size
);
// ConfigSGetDataWithDefault
/// Get a data configuration value with default
/// @param Path        The path of the configuration value
/// @param DefaultSize The default data configuration value size
/// @param DefaultData The default data configuration value
/// @param Size        On output, the data configuration value size
/// @param ...         The argument list
/// @return The data configuration value
VOID *
EFIAPI
ConfigSGetDataWithDefault (
  IN  CHAR16 *Path,
  IN  UINTN   DefaultSize,
  IN  VOID   *DefaultData,
  OUT UINTN  *Size,
  IN  ...
);
// ConfigVSGetDataWithDefault
/// Get a data configuration value with default
/// @param Path        The path of the configuration value
/// @param DefaultSize The default data configuration value size
/// @param DefaultData The default data configuration value
/// @param Size        On output, the data configuration value size
/// @param Args        The argument list
/// @return The data configuration value
VOID *
EFIAPI
ConfigVSGetDataWithDefault (
  IN  CHAR16  *Path,
  IN  UINTN    DefaultSize,
  IN  VOID    *DefaultData,
  OUT UINTN   *Size,
  IN  VA_LIST  Args
);

// ConfigSetValue
/// Set a configuration value
/// @param Path  The path of the configuration value
/// @param Type  The configuration type to set
/// @param Value The configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Value is NULL or Type is invalid
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetValue (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value
);
// ConfigSSetValue
/// Set a configuration value
/// @param Path  The path of the configuration value
/// @param Type  The configuration type to set
/// @param Value The configuration value to set
/// @param ...   The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Value is NULL or Type is invalid
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSSetValue (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value,
  IN ...
);
// ConfigVSSetValue
/// Set a configuration value
/// @param Path  The path of the configuration value
/// @param Type  The configuration type to set
/// @param Value The configuration value to set
/// @param Args  The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Value is NULL or Type is invalid
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSSetValue (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value,
  IN VA_LIST       Args
);

// ConfigSetBoolean
/// Set a boolean configuration value
/// @param Path    The path of the configuration value
/// @param Boolean The boolean configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetBoolean (
  IN CHAR16  *Path,
  IN BOOLEAN  Boolean
);
// ConfigSSetBoolean
/// Set a boolean configuration value
/// @param Path    The path of the configuration value
/// @param Boolean The boolean configuration value to set
/// @param ...     The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSSetBoolean (
  IN CHAR16  *Path,
  IN BOOLEAN  Boolean,
  IN ...
);
// ConfigVSSetBoolean
/// Set a boolean configuration value
/// @param Path    The path of the configuration value
/// @param Boolean The boolean configuration value to set
/// @param Args    The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSSetBoolean (
  IN CHAR16  *Path,
  IN BOOLEAN  Boolean,
  IN VA_LIST  Args
);

// ConfigSetInteger
/// Set an integer configuration value
/// @param Path    The path of the configuration value
/// @param Integer The integer configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetInteger (
  IN CHAR16 *Path,
  IN INTN    Integer
);
// ConfigSSetInteger
/// Set an integer configuration value
/// @param Path    The path of the configuration value
/// @param Integer The integer configuration value to set
/// @param ...     The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSSetInteger (
  IN CHAR16 *Path,
  IN INTN    Integer,
  IN ...
);
// ConfigVSSetInteger
/// Set an integer configuration value
/// @param Path    The path of the configuration value
/// @param Integer The integer configuration value to set
/// @param Args    The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSSetInteger (
  IN CHAR16  *Path,
  IN INTN     Integer,
  IN VA_LIST  Args
);

// ConfigSetUnsigned
/// Set an unsigned integer configuration value
/// @param Path     The path of the configuration value
/// @param Unsigned The unsigned integer configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetUnsigned (
  IN CHAR16 *Path,
  IN UINTN   Unsigned
);
// ConfigSSetUnsigned
/// Set an unsigned integer configuration value
/// @param Path     The path of the configuration value
/// @param Unsigned The unsigned integer configuration value to set
/// @param ...      The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSSetUnsigned (
  IN CHAR16 *Path,
  IN UINTN   Unsigned,
  IN ...
);
// ConfigVSSetUnsigned
/// Set an unsigned integer configuration value
/// @param Path     The path of the configuration value
/// @param Unsigned The unsigned integer configuration value to set
/// @param Args     The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSSetUnsigned (
  IN CHAR16  *Path,
  IN UINTN    Unsigned,
  IN VA_LIST  Args
);

// ConfigSetString
/// Set a string configuration value
/// @param Path   The path of the configuration value
/// @param String The string configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetString (
  IN CHAR16 *Path,
  IN CHAR16 *String
);
// ConfigSSetString
/// Set a string configuration value
/// @param Path   The path of the configuration value
/// @param String The string configuration value to set
/// @param ...    The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSSetString (
  IN CHAR16 *Path,
  IN CHAR16 *String,
  IN ...
);
// ConfigVSSetString
/// Set a string configuration value
/// @param Path   The path of the configuration value
/// @param String The string configuration value to set
/// @param Args   The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSSetString (
  IN CHAR16  *Path,
  IN CHAR16  *String,
  IN VA_LIST  Args
);

// ConfigSetData
/// Set a data configuration value
/// @param Path The path of the configuration value
/// @param Size The size of the data configuration value
/// @param Data The data configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Data is NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSetData (
  IN CHAR16 *Path,
  IN UINTN   Size,
  IN VOID   *Data
);
// ConfigSSetData
/// Set a data configuration value
/// @param Path The path of the configuration value
/// @param Size The size of the data configuration value
/// @param Data The data configuration value to set
/// @param ...  The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Data is NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigSSetData (
  IN CHAR16 *Path,
  IN UINTN   Size,
  IN VOID   *Data,
  IN ...
);
// ConfigVSSetData
/// Set a data configuration value
/// @param Path The path of the configuration value
/// @param Size The size of the data configuration value
/// @param Data The data configuration value to set
/// @param Args The argument list
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Data is NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for configuration data
/// @retval EFI_SUCCESS           If the configuration value was set successfully
EFI_STATUS
EFIAPI
ConfigVSSetData (
  IN CHAR16  *Path,
  IN UINTN    Size,
  IN VOID    *Data,
  IN VA_LIST  Args
);

#endif // __CONFIG_LIBRARY_HEADER__

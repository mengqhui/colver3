//
/// @file Include/Library/StringLib.h
///
/// String library
///

#pragma once
#ifndef __STRING_LIBRARY_HEADER__
#define __STRING_LIBRARY_HEADER__

#include <Version.h>

// STR_LIST_NO_DUPLICATES
/// Do not allow duplicate strings in the list
#define STR_LIST_NO_DUPLICATES 0x1
// STR_LIST_CASE_INSENSITIVE
/// Compare strings case insensitively when checking for duplicates in the list, only valid with STR_LIST_NO_DUPLICATES
#define STR_LIST_CASE_INSENSITIVE 0x2
// STR_LIST_SORTED
/// The string list should be sorted
#define STR_LIST_SORTED 0x4
// STR_LIST_INCLUDE_EMPTY
/// The string list can contain empty strings (non-NULL)
#define STR_LIST_INCLUDE_EMPTY 0x8
// STR_SEARCH_CASE_INSENSITIVE
/// Compare strings case insensitively when searching for strings
#define STR_SEARCH_CASE_INSENSITIVE 0x10

// IsUnicodeCharacter
/// Check if character is valid unicode code point
/// @param Character The character to check is valid
/// @retval TRUE  If the character is a valid unicode code point
/// @retval FALSE If the character is invalid in unicode
BOOLEAN
EFIAPI
IsUnicodeCharacter (
  IN UINT32 Character
);

// StrDup
/// Duplicate a string
/// @param Str The string to duplicate
/// @return The duplicated string which needs freed or NULL if the string could not be duplicate
CHAR16 *
EFIAPI
StrDup (
  IN CHAR16 *Str
);
// StrnDup
/// Duplicate a string
/// @param Str   The string to duplicate
/// @param Count The maximum count of characters to duplicate
/// @return The duplicated string which needs freed or NULL if the string could not be duplicate
CHAR16 *
EFIAPI
StrnDup (
  IN CHAR16 *Str,
  IN UINTN   Count
);

// StriCmp
/// Performs a case-insensitive comparison of two strings
/// @param Str1 The first string to comapre
/// @param Str2 The second string to compare
/// @retval 0  Str1 is equivalent to Str2
/// @retval >0 Str1 is lexically greater than Str2
/// @retval <0 Str1 is lexically less than Str2
INTN
EFIAPI
StriCmp (
  IN CHAR16 *Str1,
  IN CHAR16 *Str2
);
// StrniCmp
/// Performs a case-insensitive comparison of two strings
/// @param Str1  The first string to comapre
/// @param Str2  The second string to compare
/// @param Count The maximum count of characters to compare
/// @retval 0  Str1 is equivalent to Str2
/// @retval >0 Str1 is lexically greater than Str2
/// @retval <0 Str1 is lexically less than Str2
INTN
EFIAPI
StrniCmp (
  IN CHAR16 *Str1,
  IN CHAR16 *Str2,
  IN UINTN   Count
);

// StriColl
/// Performs a case-insensitive collation of two strings
/// @param Str1 The first string to collate
/// @param Str2 The second string to collate
/// @retval 0  Str1 is equivalent to Str2
/// @retval >0 Str1 is lexically greater than Str2
/// @retval <0 Str1 is lexically less than Str2
INTN
EFIAPI
StriColl (
  IN CHAR16 *Str1,
  IN CHAR16 *Str2
);
// MetaiMatch
/// Performs a case-insensitive comparison of a pattern string and a string
/// @param Strin   A pointer to a string
/// @param Pattern A pointer to a pattern string
/// @retval TRUE  Pattern was found in String
/// @retval FALSE Pattern was not found in String
BOOLEAN
EFIAPI
MetaiMatch (
  IN CHAR16 *String,
  IN CHAR16 *Pattern
);

// StrLwr
/// Converts all the characters in a string to lower case characters
/// @param Str A pointer to a string
VOID
EFIAPI
StrLwr (
  IN OUT CHAR16 *Str
);
// StrUpr
/// Converts all the characters in a string to upper case characters
/// @param Str A pointer to a string
VOID
EFIAPI
StrUpr (
  IN OUT CHAR16 *Str
);

// StrToLower
/// Duplicates all the characters in a string to lower case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all lower case characters, which must be freed or NULL if there was an error
CHAR16 *
EFIAPI
StrToLower (
  IN CHAR16 *Str
);
// StrToUpper
/// Duplicates all the characters in a string to upper case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all upper case characters, which must be freed or NULL if there was an error
CHAR16 *
EFIAPI
StrToUpper (
  IN CHAR16 *Str
);

// StriStr
/// Returns the first occurrence of a case-insensitive sub-string in a string
/// @param String       A pointer to a string
/// @param SearchString A pointer to a string for which to search
/// @return A pointer to the matched sub-string in String
/// @retval NULL If SearchString was not found in String
CHAR16 *
EFIAPI
StriStr (
  IN CHAR16 *String,
  IN CHAR16 *SearchString
);

// StrAppend
/// Append a character to a string
/// @param String    On input, the string to which to append or NULL to allocate a new string, on output, the possibly reallocated string, which needs freed
/// @param Count     On input, the length count, in characters, of the string, on output, the length count, in characters, of the string
/// @param Size      On input, the allocated size, in characters, of the string, on output, the allocated size, in characters, of the string
/// @param Character The character to append to the string
/// @return Whether the character was appended to the string or not
/// @retval EFI_INVALID_PARAMETER If String, Count, or Size is NULL or Character is invalid
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the string
/// @retval EFI_SUCCESS           If the character was appended to the string successfully
EFI_STATUS
EFIAPI
StrAppend (
  IN OUT CHAR16 **String,
  IN OUT UINTN   *Count,
  IN OUT UINTN   *Size,
  IN     UINT32   Character
);

// StrSplit
/// Split a string into a list
/// @param List    On output, the string list, which must be freed with StrListFree
/// @param Count   On output, the count of strings in the list
/// @param Str     The string to split
/// @param Split   The string that splits
/// @param Options The options for the string list
/// @return Whether the string was split or not
/// @retval EFI_INVALID_PARAMETER If List, Count, Str, or Split is NULL or *List is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the string list
/// @retval EFI_NOT_FOUND         If the split string could not be found
/// @retval EFI_SUCCESS           If the string was split into a list successfully
EFI_STATUS
EFIAPI
StrSplit (
  OUT CHAR16 ***List,
  OUT UINTN    *Count,
  IN  CHAR16   *Str,
  IN  CHAR16   *Split,
  IN  UINTN     Options
);
// StrList
/// Append a string to a list
/// @param List    The list to append the string, which must be freed with StrListFree, *List may be NULL to start new list
/// @param Count   The current count of strings in the list
/// @param Str     The string to append to the list
/// @param Length  The maximum length of the string or zero to use the entire string
/// @param Options The options for adding the strings
/// @return Whether the string was added to the list or not
/// @retval EFI_INVALID_PARAMETER If List, Count, or Str is NULL
/// @retval EFI_OUT_OF_RESOURCES  If the memory for the list could not be allocated
/// @retval EFI_ACCESS_DENIED     If STR_LIST_NO_DUPLICATES was used and the string already exists
/// @retval EFI_SUCCESS           If the string was added to the list successfully
EFI_STATUS
EFIAPI
StrList (
  IN OUT CHAR16 ***List,
  IN OUT UINTN    *Count,
  IN     CHAR16   *Str,
  IN     UINTN     Length,
  IN     UINTN     Options
);
// StrListDup
/// Duplicate a string list
/// @param Duplicate On output, the duplicated string list which must be free with StrListFree
/// @param Count     The count of strings
/// @param List      The string list to duplicate
/// @return Whether the string list was duplicated or not
/// @return EFI_INVALID_PARAMETER If Duplicate or List is NULL or *Duplicate is not NULL or Count is zero
/// @return EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @return EFI_SUCCESS           If the string list was duplicated successfully
EFI_STATUS
EFIAPI
StrListDup (
  OUT CHAR16 ***Duplicate,
  IN  UINTN     Count,
  IN  CHAR16  **List
);
// StrListFree
/// Free a string list
/// @param List  The string list to free
/// @param Count The count of strings in the list
/// @return Whether the string list was freed or not
/// @return EFI_INVALID_PARAMETER If List is NULL or Count is zero
/// @return EFI_SUCCESS           If the string list was freed successfully
EFI_STATUS
EFIAPI
StrListFree (
  IN CHAR16 **List,
  IN UINTN    Count
);

// AsciiStrDup
/// Duplicate a string
/// @param Str The string to duplicate
/// @return The duplicated string which needs freed or NULL if the string could not be duplicate
CHAR8 *
EFIAPI
AsciiStrDup (
  IN CHAR8 *Str
);
// AsciiStrnDup
/// Duplicate a string
/// @param Str   The string to duplicate
/// @param Count The maximum count of characters to duplicate
/// @return The duplicated string which needs freed or NULL if the string could not be duplicate
CHAR8 *
EFIAPI
AsciiStrnDup (
  IN CHAR8 *Str,
  IN UINTN  Count
);

// AsciiStriColl
/// Performs a case-insensitive comparison of two strings
/// @param Str1 The first string to comapre
/// @param Str2 The second string to compare
/// @retval 0  Str1 is equivalent to Str2
/// @retval >0 Str1 is lexically greater than Str2
/// @retval <0 Str1 is lexically less than Str2
INTN
EFIAPI
AsciiStriColl (
  IN CHAR8 *Str1,
  IN CHAR8 *Str2
);
// AsciiMetaiMatch
/// Performs a case-insensitive comparison of a pattern string and a string
/// @param Strin   A pointer to a string
/// @param Pattern A pointer to a pattern string
/// @retval TRUE  Pattern was found in String
/// @retval FALSE Pattern was not found in String
BOOLEAN
EFIAPI
AsciiMetaiMatch (
  IN CHAR8 *String,
  IN CHAR8 *Pattern
);

// AsciiLwr
/// Converts all the characters in a string to lower case characters
/// @param Str A pointer to a string
VOID
EFIAPI
AsciiLwr (
  IN OUT CHAR8 *Str
);
// AsciiUpr
/// Converts all the characters in a string to upper case characters
/// @param Str A pointer to a string
VOID
EFIAPI
AsciiUpr (
  IN OUT CHAR8 *Str
);

// AsciiToLower
/// Duplicates all the characters in a string to lower case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all lower case characters, which must be freed or NULL if there was an error
CHAR8 *
EFIAPI
AsciiToLower (
  IN CHAR8 *Str
);
// AsciiToUpper
/// Duplicates all the characters in a string to upper case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all upper case characters, which must be freed or NULL if there was an error
CHAR8 *
EFIAPI
AsciiToUpper (
  IN CHAR8 *Str
);

// AsciiStrniCmp
/// Performs a case-insensitive comparison of two strings
/// @param Str1  The first string to comapre
/// @param Str2  The second string to compare
/// @param Count The maximum count of characters to compare
/// @retval 0  Str1 is equivalent to Str2
/// @retval >0 Str1 is lexically greater than Str2
/// @retval <0 Str1 is lexically less than Str2
INTN
EFIAPI
AsciiStrniCmp (
  IN CHAR8 *Str1,
  IN CHAR8 *Str2,
  IN UINTN  Count
);

// AsciiStriStr
/// Returns the first occurrence of a case-insensitive sub-string in a string
/// @param String       A pointer to a string
/// @param SearchString A pointer to a string for which to search
/// @return A pointer to the matched sub-string in String
/// @retval NULL If SearchString was not found in String
CHAR8 *
EFIAPI
AsciiStriStr (
  IN CHAR8 *String,
  IN CHAR8 *SearchString
);

// AsciiStrSplit
/// Split a string into a list
/// @param List    On output, the string list, which must be freed with AsciiStrListFree
/// @param Count   On output, the count of strings in the list
/// @param Str     The string to split
/// @param Split   The string that splits
/// @param Options The options for the string list
/// @return Whether the string was split or not
/// @retval EFI_INVALID_PARAMETER If List, Count, Str, or Split is NULL or *List is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the string list
/// @retval EFI_NOT_FOUND         If the split string could not be found
/// @retval EFI_SUCCESS           If the string was split into a list successfully
EFI_STATUS
EFIAPI
AsciiStrSplit (
  OUT CHAR8 ***List,
  OUT UINTN   *Count,
  IN  CHAR8   *Str,
  IN  CHAR8   *Split,
  IN  UINTN    Options
);
// AsciiStrList
/// Append a string to a list
/// @param List    The list to append the string, which must be freed with AsciiStrListFree, *List may be NULL to start new list
/// @param Count   The current count of strings in the list
/// @param Str     The string to append to the list
/// @param Length  The maximum length of the string or zero to use the entire string
/// @param Options The options for adding the strings
/// @return Whether the string was added to the list or not
/// @retval EFI_INVALID_PARAMETER If List, Count, or Str is NULL
/// @retval EFI_OUT_OF_RESOURCES  If the memory for the list could not be allocated
/// @retval EFI_ACCESS_DENIED     If STR_LIST_NO_DUPLICATES was used and the string already exists
/// @retval EFI_SUCCESS           If the string was added to the list successfully
EFI_STATUS
EFIAPI
AsciiStrList (
  IN OUT CHAR8 ***List,
  IN OUT UINTN   *Count,
  IN     CHAR8   *Str,
  IN     UINTN    Length,
  IN     UINTN    Options
);
// AsciiStrListFree
/// Free a string list
/// @param List  The string list to free
/// @param Count The count of strings in the list
/// @return Whether the string list was freed or not
/// @return EFI_INVALID_PARAMETER If List is NULL or Count is zero
/// @return EFI_SUCCESS           If the string list was freed successfully
EFI_STATUS
EFIAPI
AsciiStrListFree (
  IN CHAR8 **List,
  IN UINTN   Count
);

// SetLanguage
/// Set the language
/// @param Language The language code to discover collation protocol or NULL for default english
/// @return Whether the language protocol was changed or not
/// @retval EFI_NOT_FOUND If the language did not match a collation protocol
/// @retval EFI_SUCCESS   If the language was changed
EFI_STATUS
EFIAPI
SetLanguage (
  IN CHAR8 *Language OPTIONAL
);
// GetLanguage
/// Get the language
/// @param Language On output, the language code used to discover collation protocol, which must be freed
/// @return Whether the language was returned or not
/// @retval EFI_INVALID_PARAMETER If Language is NULL or *Language is not NULL
/// @retval EFI_OUT_OF_RESOURCES If memory could not be allocated for the string
/// @retval EFI_SUCCESS          If the language was returned successfully
EFI_STATUS
EFIAPI
GetLanguage (
  OUT CHAR8 **Language
);
// GetLanguages
/// Get the available languages
/// @param Languages On output, languages available for collation
/// @param Count     On output, the count of languages
/// @return Whether the languages available for collation were retrieved or not
/// @retval EFI_INVALID_PARAMETER If Languages or Count is NULL or *Languages is not NULL
/// @retval EFI_NOT_FOUND         If no collation protocol with valid language support could be found
/// @retval EFI_SUCCESS           If the languages were retrieved successfully
EFI_STATUS
EFIAPI
GetLanguages (
  OUT CHAR8 ***Languages,
  OUT UINTN   *Count
);

// ToBase64
/// Convert data to Base64 encoded string
/// @param Size   The size, in bytes, of the data to encode
/// @param Data   The data to encode
/// @param Count  On output, the count of characters encoded
/// @param Base64 On output, the Base64 encoded string, which must be freed
/// @return Whether the data was converted to Base64 or not
/// @retval EFI_INVALID_PARAMETER If Data or Base64 is NULL or *Base64 is not NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the data was converted to Base64 successfully
EFI_STATUS
EFIAPI
ToBase64 (
  IN  UINTN    Size,
  IN  VOID    *Data,
  OUT UINTN   *Count OPTIONAL,
  OUT CHAR16 **Base64
);
// FromBase64
/// Convert Base64 encoded string to data
/// @param Base64 The Base64 encoded string to convert
/// @param Size   On output, the size, in bytes, of the data
/// @param Data   On output, the decoded data, which must be freed
/// @return Whether the Base64 encoded string was converted to data or not
/// @retval EFI_INVALID_PARAMETER If Base64, Size, or Data is NULL or *Data is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the Base64 string was converted to data successfully
EFI_STATUS
EFIAPI
FromBase64 (
  IN  CHAR16  *Base64,
  OUT UINTN   *Size,
  OUT VOID   **Data
);
// AsciiToBase64
/// Convert data to Base64 encoded ASCII string
/// @param Size   The size, in bytes, of the data to encode
/// @param Data   The data to encode
/// @param Count  On output, the count of characters encoded
/// @param Base64 On output, the Base64 encoded ASCII string, which must be freed
/// @return Whether the data was converted to Base64 or not
/// @retval EFI_INVALID_PARAMETER If Data or Base64 is NULL or *Base64 is not NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the data was converted to Base64 successfully
EFI_STATUS
EFIAPI
AsciiToBase64 (
  IN  UINTN   Size,
  IN  VOID   *Data,
  OUT UINTN  *Count OPTIONAL,
  OUT CHAR8 **Base64
);
// AsciiFromBase64
/// Convert Base64 encoded ASCII string to data
/// @param Base64 The Base64 encoded ASCII string to convert
/// @param Size   On output, the size, in bytes, of the data
/// @param Data   On output, the decoded data, which must be freed
/// @return Whether the Base64 encoded ASCII string was converted to data or not
/// @retval EFI_INVALID_PARAMETER If Base64, Size, or Data is NULL or *Data is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the Base64 ASCII string was converted to data successfully
EFI_STATUS
EFIAPI
AsciiFromBase64 (
  IN  CHAR8  *Base64,
  OUT UINTN  *Size,
  OUT VOID  **Data
);

#endif // __STRING_LIBRARY_HEADER__

///
/// @file Library/StringLib/StringLib.c
///
/// String library entry
///

#include "Unicode.h"

#include <Guid/GlobalVariable.h>

#include <Protocol/UnicodeCollation.h>

#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>

// CHAR_FAT_VALID
/// Bit mask to indicate the validity of character in FAT file name
#define CHAR_FAT_VALID 0x01
// MAP_TABLE_SIZE
/// Maximum FAT table size
#define MAP_TABLE_SIZE 0x100
// TO_UPPER
/// Convert character to upper case
#define TO_UPPER(a) (CHAR16)(((a) <= 0xFF) ? mEngUpperMap[a] : (a))
// TO_LOWER
// Convert character to lower case
#define TO_LOWER(a) (CHAR16)(((a) <= 0xFF) ? mEngLowerMap[a] : (a))
// DEFAULT_LANGUAGE
/// Default language - English
#define DEFAULT_LANGUAGE "en"

// mCollationProtocol
/// Unicode collation protocol
STATIC EFI_UNICODE_COLLATION_PROTOCOL *mCollationProtocol = NULL;
// mEngUpperMap
/// Character map used for converting to upper case
STATIC CHAR8  mEngUpperMap[MAP_TABLE_SIZE];
// mEngLowerMap
/// Character map used for converting to lower case
STATIC CHAR8  mEngLowerMap[MAP_TABLE_SIZE];
// mEngInfoMap
/// Character map used for FAT conversion
STATIC CHAR8  mEngInfoMap[MAP_TABLE_SIZE];
// mOtherChars
/// Other characters valid for FAT
STATIC CHAR8  mOtherChars[] = {
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  '\\',
  '.',
  '_',
  '^',
  '$',
  '~',
  '!',
  '#',
  '%',
  '&',
  '-',
  '{',
  '}',
  '(',
  ')',
  '@',
  '`',
  '\'',
  '\0'
};

// IsUnicodeCharacter
/// Check if character is valid unicode code point
/// @param Character The character to check is valid
/// @retval TRUE  If the character is a valid unicode code point
/// @retval FALSE If the character is invalid in unicode
BOOLEAN
EFIAPI
IsUnicodeCharacter (
  IN UINT32 Character
) {
  UINTN Index;
  if (Character == 0) {
    return FALSE;
  }
  for (Index = 0; Index < ARRAY_SIZE(mValidCharRanges); ++Index) {
    if (Character < mValidCharRanges[Index].Start) {
      break;
    }
    if (Character <= mValidCharRanges[Index].End) {
      return TRUE;
    }
  }
  return FALSE;
}

// StrDup
/// Duplicate a string
/// @param Str The string to duplicate
/// @return The duplicated string which needs freed or NULL if the string could not be duplicate
CHAR16 *
EFIAPI
StrDup (
  IN CHAR16 *Str
) {
  CHAR16 *Dup;
  UINTN   Len;
  // Check string is valid
  if (Str == NULL) {
    return NULL;
  }
  // Duplicate memory for string
  Len = StrLen(Str) + 1;
  Dup = AllocateZeroPool(Len * sizeof(CHAR16));
  if (Dup != NULL) {
    // Copy the string to duplicate
    if (EFI_ERROR(StrCpyS(Dup, Len, Str))) {
      FreePool(Dup);
      Dup = NULL;
    }
  }
  // Return duplicate
  return Dup;
}
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
) {
  CHAR16 *Dup;
  UINTN   Len;
  // Check string is valid
  if ((Str == NULL) || (Count == 0)) {
    return NULL;
  }
  // Duplicate memory for string
  Len = StrLen(Str);
  if (Len > Count) {
    Len = Count;
  }
  Dup = AllocateZeroPool(++Len * sizeof(CHAR16));
  if (Dup != NULL) {
    // Copy the string to duplicate
    if (EFI_ERROR(StrnCpyS(Dup, Len, Str, Count))) {
      FreePool(Dup);
      Dup = NULL;
    }
  }
  // Return duplicate
  return Dup;
}

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
) {
  if (Str1 == NULL) {
    if (Str2 == NULL) {
      return 0;
    }
    return -1;
  } else if (Str2 == NULL) {
    return 1;
  }
  // Use collation protocol if present
  if ((mCollationProtocol != NULL) && (mCollationProtocol->StriColl != NULL)) {
    return mCollationProtocol->StriColl(mCollationProtocol, Str1, Str2);
  }
  // Fallback to english collation
  while (*Str1 != 0) {
    if (TO_UPPER(*Str1) != TO_UPPER(*Str2)) {
      break;
    }

    ++Str1;
    ++Str2;
  }
  return TO_UPPER(*Str1) - TO_UPPER(*Str2);
}
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
) {
  // Check parameters
  if (Count == 0) {
    return 0;
  }
  if (Str1 == NULL) {
    if (Str2 == NULL) {
      return 0;
    }
    return -1;
  } else if (Str2 == NULL) {
    return 1;
  }
  while ((*Str1 != L'\0') && (TO_UPPER(*Str1) == TO_UPPER(*Str2)) && (Count-- > 1)) {
    ++Str1;
    ++Str2;
  }
  return TO_UPPER(*Str1) - TO_UPPER(*Str2);
}

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
) {
  if (Str1 == NULL) {
    if (Str2 == NULL) {
      return 0;
    }
    return -1;
  } else if (Str2 == NULL) {
    return 1;
  }
  // Use collation protocol if present
  if ((mCollationProtocol != NULL) && (mCollationProtocol->StriColl != NULL)) {
    INTN Result = mCollationProtocol->StriColl(mCollationProtocol, Str1, Str2);
    if (Result == 0) {
      Result = StrCmp(Str1, Str2);
    }
    return Result;
  }
  // Fallback to english collation
  while (*Str1 != 0) {
    if ((TO_UPPER(*Str1) != TO_UPPER(*Str2)) || (*Str1 != *Str2)) {
      break;
    }

    ++Str1;
    ++Str2;
  }

  return *Str1 - *Str2;
}
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
) {
  CHAR16  CharC;
  CHAR16  CharP;
  CHAR16  Index3;

  if ((String == NULL) || (Pattern == NULL)) {
    return FALSE;
  }

  // Use collation protocol if present
  if ((mCollationProtocol != NULL) && (mCollationProtocol->MetaiMatch != NULL)) {
    return mCollationProtocol->MetaiMatch(mCollationProtocol, String, Pattern);
  }
  // Fallback to english collation
  while ((CharP = *Pattern++) != '\0') {

    switch (CharP) {

    case '*':
      //
      // Match zero or more chars
      //
      while (*String != 0) {
        if (MetaiMatch(String, Pattern)) {
          return TRUE;
        }

        ++String;
      }

      return MetaiMatch(String, Pattern);

    case '?':
      //
      // Match any one char
      //
      if (*String == 0) {
        return FALSE;
      }

      ++String;
      break;

    case '[':
      //
      // Match char set
      //
      CharC = *String;
      if (CharC == 0) {
        //
        // syntax problem
        //
        return FALSE;
      }

      Index3  = 0;
      CharP   = *Pattern++;
      while (CharP != 0) {
        if (CharP == ']') {
          return FALSE;
        }

        if (CharP == '-') {
          //
          // if range of chars, get high range
          //
          CharP = *Pattern;
          if (CharP == 0 || CharP == ']') {
            //
            // syntax problem
            //
            return FALSE;
          }

          if ((TO_UPPER(CharC) >= TO_UPPER(Index3)) && (TO_UPPER(CharC) <= TO_UPPER(CharP))) {
            //
            // if in range, it's a match
            //
            break;
          }
        }

        Index3 = CharP;
        if (TO_UPPER(CharC) == TO_UPPER(CharP)) {
          //
          // if char matches
          //
          break;
        }

        CharP = *Pattern++;
      }
      //
      // skip to end of match char set
      //
      while ((CharP != 0) && (CharP != ']')) {
        CharP = *Pattern++;
      }

      ++String;
      break;

    default:
      CharC = *String;
      if (TO_UPPER(CharC) != TO_UPPER(CharP)) {
        return FALSE;
      }

      ++String;
      break;
    }
  }
  return (*String == L'\0');
}

// StrLwr
/// Converts all the characters in a string to lower case characters
/// @param Str A pointer to a string
VOID
EFIAPI
StrLwr (
  IN OUT CHAR16 *Str
) {
  if (Str == NULL) {
    return;
  }
  // Use collation protocol if present
  if ((mCollationProtocol != NULL) && (mCollationProtocol->StrLwr != NULL)) {
    mCollationProtocol->StrLwr(mCollationProtocol, Str);
  } else {
    // Fallback to english collation
    while (*Str != 0) {
      *Str = TO_LOWER(*Str);
      ++Str;
    }
  }
}
// StrUpr
/// Converts all the characters in a string to upper case characters
/// @param Str A pointer to a string
VOID
EFIAPI
StrUpr (
  IN OUT CHAR16 *Str
) {
  if (Str == NULL) {
    return;
  }
  // Use collation protocol if present
  if ((mCollationProtocol != NULL) && (mCollationProtocol->StrUpr != NULL)) {
    mCollationProtocol->StrUpr(mCollationProtocol, Str);
  } else {
    // Fallback to english collation
    while (*Str != 0) {
      *Str = TO_UPPER(*Str);
      ++Str;
    }
  }
}

// StrToLower
/// Duplicates all the characters in a string to lower case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all lower case characters, which must be freed or NULL if there was an error
CHAR16 *
EFIAPI
StrToLower (
  IN CHAR16 *Str
) {
  CHAR16 *Dup = StrDup(Str);
  if (Dup != NULL) {
    StrLwr(Dup);
  }
  return Dup;
}
// StrToUpper
/// Duplicates all the characters in a string to upper case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all upper case characters, which must be freed or NULL if there was an error
CHAR16 *
EFIAPI
StrToUpper (
  IN CHAR16 *Str
) {
  CHAR16 *Dup = StrDup(Str);
  if (Dup != NULL) {
    StrUpr(Dup);
  }
  return Dup;
}

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
) {
  // Check parameters
  if (String == NULL) {
    return NULL;
  }
  if ((SearchString == NULL) || (*SearchString == L'\0')) {
    return String;
  }

  // Use collation protocol if present
  if (mCollationProtocol != NULL) {
    UINTN StringLen = StrLen(String);
    UINTN SearchLen = StrLen(SearchString);
    INTN  Result;

    // Iterate through string for matches
    while ((StringLen >= SearchLen) && (*String != L'\0')) {
      // Duplicate substring
      CHAR16 *SubString = StrnDup(String, SearchLen);
      if (SubString == NULL) {
        break;
      }
      // Check for match
      Result = StriCmp(SubString, SearchString);
      // Free substring
      FreePool(SubString);
      // Return this substring if match
      if (Result == 0) {
        return String;
      }
      // Advance string and decrement remaining string length
      ++String;
      --StringLen;
    }
  } else {
    // Fallback to english collation

    // Iterate through string for matches
    while (*String != L'\0') {
      CHAR16 *SearchStringTmp = SearchString;
      CHAR16 *FirstMatch = String;

      // Check for match
      while ((TO_UPPER(*String) == TO_UPPER(*SearchStringTmp)) && (*String != L'\0')) {
        ++String;
        ++SearchStringTmp;
      }

      if (*SearchStringTmp == L'\0') {
        // Found
        return FirstMatch;
      }
      if (*String == L'\0') {
        // Not found
        return NULL;
      }

      String = FirstMatch + 1;
    }
  }
  // Not found
  return NULL;
}

// StrAppend
/// Append a character to a string
/// @param String    On input, the string to which to append or NULL to allocate a new string, on output, the possibly reallocated string, which needs freed
/// @param Count     On input, the length count, in characters, of the string, on output, the length count, in characters, of the string
/// @param Size      On input, the allocated size, in characters, of the string, on output, the allocated size, in characters, of the string
/// @param Character The character to append to the string
/// @return Whether the character was appended to the string or not
/// @retval EFI_INVALID_PARAMETER If String, Count, or Size is NULL or Character is invalid or *Count >= *Size
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the string
/// @retval EFI_SUCCESS           If the character was appended to the string successfully
EFI_STATUS
EFIAPI
StrAppend (
  IN OUT CHAR16 **String,
  IN OUT UINTN   *Count,
  IN OUT UINTN   *Size,
  IN     UINT32   Character
) {
  CHAR16 *Str;
  UINTN   NewCount;
  UINTN   NewSize;
  // Check parameters
  if ((String == NULL) || (Count == NULL) || (Size == NULL) || !IsUnicodeCharacter(Character)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the string information
  Str = *String;
  NewCount = *Count;
  NewSize = *Size;
  // Check if bounds are sane
  if (NewCount >= NewSize) {
    NewSize = NewCount = 0;
  }
  // Check if the string needs reallocated
  if ((Str == NULL) || (NewSize == 0) || ((NewSize - NewCount) < 3)) {
    CHAR16 *NewStr;
    // Check the buffer size is valid
    if (NewSize == 0) {
      NewSize = 4;
    }
    // Increase the buffer size
    while (((NewSize <<= 1) - NewCount) < 3);
    // Reallocate the buffer
    NewStr = (CHAR16 *)AllocateZeroPool(NewSize * sizeof(CHAR16));
    if (NewStr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    if (Str != NULL) {
      if (NewCount > 0) {
        CopyMem(NewStr, Str, NewCount * sizeof(CHAR16));
      }
      FreePool(Str);
    }
    *String = Str = NewStr;
    *Size = NewSize;
  }
  // Get offset into string to place character
  Str += NewCount;
  // Check if surrogate pairs needed
  if (Character >= 0x10000) {
    // Append the surrogate pairs
    *Str++ = (0xD800 | ((Character >> 10) & 0x3FF));
    *Str++ = (0xDC00 | (Character & 0x3FF));
    *Str = L'\0';
    *Count = NewCount + 2;
    return EFI_SUCCESS;
  }
  // Append character
  *Str++ = (CHAR16)Character;
  *Str = L'\0';
  *Count = NewCount + 1;
  return EFI_SUCCESS;
}

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
) {
  CHAR16  *NextStr;
  CHAR16 **NewList;
  UINTN    NewCount;
  UINTN    Length;
  // Check parameters
  if ((List == NULL) || (*List != NULL) || (Count == NULL) || (Str == NULL) || (Split == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  Length = StrLen(Split);
  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Length > StrLen(Str)) {
    return EFI_NOT_FOUND;
  }
  // Start new string list
  NewList = NULL;
  NewCount = 0;
  // Iterate through the string
  do {
    // Get next instance of split string
    if ((Options & STR_SEARCH_CASE_INSENSITIVE) != 0) {
      NextStr = StriStr(Str, Split);
    } else {
      NextStr = StrStr(Str, Split);
    }
    // Check if the split string was found
    if (NextStr == NULL) {
      if (NewCount == 0) {
        // No split string found
        return EFI_NOT_FOUND;
      }
      // Last string so done splitting
      StrList(&NewList, &NewCount, Str, 0, Options);
      break;
    }
    // Add string to list
    StrList(&NewList, &NewCount, Str, NextStr - Str, Options);
    // Advance string
    Str = NextStr + Length;
  // If we reach the null terminator then done splitting
  } while (*Str != '\0');

  // Return list and count
  *List = NewList;
  *Count = NewCount;
  return EFI_SUCCESS;
}
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
) {
  CHAR16  *NewStr;
  CHAR16 **NewList;
  CHAR16 **InitialList;
  UINTN    InitialCount;
  UINTN    NewCount;
  UINTN    Index;

  // Check parameters
  if ((List == NULL) || (Count == NULL) || (Str == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the initial list and count
  InitialList = *List;
  InitialCount = (InitialList == NULL) ? 0 : *Count;
  // Check for duplicates if needed
  if ((Options & STR_LIST_NO_DUPLICATES) != 0) {
    // Check case insensitively if needed
    if ((Options & STR_LIST_CASE_INSENSITIVE) != 0) {
      // Check for duplicates case insensitively
      for (Index = 0; Index < InitialCount; ++Index) {
        if (StriCmp(InitialList[Index], Str) == 0) {
          // Already in list
          return EFI_ACCESS_DENIED;
        }
      }
    } else {
      // Check for duplicates
      for (Index = 0; Index < InitialCount; ++Index) {
        if (StrCmp(InitialList[Index], Str) == 0) {
          // Already in list
          return EFI_ACCESS_DENIED;
        }
      }
    }
  }
  // Duplicate the string
  NewStr = (Length == 0) ? StrDup(Str) : StrnDup(Str, Length);
  if (NewStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Reallocate size in the list for one more string
  NewCount = InitialCount + 1;
  NewList = AllocateZeroPool(NewCount * sizeof(CHAR16 *));
  if (NewList == NULL) {
    FreePool(NewStr);
    return EFI_OUT_OF_RESOURCES;
  }
  // Check if the string list is sorted
  if ((Options & STR_LIST_SORTED) != 0) {
    // Determine insertion point with binary search
    UINTN Left = 0;
    UINTN Right = InitialCount;
    UINTN Index = 0;
    while (Left < Right) {
      Index = Left + ((Right - Left) >> 1);
      if (StriColl(InitialList[Index], NewStr) < 0) {
        Left = ++Index;
      } else {
        Right = Index;
      }
    }
    // Copy old list before insertion point
    if (Index > 0) {
      CopyMem((VOID *)NewList, (VOID *)InitialList, Index * sizeof(CHAR16 *));
    }
    // Insert string into list
    NewList[Index] = NewStr;
    // Copy after insertion point
    if (Index < InitialCount) {
      CopyMem((VOID *)(NewList + Index + 1), (VOID *)(InitialList + Index), (InitialCount - Index) * sizeof(CHAR16 *));
    }
  } else {
    // Copy old list
    CopyMem((VOID *)NewList, (VOID *)InitialList, InitialCount * sizeof(CHAR16 *));
    FreePool(InitialList);
    // Append string to list
    NewList[InitialCount] = NewStr;
  }
  // Set new list and count
  *List = NewList;
  *Count = NewCount;
  return EFI_SUCCESS;
}
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
) {
  CHAR16 **NewList;
  UINTN    Index;
  // Check parameters
  if ((Duplicate == NULL) || (List == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate a new list
  NewList = (CHAR16 **)AllocateZeroPool(Count * sizeof(CHAR16 *));
  if (NewList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Duplicate strings
  for (Index = 0; Index < Count; ++Index) {
    NewList[Index] = StrDup(List[Index]);
  }
  // Return duplicate string list
  *Duplicate = NewList;
  return EFI_SUCCESS;
}
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
) {
  UINTN Index;
  // Check parameters
  if ((List == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Free each string
  for (Index = 0; Index < Count; ++Index) {
    FreePool(List[Index]);
  }
  // Free the list
  FreePool(List);
  return EFI_SUCCESS;
}

// AsciiStrDup
/// Duplicate a string
/// @param Str The string to duplicate
/// @return The duplicated string which needs freed or NULL if the string could not be duplicate
CHAR8 *
EFIAPI
AsciiStrDup (
  IN CHAR8 *Str
) {
  CHAR8 *Dup;
  UINTN  Len;
  // Check string is valid
  if (Str == NULL) {
    return NULL;
  }
  // Duplicate memory for string
  Len = AsciiStrLen(Str) + 1;
  Dup = AllocateZeroPool(Len * sizeof(CHAR8));
  if (Dup != NULL) {
    // Copy the string to duplicate
    if (EFI_ERROR(AsciiStrCpyS(Dup, Len, Str))) {
      FreePool(Dup);
      Dup = NULL;
    }
  }
  // Return duplicate
  return Dup;
}
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
) {
  CHAR8 *Dup;
  UINTN  Len;
  // Check string is valid
  if ((Str == NULL) || (Count == 0)) {
    return NULL;
  }
  // Duplicate memory for string
  Len = AsciiStrLen(Str);
  if (Len > Count) {
    Len = Count;
  }
  Dup = AllocateZeroPool(++Len * sizeof(CHAR8));
  if (Dup != NULL) {
    // Copy the string to duplicate
    if (EFI_ERROR(AsciiStrnCpyS(Dup, Len, Str, Count))) {
      FreePool(Dup);
      Dup = NULL;
    }
  }
  // Return duplicate
  return Dup;
}

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
) {
  if (Str1 == NULL) {
    if (Str2 == NULL) {
      return 0;
    }
    return -1;
  } else if (Str2 == NULL) {
    return 1;
  }
  while (*Str1 != 0) {
    if ((mEngUpperMap[*Str1] != mEngUpperMap[*Str2]) || (*Str1 != *Str2)) {
      break;
    }

    ++Str1;
    ++Str2;
  }
  return *Str1 - *Str2;
}
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
) {
  CHAR8 CharC;
  CHAR8 CharP;
  CHAR8 Index3;

  if ((String == NULL) || (Pattern == NULL)) {
    return FALSE;
  }

  while ((CharP = *Pattern++) != '\0') {

    switch (CharP) {

    case '*':
      //
      // Match zero or more chars
      //
      while (*String != 0) {
        if (AsciiMetaiMatch(String, Pattern)) {
          return TRUE;
        }

        ++String;
      }

      return AsciiMetaiMatch(String, Pattern);

    case '?':
      //
      // Match any one char
      //
      if (*String == 0) {
        return FALSE;
      }

      ++String;
      break;

    case '[':
      //
      // Match char set
      //
      CharC = *String;
      if (CharC == 0) {
        //
        // syntax problem
        //
        return FALSE;
      }

      Index3  = 0;
      CharP   = *Pattern++;
      while (CharP != 0) {
        if (CharP == ']') {
          return FALSE;
        }

        if (CharP == '-') {
          //
          // if range of chars, get high range
          //
          CharP = *Pattern;
          if (CharP == 0 || CharP == ']') {
            //
            // syntax problem
            //
            return FALSE;
          }

          if ((mEngUpperMap[CharC] >= mEngUpperMap[Index3]) && (mEngUpperMap[CharC] <= mEngUpperMap[CharP])) {
            //
            // if in range, it's a match
            //
            break;
          }
        }

        Index3 = CharP;
        if (mEngUpperMap[CharC] == mEngUpperMap[CharP]) {
          //
          // if char matches
          //
          break;
        }

        CharP = *Pattern++;
      }
      //
      // skip to end of match char set
      //
      while ((CharP != 0) && (CharP != ']')) {
        CharP = *Pattern;
        ++Pattern;
      }

      ++String;
      break;

    default:
      CharC = *String;
      if (mEngUpperMap[CharC] != mEngUpperMap[CharP]) {
        return FALSE;
      }

      ++String;
      break;
    }
  }
  return (*String == '\0');
}

// AsciiLwr
/// Converts all the characters in a string to lower case characters
/// @param Str A pointer to a string
VOID
EFIAPI
AsciiLwr (
  IN OUT CHAR8 *Str
) {
  if (Str == NULL) {
    return;
  }
  while (*Str != 0) {
    *Str = mEngLowerMap[*Str];
    ++Str;
  }
}
// AsciiUpr
/// Converts all the characters in a string to upper case characters
/// @param Str A pointer to a string
VOID
EFIAPI
AsciiUpr (
  IN OUT CHAR8 *Str
) {
  if (Str == NULL) {
    return;
  }
  while (*Str != 0) {
    *Str = mEngUpperMap[*Str];
    ++Str;
  }
}

// AsciiToLower
/// Duplicates all the characters in a string to lower case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all lower case characters, which must be freed or NULL if there was an error
CHAR8 *
EFIAPI
AsciiToLower (
  IN CHAR8 *Str
) {
  CHAR8 *Dup = AsciiStrDup(Str);
  if (Dup != NULL) {
    AsciiLwr(Dup);
  }
  return Dup;
}
// AsciiToUpper
/// Duplicates all the characters in a string to upper case characters
/// @param Str A pointer to a string
/// @return A duplicate string with all upper case characters, which must be freed or NULL if there was an error
CHAR8 *
EFIAPI
AsciiToUpper (
  IN CHAR8 *Str
) {
  CHAR8 *Dup = AsciiStrDup(Str);
  if (Dup != NULL) {
    AsciiUpr(Dup);
  }
  return Dup;
}

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
) {
  // Check parameters
  if (Count == 0) {
    return 0;
  }
  if (Str1 == NULL) {
    if (Str2 == NULL) {
      return 0;
    }
    return -1;
  } else if (Str2 == NULL) {
    return 1;
  }
  while ((*Str1 != '\0') && (mEngUpperMap[*Str1] == mEngUpperMap[*Str2]) && (Count-- > 1)) {
    ++Str1;
    ++Str2;
  }
  return mEngUpperMap[*Str1] - mEngUpperMap[*Str2];
}

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
) {
  // Check parameters
  if (String == NULL) {
    return NULL;
  }
  if ((SearchString == NULL) || (*SearchString == '\0')) {
    return String;
  }
  // Iterate through string for matches
  while (*String != '\0') {
    CHAR8 *SearchStringTmp = SearchString;
    CHAR8 *FirstMatch = String;

    // Check for match
    while ((mEngUpperMap[*String] == mEngUpperMap[*SearchStringTmp]) && (*String != '\0')) {
      ++String;
      ++SearchStringTmp;
    }

    if (*SearchStringTmp == '\0') {
      // Found
      return FirstMatch;
    }
    if (*String == '\0') {
      // Not found
      return NULL;
    }

    String = FirstMatch + 1;
  }
  // Not found
  return NULL;
}

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
) {
  CHAR8  *NextStr;
  CHAR8 **NewList;
  UINTN   NewCount;
  UINTN   Length;
  // Check parameters
  if ((List == NULL) || (*List != NULL) || (Count == NULL) || (Str == NULL) || (Split == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  Length = AsciiStrLen(Split);
  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (Length > AsciiStrLen(Str)) {
    return EFI_NOT_FOUND;
  }
  // Start new string list
  NewList = NULL;
  NewCount = 0;
  // Iterate through the string
  do {
    // Get next instance of split string
    if ((Options & STR_SEARCH_CASE_INSENSITIVE) != 0) {
      NextStr = AsciiStriStr(Str, Split);
    } else {
      NextStr = AsciiStrStr(Str, Split);
    }
    // Check if the split string was found
    if (NextStr == NULL) {
      if (NewCount == 0) {
        // No split string found
        //return EFI_NOT_FOUND;
      }
      // Last string so done splitting
      AsciiStrList(&NewList, &NewCount, Str, 0, Options);
      break;
    }
    // Add string to list
    AsciiStrList(&NewList, &NewCount, Str, NextStr - Str, Options);
    // Advance string
    Str = NextStr + Length;
  // If we reach the null terminator then done splitting
  } while (*Str != '\0');

  // Return list and count
  *List = NewList;
  *Count = NewCount;
  return EFI_SUCCESS;
}

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
) {
  CHAR8  *NewStr;
  CHAR8 **NewList;
  CHAR8 **InitialList;
  UINTN   InitialCount;
  UINTN   NewCount;
  UINTN   Index;

  // Check parameters
  if ((List == NULL) || (Count == NULL) || (Str == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the initial list and count
  InitialList = *List;
  InitialCount = (InitialList == NULL) ? 0 : *Count;
  // Check for duplicates if needed
  if ((Options & STR_LIST_NO_DUPLICATES) != 0) {
    // Check case insensitively if needed
    if ((Options & STR_LIST_CASE_INSENSITIVE) != 0) {
      // Check for duplicates case insensitively
      for (Index = 0; Index < InitialCount; ++Index) {
        if (AsciiStriCmp(InitialList[Index], Str) == 0) {
          // Already in list
          return EFI_ACCESS_DENIED;
        }
      }
    } else {
      // Check for duplicates
      for (Index = 0; Index < InitialCount; ++Index) {
        if (AsciiStrCmp(InitialList[Index], Str) == 0) {
          // Already in list
          return EFI_ACCESS_DENIED;
        }
      }
    }
  }
  // Duplicate the string
  NewStr = (Length == 0) ? AsciiStrDup(Str) : AsciiStrnDup(Str, Length);
  if (NewStr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Reallocate size in the list for one more string
  NewCount = InitialCount + 1;
  NewList = AllocateZeroPool(NewCount * sizeof(CHAR8 *));
  if (NewList == NULL) {
    FreePool(NewStr);
    return EFI_OUT_OF_RESOURCES;
  }
  // Check if the string list is sorted
  if ((Options & STR_LIST_SORTED) != 0) {
    // Determine insertion point with binary search
    UINTN Left = 0;
    UINTN Right = InitialCount;
    UINTN Index = 0;
    while (Left < Right) {
      Index = Left + ((Right - Left) >> 1);
      if (AsciiStriColl(InitialList[Index], NewStr) < 0) {
        Left = ++Index;
      } else {
        Right = Index;
      }
    }
    // Copy old list before insertion point
    if (Index > 0) {
      CopyMem((VOID *)NewList, (VOID *)InitialList, Index * sizeof(CHAR8 *));
    }
    // Insert string into list
    NewList[Index] = NewStr;
    // Copy after insertion point
    if (Index < InitialCount) {
      CopyMem((VOID *)(NewList + Index + 1), (VOID *)(InitialList + Index), (InitialCount - Index) * sizeof(CHAR8 *));
    }
  } else {
    // Copy old list
    CopyMem((VOID *)NewList, (VOID *)InitialList, InitialCount * sizeof(CHAR8 *));
    FreePool(InitialList);
    // Append string to list
    NewList[InitialCount] = NewStr;
  }
  // Set new list and count
  *List = NewList;
  *Count = NewCount;
  return EFI_SUCCESS;
}
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
) {
  UINTN Index;
  // Check parameters
  if ((List == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Free each string
  for (Index = 0; Index < Count; ++Index) {
    FreePool(List[Index]);
  }
  // Free the list
  FreePool(List);
  return EFI_SUCCESS;
}

// ToAscii
/// Convert string to ASCII
/// @param String The string to convert
/// @return The converted string or NULL if there was an error
CHAR8 *
EFIAPI
ToAscii (
  IN CHAR16 *String
) {
  UINTN  Length;
  CHAR8 *Str;
  // Check parameters
  if (String == NULL) {
    return NULL;
  }
  // Get string length
  Length = StrLen(String);
  if (Length == 0) {
    return NULL;
  }
  // Allocate new string
  Str = (CHAR8 *)AllocateZeroPool((Length + 1) * sizeof(CHAR8));
  if (Str != NULL) {
    // Convert the string
    if (EFI_ERROR(UnicodeStrToAsciiStrS(String, Str, Length + 1))) {
      FreePool(Str);
      Str = NULL;
    }
  }
  return Str;
}
// FromAscii
/// Convert string from ASCII
/// @param String The string to convert
/// @return The converted string or NULL if there was an error
CHAR16 *
EFIAPI
FromAscii (
  IN CHAR8 *String
) {
  UINTN   Length;
  CHAR16 *Str;
  // Check parameters
  if (String == NULL) {
    return NULL;
  }
  // Get string length
  Length = AsciiStrLen(String);
  if (Length == 0) {
    return NULL;
  }
  // Allocate new string
  Str = (CHAR16 *)AllocateZeroPool((Length + 1) * sizeof(CHAR16));
  if (Str != NULL) {
    // Convert the string
    if (EFI_ERROR(AsciiStrToUnicodeStrS(String, Str, Length + 1))) {
      FreePool(Str);
      Str = NULL;
    }
  }
  return Str;
}

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
) {
  EFI_STATUS  Status;
  UINT32      Options = EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS;
  CHAR8      *PlatformLanguages = NULL;
  CHAR8      *RequestLanguage;
  CHAR8      *LanguageCode;
  // Get the language to set
  if ((Language != NULL) && (AsciiStrLen(Language) > 0)) {
    RequestLanguage = Language;
  } else {
    RequestLanguage = DEFAULT_LANGUAGE;
    Language = NULL;
  }
  // Get the supported platform languages
  Status = GetEfiGlobalVariable2(L"PlatformLangCodes", (VOID **)&PlatformLanguages, NULL);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if(PlatformLanguages == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get the best language code
  LanguageCode = GetBestLanguage(PlatformLanguages, FALSE, RequestLanguage, NULL);
  FreePool(PlatformLanguages);
  if (LanguageCode == NULL) {
    return EFI_NOT_FOUND;
  }
  // Set platform language
  Status = gRT->SetVariable(L"PlatformLang", &gEfiGlobalVariableGuid, Options, AsciiStrSize(LanguageCode), (VOID *)LanguageCode);
  FreePool(LanguageCode);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Search for collation protocol
  if ((Language != NULL) && (gBS != NULL) && (gBS->HandleProtocol != NULL) && (gBS->LocateHandleBuffer != NULL)) {
    EFI_UNICODE_COLLATION_PROTOCOL *Collation;
    EFI_HANDLE                     *Handles = NULL;
    UINTN                           Count = 0;
    UINTN                           Index;
    // Locate collation protocols that give language codes in RFC 4646
    if (!EFI_ERROR(gBS->LocateHandleBuffer(ByProtocol, &gEfiUnicodeCollation2ProtocolGuid, NULL, &Count, &Handles)) && (Handles != NULL)) {
      // On first pass try to exact match 
      for (Index = 0; Index < Count; ++Index) {
        // Get each collation protocol
        Collation = NULL;
        if (EFI_ERROR(gBS->HandleProtocol(Handles[Index], &gEfiUnicodeCollation2ProtocolGuid, (VOID **)&Collation))) {
          continue;
        }
        // Check the collation protocol has valid language support
        if ((Collation == NULL) || (Collation->SupportedLanguages == NULL)) {
          continue;
        }
        // Check if this protocol supports this language
        LanguageCode = GetBestLanguage(Collation->SupportedLanguages, FALSE, RequestLanguage, NULL);
        if (LanguageCode == NULL) {
          continue;
        }
        FreePool(LanguageCode);
        mCollationProtocol = Collation;
        break;
      }
    }
  } else {
    // Set default collation protocol
    mCollationProtocol = NULL;
  }
  return EFI_SUCCESS;
}
// GetLanguage
/// Get the language
/// @param Language On output, the language code used to discover collation protocol, which must be freed
/// @return Whether the language was returned or not
/// @retval EFI_INVALID_PARAMETER If Language is NULL or *Language is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the string
/// @retval EFI_SUCCESS           If the language was returned successfully
EFI_STATUS
EFIAPI
GetLanguage (
  OUT CHAR8 **Language
) {
  CHAR8 *PlatformLanguage = NULL;
  if ((Language == NULL) || (*Language != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the language from the platform
  if (!EFI_ERROR(GetEfiGlobalVariable2(L"PlatformLang", (VOID **)&PlatformLanguage, NULL)) && (PlatformLanguage != NULL)) {
    *Language = PlatformLanguage;
  } else {
    // Get the default language
    *Language = AsciiStrDup(DEFAULT_LANGUAGE);
  }
  return (*Language == NULL) ? EFI_OUT_OF_RESOURCES : EFI_SUCCESS;
}
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
) {
  EFI_STATUS   Status = EFI_SUCCESS;
  CHAR8      **AvailableLanguages = NULL;
  CHAR8       *PlatformLanguages = NULL;
  UINTN        LanguageCount = 0;
  if ((Languages == NULL) || (*Languages != NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the languages from the platform
  if (!EFI_ERROR(GetEfiGlobalVariable2(L"PlatformLangCodes", (VOID **)&PlatformLanguages, NULL)) && (PlatformLanguages != NULL)) {
    Status = AsciiStrSplit(&AvailableLanguages, &LanguageCount, PlatformLanguages, ";", STR_LIST_NO_DUPLICATES | STR_LIST_CASE_INSENSITIVE | STR_LIST_SORTED);
    FreePool(PlatformLanguages);
  }
  if (!EFI_ERROR(Status)) {
    // Get the default language
    Status = AsciiStrList(&AvailableLanguages, &LanguageCount, DEFAULT_LANGUAGE, 0, STR_LIST_NO_DUPLICATES | STR_LIST_CASE_INSENSITIVE | STR_LIST_SORTED);
    // Check if default language already exists in list
    if (Status == EFI_ACCESS_DENIED) {
      Status = EFI_SUCCESS;
    }
  }
  // Return languages
  *Languages = AvailableLanguages;
  *Count = LanguageCount;
  return Status;
}

// StringLibInitialize
/// String  library initialize use
/// @return Whether the library initialized successfully or not
/// @retval EFI_SUCCESS The library successfully initialized
EFI_STATUS
EFIAPI
StringLibInitialize (
  VOID
) {
  UINTN Index;
  UINTN Index2;

  // Initialize unicode collation for english as fallback
  for (Index = 0; Index < MAP_TABLE_SIZE; ++Index) {
    mEngUpperMap[Index] = (CHAR8)Index;
    mEngLowerMap[Index] = (CHAR8)Index;
    mEngInfoMap[Index]  = 0;

    if (((Index >= 'a') && (Index <= 'z')) || ((Index >= 0xe0) && (Index <= 0xf6)) || ((Index >= 0xf8) && (Index <= 0xfe))) {

      Index2 = (Index - 0x20);
      mEngUpperMap[Index] = (CHAR8)Index2;
      mEngLowerMap[Index2] = (CHAR8)Index;

      mEngInfoMap[Index] |= CHAR_FAT_VALID;
      mEngInfoMap[Index2] |= CHAR_FAT_VALID;
    }
  }
  for (Index = 0; mOtherChars[Index] != 0; Index++) {
    Index2 = mOtherChars[Index];
    mEngInfoMap[Index2] |= CHAR_FAT_VALID;
  }

  return EFI_SUCCESS;
}

// StringLibFinish
/// String library finish use
/// @return Whether the library was finished successfully or not
/// @retval EFI_SUCCESS The library successfully finished
EFI_STATUS
EFIAPI
StringLibFinish (
  VOID
) {
  return EFI_SUCCESS;
}

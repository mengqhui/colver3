///
/// @file Library/StringLib/Base64.c
///
/// String library Base64
///

#include "Unicode.h"

// mBase64Dict
/// Base64 dictionary
STATIC CHAR8 mBase64Dict[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// IsBase64Character
/// Check if character is Base64 character
/// @param Character The character to check is Base64
/// @retval TRUE  The character is Base64
/// @retval FALSE The character is not Base64
STATIC BOOLEAN
EFIAPI
IsBase64Character (
  IN UINT32 Character
) {
  return (((Character >= 'A') && (Character <= 'Z')) ||
          ((Character >= 'a') && (Character <= 'z')) ||
          ((Character >= '0') && (Character <= '9')) ||
          (Character == '+') || (Character == '/'));
}
// FromBase64Character
/// Get 6bit value from 8bit encoded character
/// @param Character The character to decode
/// @return The decoded 6bit value
STATIC UINT32
EFIAPI
FromBase64Character (
  IN UINT32 Character
) {
  UINT32 Result = 0;
  if ((Character >= 'A') && (Character <= 'Z')) {
    Result = (Character - 'A');
  } else if ((Character >= 'a') && (Character <= 'z')) {
    Result = ((Character - 'a') + 26);
  } else if ((Character >= '0') && (Character <= '9')) {
    Result = ((Character - '0') + 52);
  } else if (Character == '+') {
    Result = 62;
  } else if (Character == '/') {
    Result = 63;
  }
  return Result;
}

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
) {
  CHAR16 *Str;
  CHAR16 *Result;
  UINT8  *Ptr;
  UINTN   Length;
  UINTN   Offset;
  UINTN   Index;
  // Check parameters
  if ((Data == NULL) || (Base64 == NULL) || (*Base64 != NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the length of the encoded string
  Length =  (((Size + 2) / 3 ) * 4);
  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate a buffer for the string
  Result = (CHAR16 *)AllocateZeroPool((Length + 1) * sizeof(CHAR16));
  if (Result == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Check if the data is multiple of three 
  Offset = Size % 3;
  Size -= Offset;
  // Convert the data to Base64
  Index = 0;
  Str = Result;
  Ptr = (UINT8 *)Data;
  while (Index < Size) {
    UINT8 Tmp;
    *Str++ = mBase64Dict[(Ptr[Index] >> 2) & 0x3F];
    Tmp = (Ptr[Index++] & 0x03) << 4;
    *Str++ = mBase64Dict[Tmp | ((Ptr[Index] >> 4) & 0x0F)];
    Tmp = (Ptr[Index++] & 0x0F) << 2;
    *Str++ = mBase64Dict[Tmp | ((Ptr[Index] >> 6) & 0x03)];
    *Str++ = mBase64Dict[Ptr[Index++] & 0x3F];
  }
  // Add padding
  if (Offset == 1) {
    *Str++ = mBase64Dict[(Ptr[Index] >> 2) & 0x3F];
    *Str++ = mBase64Dict[(Ptr[Index] & 0x03) << 4];
    *Str++ = '=';
    *Str++ = '=';
  } else if (Offset == 2) {
    UINT8 Tmp;
    *Str++ = mBase64Dict[(Ptr[Index] >> 2) & 0x3F];
    Tmp = (Ptr[Index++] & 0x03) << 4;
    *Str++ = mBase64Dict[Tmp | ((Ptr[Index] >> 4) & 0x0F)];
    *Str++ = mBase64Dict[(Ptr[Index] & 0x0F) << 2];
    *Str++ = '=';
  }
  *Str = '\0';
  // Return Base64 encoded string and count if requested
  if (Count != NULL) {
    *Count = Length;
  }
  *Base64 = Result;
  return EFI_SUCCESS;
}
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
) {
  UINTN   DataSize;
  UINTN   Length;
  UINTN   Offset;
  UINTN   Index;
  VOID   *Buffer;
  UINT8  *Ptr;
  CHAR16 *NewBase64;
  CHAR16 *String;
  CHAR16 *Str;
  UINT32  Character;
  // Check parameters
  if ((Base64 == NULL) || (Size == NULL) || (Data == NULL) || (*Data != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the string length
  Length = 0;
  Offset = 0;
  Str = Base64;
  while (*Str != '\0') {
    if (*Str == '=') {
      ++Offset;
    } else if (IsBase64Character(*Str)) {
      ++Length;
      Offset = 0;
    }
    ++Str;
  }
  if (Offset > 2) {
    Offset = 0;
  }
  Length -= (4 - Offset);
  Offset = (3 - Offset);
  if ((Length == 0) || ((Length % 4) != 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Make a copy of the corrected Base64
  NewBase64 = (CHAR16 *)AllocateZeroPool((Length + 1) * sizeof(CHAR16));
  if (NewBase64 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Str = Base64;
  String = NewBase64;
  while (*Str != '\0') {
    Character = *Str++;
    if (IsBase64Character(Character)) {
      *String++ = (CHAR16)Character;
    }
  }
  *String = '\0';
  // Get the size needed and add padding to align to 16 bytes
  DataSize = (((Length + Offset + 3) / 4) * 3);
  DataSize += (0x10 - (DataSize & 0x7));
  // Allocate the data buffer
  Buffer = AllocateZeroPool(DataSize);
  if (Buffer == NULL) {
    FreePool(NewBase64);
    return EFI_OUT_OF_RESOURCES;
  }
  // Iterate through the encoded string to decode the data
  Index = 0;
  Str = NewBase64;
  Ptr = (UINT8 *)Buffer;
  while (Index < Length) {
    *Ptr = ((FromBase64Character(Str[Index++]) & 0x3F) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr++ |= ((Character >> 4) & 0x03);
    *Ptr = ((Character & 0xF) << 4);
    Character = FromBase64Character(Str[Index++]);
    *Ptr++ |= ((Character >> 2) & 0xF);
    *Ptr = ((Character & 0x3) << 6);
    *Ptr++ |= (FromBase64Character(Str[Index++]) & 0x3F);
  }
  // Decode the extra offset bytes
  if (Offset == 1) {
    *Ptr = ((FromBase64Character(Str[Index++]) & 0x3F) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr |= ((Character >> 4) & 0x03);
  } else if (Offset == 2) {
    *Ptr = ((FromBase64Character(Str[Index++]) & 0x3F) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr++ |= ((Character >> 4) & 0x03);
    *Ptr = ((Character & 0x0F) << 4);
    Character = FromBase64Character(Str[Index++]);
    *Ptr |= ((Character >> 2) & 0xF);
  }
  FreePool(NewBase64);
  // Return the data
  *Size = (((Length / 4) * 3) + Offset);
  *Data = Buffer;
  return EFI_SUCCESS;
}
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
) {
  CHAR8 *Str;
  CHAR8 *Result;
  UINT8 *Ptr;
  UINTN  Length;
  UINTN  Offset;
  UINTN  Index;
  // Check parameters
  if ((Data == NULL) || (Base64 == NULL) || (*Base64 != NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the length of the encoded string
  Length =  (((Size + 2) / 3 ) * 4);
  if (Length == 0) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate a buffer for the string
  Result = (CHAR8 *)AllocateZeroPool((Length + 1) * sizeof(CHAR8));
  if (Result == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Check if the data is multiple of three 
  Offset = Size % 3;
  Size -= Offset;
  // Convert the data to Base64
  Index = 0;
  Str = Result;
  Ptr = (UINT8 *)Data;
  while (Index < Size) {
    UINT8 Tmp;
    *Str++ = mBase64Dict[(Ptr[Index] >> 2) & 0x3F];
    Tmp = (Ptr[Index++] & 0x03) << 4;
    *Str++ = mBase64Dict[Tmp | ((Ptr[Index] >> 4) & 0x0F)];
    Tmp = (Ptr[Index++] & 0x0F) << 2;
    *Str++ = mBase64Dict[Tmp | ((Ptr[Index] >> 6) & 0x03)];
    *Str++ = mBase64Dict[Ptr[Index++] & 0x3F];
  }
  // Add padding
  if (Offset == 1) {
    *Str++ = mBase64Dict[(Ptr[Index] >> 2) & 0x3F];
    *Str++ = mBase64Dict[(Ptr[Index] & 0x03) << 4];
    *Str++ = '=';
    *Str++ = '=';
  } else if (Offset == 2) {
    UINT8 Tmp;
    *Str++ = mBase64Dict[(Ptr[Index] >> 2) & 0x3F];
    Tmp = (Ptr[Index++] & 0x03) << 4;
    *Str++ = mBase64Dict[Tmp | ((Ptr[Index] >> 4) & 0x0F)];
    *Str++ = mBase64Dict[(Ptr[Index++] & 0x0F) << 2];
    *Str++ = '=';
  }
  *Str = '\0';
  // Return Base64 encoded string and count if requested
  if (Count != NULL) {
    *Count = Length;
  }
  *Base64 = Result;
  return EFI_SUCCESS;
}
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
) {
  UINTN   DataSize;
  UINTN   Length;
  UINTN   Offset;
  UINTN   Index;
  VOID   *Buffer;
  UINT8  *Ptr;
  CHAR8  *NewBase64;
  CHAR8  *String;
  CHAR8  *Str;
  UINT32  Character;
  // Check parameters
  if ((Base64 == NULL) || (Size == NULL) || (Data == NULL) || (*Data != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get the string length
  Length = 0;
  Offset = 0;
  Str = Base64;
  while (*Str != '\0') {
    if (*Str == '=') {
      ++Offset;
    } else if (IsBase64Character(*Str)) {
      ++Length;
      Offset = 0;
    }
    ++Str;
  }
  if (Offset > 2) {
    Offset = 0;
  }
  Length -= (4 - Offset);
  Offset = (3 - Offset);
  if ((Length == 0) || ((Length % 4) != 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Make a copy of the corrected Base64
  NewBase64 = (CHAR8 *)AllocateZeroPool((Length + 1) * sizeof(CHAR8));
  if (NewBase64 == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Str = Base64;
  String = NewBase64;
  while (*Str != '\0') {
    Character = *Str++;
    if (IsBase64Character(Character)) {
      *String++ = (CHAR8)Character;
    }
  }
  *String = '\0';
  // Get the size needed and add padding to align to 16 bytes
  DataSize = (((Length + 3) / 4) * 3);
  DataSize += (0x10 - (DataSize & 0x7));
  // Allocate the data buffer
  Buffer = AllocateZeroPool(DataSize);
  if (Buffer == NULL) {
    FreePool(NewBase64);
    return EFI_OUT_OF_RESOURCES;
  }
  // Iterate through the encoded string to decode the data
  Index = 0;
  Str = NewBase64;
  Ptr = (UINT8 *)Buffer;
  while (Index < Length) {
    *Ptr = ((FromBase64Character(Str[Index++]) & 0x3F) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr++ |= ((Character >> 4) & 0x03);
    *Ptr = ((Character & 0x3C) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr++ |= ((Character >> 2) & 0xF);
    *Ptr = ((Character & 0x3) << 6);
    *Ptr++ |= (FromBase64Character(Str[Index++]) & 0x3F);
  }
  // Decode the extra offset bytes
  if (Offset == 1) {
    *Ptr = ((FromBase64Character(Str[Index++]) & 0x3F) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr |= ((Character >> 4) & 0x03);
  } else if (Offset == 2) {
    *Ptr = ((FromBase64Character(Str[Index++]) & 0x3F) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr++ |= ((Character >> 4) & 0x03);
    *Ptr = ((Character & 0x3C) << 2);
    Character = FromBase64Character(Str[Index++]);
    *Ptr |= ((Character >> 2) & 0xF);
  }
  FreePool(NewBase64);
  // Return the data
  *Size = (((Length / 4) * 3) + Offset);
  *Data = Buffer;
  return EFI_SUCCESS;
}

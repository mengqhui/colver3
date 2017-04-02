//
/// @file Library/ParseLib/ParseLib.c
///
/// Parse library
///

#include <Library/ParseLib.h>

#include <Library/PlatformLib.h>

// LANG_MESSAGE_ERROR
/// Language error message
#define LANG_MESSAGE_ERROR 1
// LANG_MESSAGE_WARNING
/// Language warning message
#define LANG_MESSAGE_WARNING 2

// LANG_MESSAGE
/// Language message
typedef struct _LANG_MESSAGE LANG_MESSAGE;
struct _LANG_MESSAGE {

  // Next
  /// The next message
  LANG_MESSAGE *Next;
  // Flags
  /// The message flags
  UINTN         Flags;
  // Message
  /// The message
  CHAR16       *Message;

};
// LANG_LIST
/// Language state list
typedef struct _LANG_LIST LANG_LIST;
struct _LANG_LIST {

  // Previous
  /// The previous state
  LANG_LIST  *Previous;
  // State
  /// The state
  LANG_STATE *State;

};
// LANG_RULE
/// Language state rule
struct _LANG_RULE {

  // Callback
  /// Token parsed callback
  LANG_CALLBACK   Callback;
  // Options
  /// The options of this language state rule
  UINTN           Options;
  // NextRule
  /// The identifier of the language state to which to change when this rule is satisfied
  UINTN           NextState;
  // Count
  /// The count of token strings in the set
  UINTN           Count;
  // Tokens
  /// The set of token strings
  CHAR16        **Tokens;

};
// LANG_STATE
/// Language state
struct _LANG_STATE {

  // Callback
  /// Token parsed callback
  LANG_CALLBACK   Callback;
  // Id
  /// The language state identifier
  UINTN           Id;
  // Options
  /// The language state options
  UINTN           Options;
  // Count
  /// The count of language state rules
  UINTN           Count;
  // States
  /// The language state rules
  LANG_RULE     **Rules;

};
// LANG_PARSER
/// Language parser
struct _LANG_PARSER {

  // DecodeCount
  /// Decoded character expected remaining character count
  UINT32          DecodeCount;
  // DecodedCharacter
  /// Decoded character
  UINT32          DecodedCharacter;
  // TokenCount
  /// The current parsed token count of characters
  UINTN           TokenCount;
  // TokenSize
  /// The current parsed token maximum count of characters, including null-terminator
  UINTN           TokenSize;
  // Token
  /// The current parsed token
  CHAR16         *Token;
  // Callback
  /// Token parsed callback
  LANG_CALLBACK   Callback;
  // State
  /// The current parser state
  LANG_STATE     *State;
  // PreviousStates
  /// The previous parser states
  LANG_LIST      *PreviousStates;
  // Count
  /// The count of parser states
  UINTN           Count;
  // States
  /// The parser states
  LANG_STATE    **States;
  // Messages
  /// The parser messages
  LANG_MESSAGE   *Messages;

};

// ParseMessage
/// Add a parser message
/// @param Parser      The language parser to which to add a message
/// @param Flags       The message flags
/// @param MessageDesc The description of the message
/// @return Whether the message was added or not
/// @retval EFI_INVALID_PARAMETER If Parser or MessageDesc is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the message was added successfully
EFI_STATUS
EFIAPI
ParseMessage (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Flags,
  IN     CHAR16      *MessageDesc,
  ...
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, MessageDesc);
  Status = ParseVMessage(Parser, Flags, MessageDesc, Args);
  VA_END(Args);
  return Status;
}
// ParseVMessage
/// Add a parser message
/// @param Parser      The language parser to which to add a message
/// @param Flags       The message flags
/// @param MessageDesc The description of the message
/// @param Args        The arguments
/// @return Whether the message was added or not
/// @retval EFI_INVALID_PARAMETER If Parser or MessageDesc is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the message was added successfully
EFI_STATUS
EFIAPI
ParseVMessage (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Flags,
  IN     CHAR16      *MessageDesc,
  IN     VA_LIST      Args
) {
  LANG_MESSAGE *Message;
  // Check parameters
  if ((Parser == NULL) || (MessageDesc == NULL) || (*MessageDesc == L'\0')) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate a new message
  Message = (LANG_MESSAGE *)AllocateZeroPool(sizeof(LANG_MESSAGE));
  if (Message == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set message
  Message->Next = NULL;
  Message->Flags = Flags;
  Message->Message = CatVSPrint(NULL, MessageDesc, Args);
  if (Message->Message == NULL) {
    FreePool(Message);
    return EFI_OUT_OF_RESOURCES;
  }
  // Add the message
  if (Parser->Messages == NULL) {
    Parser->Messages = Message;
  } else {
    LANG_MESSAGE *Ptr = Parser->Messages;
    while (Ptr->Next != NULL) {
      Ptr = Ptr->Next;
    }
    Ptr->Next = Message;
  }
  return EFI_SUCCESS;
}

// ParseError
/// Add a parser error
/// @param Parser    The language parser to which to add an error
/// @param ErrorDesc The description of the error
/// @return Whether the error message was added or not
/// @retval EFI_INVALID_PARAMETER If Parser or ErrorDesc is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the error message was added successfully
EFI_STATUS
EFIAPI
ParseError (
  IN OUT LANG_PARSER *Parser,
  IN     CHAR16      *ErrorDesc,
  ...
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, ErrorDesc);
  Status = ParseVError(Parser, ErrorDesc, Args);
  VA_END(Args);
  return Status;
}
// ParseVError
/// Add a parser error
/// @param Parser    The language parser to which to add an error
/// @param ErrorDesc The description of the error
/// @param Args      The arguments
/// @return Whether the error message was added or not
/// @retval EFI_INVALID_PARAMETER If Parser or ErrorDesc is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the error message was added successfully
EFI_STATUS
EFIAPI
ParseVError (
  IN OUT LANG_PARSER *Parser,
  IN     CHAR16      *ErrorDesc,
  IN     VA_LIST      Args
) {
  return ParseVMessage(Parser, LANG_MESSAGE_ERROR, ErrorDesc, Args);
}

// ParseWarn
/// Add a parser warning
/// @param Parser   The language parser to which to add a warning
/// @param WarnDesc The description of the warning
/// @return Whether the warning message was added or not
/// @retval EFI_INVALID_PARAMETER If Parser or ErrorDesc is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the warning message was added successfully
EFI_STATUS
EFIAPI
ParseWarn (
  IN OUT LANG_PARSER *Parser,
  IN     CHAR16      *WarnDesc,
  ...
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, WarnDesc);
  Status = ParseVWarn(Parser, WarnDesc, Args);
  VA_END(Args);
  return Status;
}
// ParseVWarn
/// Add a parser warning
/// @param Parser   The language parser to which to add a warning
/// @param WarnDesc The description of the warning
/// @param Args     The arguments
/// @return Whether the warning message was added or not
/// @retval EFI_INVALID_PARAMETER If Parser or ErrorDesc is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the warning message was added successfully
EFI_STATUS
EFIAPI
ParseVWarn (
  IN OUT LANG_PARSER *Parser,
  IN     CHAR16      *WarnDesc,
  IN     VA_LIST      Args
) {
  return ParseVMessage(Parser, LANG_MESSAGE_WARNING, WarnDesc, Args);
}

// FindParseState
/// Find parser state by identifier
/// @param Parser The language parser
/// @param Id     The identifier of the parser state to find
/// @return The parser state or NULL if there was an error or the parser state was not found
STATIC LANG_STATE *
EFIAPI
FindParseState (
  IN LANG_PARSER *Parser,
  IN UINTN        Id
) {
  UINTN Index;
  // Check parameters
  if (Parser == NULL) {
    return NULL;
  }
  for (Index = 0; Index < Parser->Count; ++Index) {
    if (Parser->States[Index] != NULL) {
      // Check if identifier matches
      if (Parser->States[Index]->Id == Id) {
        // Found initial state
        return Parser->States[Index];
      }
    }
  }
  return NULL;
}
// SetNextParseState
/// Set the next parse state
/// @param Parser 
/// @param Id     
/// @param Rule   
/// @return
STATIC EFI_STATUS
EFIAPI
SetNextParseState (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Id,
  IN     BOOLEAN      Push
) {
  LANG_LIST *List;
  // Check if popping previous state
  if (Id == LANG_STATE_PREVIOUS) {
    List = Parser->PreviousStates;
    if (List != NULL) {
      // Check if pushing current state
      if (Push) {
        // Swap states on the top of the stack
        LANG_STATE *State = List->State;
        List->State = Parser->State;
        Parser->State = State;
      } else {
        // Pop previous state
        Parser->PreviousStates = List->Previous;
        Parser->State = List->State;
        FreePool(List);
      }
    } else {
      // No previous state
      Parser->State = NULL;
    }
  } else {
    //  Check if pushing current state
    if (Push) {
      List = (LANG_LIST *)AllocateZeroPool(sizeof(LANG_LIST));
      if (List == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      List->State = Parser->State;
      List->Previous = Parser->PreviousStates;
      Parser->PreviousStates = List;
    }
    // Change parser state
    Parser->State = FindParseState(Parser, Id);
  }
  // Check parser state is valid
  if (Parser->State != NULL) {
    return EFI_SUCCESS;
  }
  // State not found
  return EFI_NOT_FOUND;
}
// ParseCheckRules
/// Check whether a rule matching is satisfied
/// @param Parser  The language parser used for parsing
/// @param Count   The count of language rules
/// @param Rules   The language rules to check if satisfied
/// @param Context The parse context
/// @return Whether the rule matching was satisfied or not
/// @retval EFI_INVALID_PARAMETER If Parser or Rule is NULL or internally invalid
/// @retval EFI_NOT_FOUND         If the rule matching was not satisfied
/// @retval EFI_SUCCESS           If the rule matching was satisfied
STATIC EFI_STATUS
EFIAPI
ParseCheckRules (
  IN OUT LANG_PARSER  *Parser,
  IN     UINTN         Count,
  IN     LANG_RULE   **Rules,
  IN     VOID         *Context OPTIONAL
) {
  EFI_STATUS Status;
  LANG_RULE *Rule;
  UINTN      Index;
  UINTN      RuleIndex;
  UINTN      MatchRule = (UINTN)-1;
  UINTN      MatchIndex = (UINTN)-1;
  UINTN      MatchOffset = (UINTN)-1;
  UINTN      MatchLength = 0;
  // The token comparison function
  CHAR16 *(EFIAPI *Search) (CHAR16 *, CHAR16 *);
  INTN (EFIAPI *Compare)(CHAR16 *, CHAR16 *, UINTN);
  // Check parameters
  if ((Parser == NULL) || (Rules == NULL) || (Count == 0) ||
      (Parser->Token == NULL) || (Parser->TokenCount == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Iterate through all the rules
  for (RuleIndex = 0; RuleIndex < Count; ++RuleIndex) {
    Rule = Rules[RuleIndex];
    if (Rule == NULL) {
      continue;
    }
    // Set the comparison function
    if ((Rule->Options & LANG_RULE_INSENSITIVE) != 0) {
      Search = StriStr;
      Compare = StrniCmp;
    } else {
      Search = StrStr;
      Compare = StrnCmp;
    }
    // Check if the token matches
    for (Index = 0; Index < Rule->Count; ++Index) {
      CHAR16 *Ptr;
      UINTN   Offset;
      UINTN   Length = StrLen(Rule->Tokens[Index]);
      // Try to find a match of this token somewhere in the parser token
      Ptr = Search(Parser->Token, Rule->Tokens[Index]);
      if (Ptr == NULL) {
        // Check if the rule allows tokens before match
        if ((Compare(Parser->Token, Rule->Tokens[Index], Parser->TokenCount) == 0) ||
            (((Rule->Options & LANG_RULE_TOKEN) != 0) && (MatchOffset > Parser->TokenCount))) {
          // Assume this is a valid token in the future
          MatchRule = RuleIndex;
          MatchIndex = Index;
          MatchLength = 0;
          MatchOffset = Parser->TokenCount;
        }
      } else {
        // Found a match
        Offset = (UINTN)(Ptr - Parser->Token);
        if ((MatchOffset > Offset) || ((MatchOffset == Offset) && (Length > MatchLength))) {
          MatchRule = RuleIndex;
          MatchIndex = Index;
          MatchLength = Length;
          MatchOffset = Offset;
        }
      }
    }
  }
  // Check if a rule was matched
  if (MatchRule < Count) {
    Rule = Rules[MatchRule];
    // Check if token was matched
    if ((Rule != NULL) && (MatchIndex < Rule->Count)) {
      CHAR16        *Match;
      CHAR16        *Token;
      LANG_CALLBACK  Callback;
      // Check if this is a token before a match
      if ((MatchLength == 0) || ((MatchOffset + MatchLength) >= Parser->TokenCount)) {
        return EFI_SUCCESS;
      }
      // Check if this is a previous state pop
      if ((Rule->Options & LANG_RULE_POP) != 0) {
        // Set previous parser state
        Status = SetNextParseState(Parser, Rule->NextState, ((Rule->Options & LANG_RULE_PUSH) != 0));
        if (EFI_ERROR(Status)) {
          return Status;
        }
      }
      // Set callback
      if (Rule->Callback != NULL) {
        Callback = Rule->Callback;
      } else if (Parser->State->Callback != NULL) {
        Callback = Parser->State->Callback;
      } else {
        Callback = Parser->Callback;
      }
      if (Callback == NULL) {
        return EFI_NOT_READY;
      }
      // Get the current token from the parser token
      Token = StrnDup(Parser->Token, MatchOffset);
      if ((MatchOffset != 0) && (Token == NULL)) {
        return EFI_OUT_OF_RESOURCES;
      }
      // Get the mismatch token from the parser state
      Match = StrnDup(Parser->Token + MatchOffset, MatchLength);
      if (Match == NULL) {
        FreePool(Token);
        return EFI_OUT_OF_RESOURCES;
      }
      // Change parser token to the part that belongs to the next token
      Parser->TokenCount -= (MatchOffset + MatchLength);
      CopyMem(Parser->Token, Parser->Token + (MatchOffset + MatchLength), Parser->TokenCount);
      ZeroMem(Parser->Token + Parser->TokenCount, MatchOffset + MatchLength);
      // Callback for token
      if ((MatchOffset != 0) && ((Rule->Options & LANG_RULE_SKIP_TOKEN) == 0)) {
        Status = Callback(Parser, Parser->State->Id, Token, Context);
        if (EFI_ERROR(Status)) {
          if (Status == EFI_NOT_READY) {
            ParseError(Parser, L"Unexpected termination \"%s\"", Parser, Token);
          } else if (Status == EFI_NOT_FOUND) {
            ParseError(Parser, L"Unexpected token \"%s\"", Parser, Token);
          }
          FreePool(Token);
          FreePool(Match);
          return Status;
        }
      }
      FreePool(Token);
      // Callback for match
      if (((Rule->Options & LANG_RULE_SKIP) == 0) &&
          (((Rule->Options & LANG_RULE_SKIP_EMPTY) == 0) || (MatchOffset > 0))) {
        Status = Callback(Parser, Parser->State->Id, Match, Context);
        if (EFI_ERROR(Status)) {
          if (Status == EFI_NOT_READY) {
            ParseError(Parser, L"Unexpected termination \"%s\"", Match);
          } else if (Status == EFI_NOT_FOUND) {
            ParseError(Parser, L"Unexpected token \"%s\"", Match);
          }
          FreePool(Match);
          return Status;
        }
      }
      FreePool(Match);
      // Check if this is a previous state pop
      if (((Rule->Options & LANG_RULE_POP) != 0) && (Rule->NextState == LANG_STATE_PREVIOUS)) {
        // The state change already happened
        return EFI_SUCCESS;
      }
      // Set next parser state
      return SetNextParseState(Parser, Rule->NextState, ((Rule->Options & LANG_RULE_PUSH) != 0));
    }
  }
  // Rule match not found
  return EFI_NOT_FOUND;
}

// ParseCharacter
/// Parse a character
/// @param Parser    The language parser to use in parsing
/// @param Character The next character to parse
/// @param Context   The parse context
/// @return Whether the character was parsed or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL or Character is invalid
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the character was parsed successfully
EFI_STATUS
EFIAPI
ParseCharacter (
  IN OUT LANG_PARSER *Parser,
  IN     UINT32       Character,
  IN     VOID        *Context OPTIONAL
) {
  EFI_STATUS Status;
  // Check parameters
  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Check langage parser is valid
  if (Parser->State == NULL) {
    ParseError(Parser, L"Invalid parser state");
    return EFI_NOT_FOUND;
  }
  if ((Parser->State->Rules == NULL) || (Parser->State->Count == 0)) {
    ParseError(Parser, L"Invalid parser state");
    return EFI_NOT_FOUND;
  }
  // Append the character to the token
  Status = StrAppend(&(Parser->Token), &(Parser->TokenCount), &(Parser->TokenSize), Character);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if ((Parser->Token == NULL) || (Parser->TokenCount == 0)) {
    ParseError(Parser, L"Invalid token");
    return EFI_INVALID_PARAMETER;
  }
  // Check each rule
  return ParseCheckRules(Parser, Parser->State->Count, Parser->State->Rules, Context);
}

// DecodeSurrogates
/// Decode surrogate pair
/// @param Parser    The language parser
/// @param Character On input, the initial character point, On output, the decoded code point
/// @retval TRUE  If the character was invalid or used as a first surrogate in a pair
/// @retval FALSE If the code point was decoded
STATIC BOOLEAN
EFIAPI
DecodeSurrogates (
  IN OUT LANG_PARSER *Parser,
  IN OUT UINT32      *Character
) {
  if ((*Character >= 0xD800) && (*Character < 0xDC00)) {
    // High surrogate
    Parser->DecodedCharacter = (*Character & 0x3FF);
    Parser->DecodeCount = 1;
    return TRUE;
  } else if ((*Character >= 0xDC00) && (*Character < 0xE000)) {
    // Low surrogate
    if (Parser->DecodeCount != 1) {
      Parser->DecodedCharacter = 0;
      Parser->DecodeCount = 0;
      return TRUE;
    }
    // Decode charcter
    Parser->DecodeCount = 0;
    Parser->DecodedCharacter <<= 10;
    Parser->DecodedCharacter |= (*Character & 0x3FF);
    *Character = (Parser->DecodedCharacter + 0x100000);
    Parser->DecodedCharacter = 0;
  } else {
    // Reset decoding
    Parser->DecodedCharacter = 0;
    Parser->DecodeCount = 0;
  }
  return FALSE;
}
// Parse
/// Parse a UTF-16 string for tokens
/// @param Parser    The language parser to use in parsing
/// @param Count     The count of characters in the string or NULL if the string is null-terminated
/// @param String    The string to parse
/// @param SwapBytes Whether to swap the byte order of each character
/// @param Context   The parse context
/// @return Whether the string was parsed or not
/// @retval EFI_INVALID_PARAMETER If Parser or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the string was parsed successfully
EFI_STATUS
EFIAPI
Parse (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Count OPTIONAL,
  IN     CHAR16      *String,
  IN     BOOLEAN      SwapBytes,
  IN     VOID        *Context OPTIONAL
) {
  EFI_STATUS Status = EFI_SUCCESS;
  // Check parameters
  if ((Parser == NULL) || (String == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get count from null-terminated string
  if (Count == 0) {
    Count = StrLen(String);
  }
  if (Count == 0) {
    return EFI_SUCCESS;
  }
  // Check if bytes need swapped
  if (SwapBytes) {
    // Iterate through buffer
    while ((*String != '\0') && (Count-- > 0)) {
      // Parse each character and swap byte order
      UINT32 Character = (UINT32)(((*String >> 8) & 0x0F) | ((*String << 8) & 0xF0));
      ++String;
      if (DecodeSurrogates(Parser, &Character)) {
        continue;
      }
      // Parse character
      Status = ParseCharacter(Parser, Character, Context);
      if (EFI_ERROR(Status)) {
        break;
      }
    }
  } else {
    // Iterate through buffer
    while ((*String != '\0') && (Count-- > 0)) {
      // Parse each character
      UINT32 Character = (UINT32)*String++;
      if (DecodeSurrogates(Parser, &Character)) {
        continue;
      }
      // Parse character
      Status = ParseCharacter(Parser, Character, Context);
      if (EFI_ERROR(Status)) {
        break;
      }
    }
  }
  return Status;
}
// ParseUtf8
/// Parse a UTF-8 string for tokens
/// @param Parser   The language parser to use in parsing
/// @param Count    The count of characters in the string or NULL if the string is null-terminated
/// @param String   The string to parse
/// @param Encoding The encoding of the string or NULL for UTF-8
/// @param Context  The parse context
/// @return Whether the string was parsed or not
/// @retval EFI_INVALID_PARAMETER If Parser or String is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_UNSUPPORTED       If the encoding is unsupported
/// @retval EFI_SUCCESS           If the string was parsed successfully
EFI_STATUS
EFIAPI
ParseEncoding (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Count OPTIONAL,
  IN     CHAR8       *String,
  IN     CHAR8       *Encoding OPTIONAL,
  IN     VOID        *Context OPTIONAL
) {
  EFI_STATUS Status = EFI_SUCCESS;
  // Check parameters
  if ((Parser == NULL) || (String == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get count from null-terminated string
  if (Count == 0) {
    Count = AsciiStrLen(String);
  }
  if (Count == 0) {
    return EFI_SUCCESS;
  }
  // Decode character from buffer
  if ((Encoding == NULL) || (AsciiStriCmp(Encoding, "UTF-8") == 0)) {
    // UTF-8
    while ((*String != '\0') && (Count-- > 0)) {
      // Iterate through buffer
      UINT32 Character = (UINT32)*String++;
      if ((Character & 0x80) == 0) {
        // ASCII character
        Parser->DecodedCharacter = 0;
        Parser->DecodeCount = 0;
      } else if ((Character & 0x40) != 0) {
        // Trailing character
        if (Parser->DecodeCount == 0) {
          // Invalid character sequence
          Parser->DecodedCharacter = 0;
          continue;
        }
        // Decode character part
        Parser->DecodedCharacter <<= 6;
        Parser->DecodedCharacter |= (Character & 0x3F);
        // If there are still characters left then decode them
        if (--(Parser->DecodeCount) > 0) {
          continue;
        }
        // Character decoded so parse it
        Character = Parser->DecodedCharacter;
        Parser->DecodedCharacter = 0;
      } else {
        CHAR16 Counter = 0x20;
        // Header character
        Parser->DecodeCount = 1;
        while ((Counter != 0) && ((Character & Counter) != 0)) {
          ++(Parser->DecodeCount);
          Counter >>= 1;
        }
        if (Parser->DecodeCount > 6) {
          // Invalid character sequence
          Parser->DecodedCharacter = 0;
          Parser->DecodeCount = 0;
          continue;
        }
        // Decode initial character sequence
        Parser->DecodedCharacter = (Character & (--Counter));
        continue;
      }
      // Parse each character
      Status = ParseCharacter(Parser, Character, Context);
      if (EFI_ERROR(Status)) {
        break;
      }
    }
  } else if ((AsciiStriCmp(Encoding, "ISO-8859-1") == 0) ||
             (AsciiStriCmp(Encoding, "ISO-Latin-1") == 0)) {
    // Latin-1
    while ((*String != '\0') && (Count-- > 0)) {
      // Parse each character
      Status = ParseCharacter(Parser, (UINT32)*String++, Context);
      if (EFI_ERROR(Status)) {
        break;
      }
    }
  } else if (AsciiStriCmp(Encoding, "ASCII") == 0) {
    // ASCII
    while ((*String != '\0') && (Count-- > 0)) {
      // Parse each character
      Status = ParseCharacter(Parser, (UINT32)(*String++ & 0x7F), Context);
      if (EFI_ERROR(Status)) {
        break;
      }
    }
  } else {
    return EFI_UNSUPPORTED;
  }
  return Status;
}
// ParseBuffer
/// Parse a buffer for tokens
/// @param Parser   The language parser to use in parsing
/// @param Size     The size, in bytes, of the buffer
/// @param Buffer   The buffer to parse
/// @param Encoding The encoding of the buffer or NULL to try detecting the encoding
/// @param Context  The parse context
/// @return Whether the buffer was parsed or not
/// @retval EFI_INVALID_PARAMETER If Parser or Buffer is NULL or Size is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_BAD_BUFFER_SIZE   If the buffer size is not a multiple of the encoding character size
/// @retval EFI_UNSUPPORTED       If the encoding of the buffer could not be determined
/// @retval EFI_SUCCESS           If the buffer was parsed successfully
EFI_STATUS
EFIAPI
ParseBuffer (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Size,
  IN     VOID        *Buffer,
  IN     CHAR8       *Encoding OPTIONAL,
  IN     VOID        *Context OPTIONAL
) {
  BOOLEAN SwapBytes = FALSE;
  // Check parameters
  if ((Parse == NULL) || (Buffer == NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Check for encoding
  if (Encoding != NULL) {
    // Check for UTF-16 encoding
    if (AsciiStrniCmp(Encoding, "UTF-16", 6) == 0) {
      // Check to make sure buffer size is good
      if ((Size % sizeof(CHAR16)) != 0) {
        return EFI_BAD_BUFFER_SIZE;
      }
      // Check if bytes need swapped
      if (AsciiStrCmp(Encoding + 6, "LE") == 0) {
        // Little endian
        SwapBytes = IsCPUBigEndian();
      } else if (AsciiStrCmp(Encoding + 6, "BE") == 0) {
        // Big endian
        SwapBytes = IsCPULittleEndian();
      } else {
        // NOTE: Maybe check if encoding endianness matches system here?
        return EFI_UNSUPPORTED;
      }
      // Parse UTF-16 string
      return Parse(Parser, Size / sizeof(CHAR16), (CHAR16 *)Buffer, SwapBytes, Context);
    }
    // Check to make sure buffer size is good
    if ((Size % sizeof(CHAR8)) != 0) {
      return EFI_BAD_BUFFER_SIZE;
    }
    // Parse encoded string
    return ParseEncoding(Parser, Size / sizeof(CHAR8), (CHAR8 *)Buffer, Encoding, Context);
  }
  // Try to determine UTF-16 from byte order mark
  switch (*((CHAR16 *)Buffer)) {
    case 0xFFEF:
      // Parse UTF-16 string with byte order swapped
      SwapBytes = TRUE;

    case 0xFEFF:
      // Check to make sure buffer size is good
      if ((Size % sizeof(CHAR16)) != 0) {
        return EFI_BAD_BUFFER_SIZE;
      }
      // Parse UTF-16 string
      return Parse(Parser, Size / sizeof(CHAR16), (CHAR16 *)Buffer, SwapBytes, Context);

    default:
      break;
  }
  // Try to determine UTF-8 from byte order mark
  if (Size >= (sizeof(CHAR8) * 3)) {
    CHAR8 *Ptr = (CHAR8 *)Buffer;
    if ((*Ptr++ == 0xEF) && (*Ptr++ == 0xBB) && (*Ptr++ == 0xBF)) {
      // Check to make sure buffer size is good
      if ((Size % sizeof(CHAR8)) != 0) {
        return EFI_BAD_BUFFER_SIZE;
      }
      // Parse encoded string
      return ParseEncoding(Parser, Size / sizeof(CHAR8), (CHAR8 *)Buffer, "UTF-8", Context);
    }
  }
  // NOTE: Detection failed because no byte order mark - assume UTF-8 or Latin-1 here?
  return ParseEncoding(Parser, Size / sizeof(CHAR8), (CHAR8 *)Buffer, "ASCII", Context);
}

// DuplicateParseRule
/// Duplicate parser state rule
/// @param The parser state rule to duplicate
/// @return The duplicated parser state rule or NULL if there was an error
STATIC LANG_RULE *
EFIAPI
DuplicateParseRule (
  IN LANG_RULE *Rule
) {
  LANG_RULE *Duplicate;
  // Check parameters
  if (Rule == NULL) {
    return NULL;
  }
  // Allocate new rule
  Duplicate = (LANG_RULE *)AllocateZeroPool(sizeof(LANG_RULE));
  if (Duplicate != NULL) {
    // Duplicate the rule
    Duplicate->Callback = Rule->Callback;
    Duplicate->Options = Rule->Options;
    Duplicate->NextState = Rule->NextState;
    Duplicate->Tokens = NULL;
    StrListDup(&(Duplicate->Tokens), Duplicate->Count = Rule->Count, Rule->Tokens);
  }
  return Duplicate;
}
// DuplicateParseRules
/// Duplicate parser state rules
/// @param Count The count of parser state rules
/// @param Rules The parser state rules to duplicate
/// @return The duplicated parser state rules or NULL if there was an error
STATIC LANG_RULE **
EFIAPI
DuplicateParseRules (
  IN UINTN       Count,
  IN LANG_RULE **Rules
) {
  LANG_RULE **List;
  // Check parameters
  if ((Rules == NULL) || (Count == 0)) {
    return NULL;
  }
  // Allocate new list for rules
  List = (LANG_RULE **)AllocateZeroPool(Count * sizeof(LANG_RULE *));
  if (List != NULL) {
    UINTN Index;
    // Duplicate rules
    for (Index = 0; Index < Count; ++Index) {
      List[Index] = DuplicateParseRule(Rules[Index]);
    }
  }
  return List;
}

// CreateParseRule
/// Create parser state rule
/// @param Rule      On output, the created state rule
/// @param Callback  The token parsed callback
/// @param Options   The parser state rule matching options
/// @param NextState The state to which to change if this rule is satisfied
/// @param Count     The count of tokens
/// @param Tokens    The token set used for matching
/// @return Whether the parser state rule was created or not
/// @retval EFI_INVALID_PARAMETER If Rule or Tokens is NULL or *Rule is not NULL or Count is zero
/// @retval EFI_OUT_OF_MEMORY     If memory could not allocated for the parser state rule
/// @retval EFI_SUCCESS           If the parser state rule was created successfully
EFI_STATUS
EFIAPI
CreateParseRule (
  OUT LANG_RULE     **Rule,
  IN  LANG_CALLBACK   Callback OPTIONAL,
  IN  UINTN           Options,
  IN  UINTN           NextState,
  IN  UINTN           Count,
  IN  CHAR16        **Tokens
) {
  EFI_STATUS  Status;
  LANG_RULE  *Ptr;
  // Check parameters
  if ((Rule == NULL) || (*Rule != NULL) || (Tokens == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate state rule
  Ptr = (LANG_RULE *)AllocateZeroPool(sizeof(LANG_RULE));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Ptr->Callback = Callback;
  Ptr->Options = Options;
  Ptr->NextState = NextState;
  // Duplicate tokens
  Ptr->Tokens = NULL;
  Status = StrListDup(&(Ptr->Tokens), Ptr->Count = Count, Tokens);
  if (EFI_ERROR(Status)) {
    FreeParseRule(Ptr);
    return Status;
  }
  // Return state rule
  *Rule = Ptr;
  return EFI_SUCCESS;
}
// FreeParseRule
/// Free parser state rule
/// @param Rule The parser state rule to free
/// @return Whether the parser state rule was freed or not
/// @retval EFI_INVALID_PARAMETER If Rule is NULL
/// @retval EFI_SUCCESS           If the parser state rule was freed successfully
EFI_STATUS
EFIAPI
FreeParseRule (
  IN LANG_RULE *Rule
) {
  // Check parameters
  if (Rule == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Rule->Tokens != NULL) {
    StrListFree(Rule->Tokens, Rule->Count);
    Rule->Tokens = NULL;
  }
  Rule->Count = 0;
  FreePool(Rule);
  return EFI_SUCCESS;
}
// FreeParseRules
/// Free parser state rules
/// @param Count The count of state rules to free
/// @param Rules The parser state rules to free
/// @return Whether the parser state rules were freed or not
/// @retval EFI_INVALID_PARAMETER If Rules is NULL or Count is zero
/// @retval EFI_SUCCESS           If the parser state rules were freed successfully
EFI_STATUS
EFIAPI
FreeParseRules (
  IN UINTN       Count,
  IN LANG_RULE **Rules
) {
  UINTN Index;
  // Check parameters
  if ((Rules == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  for (Index = 0; Index < Count; ++Index) {
    if (Rules[Index] != NULL) {
      FreeParseRule(Rules[Index]);
    }
  }
  FreePool(Rules);
  return EFI_SUCCESS;
}

// DuplicateParseState
/// Duplicate parser state
/// @param State The parser state to duplicate
/// @return The duplicated parser state or NULL if there was an error
LANG_STATE *
EFIAPI
DuplicateParseState (
  IN LANG_STATE *State
) {
  LANG_STATE *Duplicate;
  if (State == NULL) {
    return NULL;
  }
  Duplicate = (LANG_STATE *)AllocateZeroPool(sizeof(LANG_STATE));
  if (Duplicate != NULL) {
    Duplicate->Id = State->Id;
    Duplicate->Callback = State->Callback;
    Duplicate->Rules = DuplicateParseRules(Duplicate->Count = State->Count, State->Rules);
  }
  return Duplicate;
}
// DuplicateParseStates
/// Duplicate parser states
/// @param Count  
/// @param States 
/// @return The duplicates parser states or NULL if there was an error
STATIC LANG_STATE **
EFIAPI
DuplicateParseStates (
  IN UINTN        Count,
  IN LANG_STATE **States
) {
  LANG_STATE **Duplicate;
  // Check parameters
  if ((States == NULL) || (Count == 0)) {
    return NULL;
  }
  // Allocate new states list
  Duplicate = (LANG_STATE **)AllocateZeroPool(Count * sizeof(LANG_STATE *));
  if (Duplicate != NULL) {
    UINTN Index;
    // Duplicate the states
    for (Index = 0; Index < Count; ++Index) {
      Duplicate[Index] = DuplicateParseState(States[Index]);
    }
  }
  return Duplicate;
}
// CreateParseState
/// Create parser state
/// @param State   On output, the created parser state
/// @param Callback  The token parsed callback
/// @param Id      The identifier of this parser state
/// @param Count   The count of parser state rules
/// @param Rules   The parser state rules to use for this state
/// @return Whether the parser state was created or not
/// @retval EFI_INVALID_PARAMETER If State or Rules is NULL or *State is not NULL or Count is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the state
/// @retval EFI_SUCCESS           If the parser state was created successfully
EFI_STATUS
EFIAPI
CreateParseState (
  OUT LANG_STATE    **State,
  IN  LANG_CALLBACK   Callback OPTIONAL,
  IN  UINTN           Id,
  IN  UINTN           Count,
  IN  LANG_RULE     **Rules
) {
  LANG_STATE *Ptr;
  // Check parameters
  if ((State == NULL) || (*State != NULL) || (Rules == NULL) ||
      (Count == 0) || (Id == LANG_STATE_PREVIOUS)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate parser state
  Ptr = (LANG_STATE *)AllocateZeroPool(sizeof(LANG_STATE));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set parser state identifier
  Ptr->Id = Id;
  Ptr->Callback = Callback;
  // Duplicate parser state rules
  Ptr->Rules = DuplicateParseRules(Ptr->Count = Count, Rules);
  // Return the parser state
  *State = Ptr;
  return EFI_SUCCESS;
}
// CreateParseStates
/// Create parser states from static states
/// @param States       On output, the created parser states, which must be freed with FreeParseStates
/// @param Count        The count of parser states
/// @param StaticStates The static parser states
/// @return Whether the parser states were created or not
/// @retval EFI_INVALID_PARAMETER If States or StaticStates is NULL or *States is not NULL or Count is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the states
/// @retval EFI_SUCCESS           If the parser states were created successfully
EFI_STATUS
EFIAPI
CreateParseStates (
  OUT LANG_STATE        ***States,
  IN  UINTN                Count,
  IN  LANG_STATIC_STATE   *StaticStates
) {
  EFI_STATUS   Status;
  LANG_STATE **Ptr;
  UINTN        Index;
  // Check parameters
  if ((States == NULL) || (StaticStates == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate parser states array
  Ptr = (LANG_STATE **)AllocateZeroPool(Count * sizeof(LANG_STATE *));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Create parser states
  for (Index = 0; Index < Count; ++Index) {
    LANG_RULE **Rules;
    UINTN       RuleIndex;
    // Allocate buffer for the state rules
    Rules = (LANG_RULE **)AllocateZeroPool(StaticStates[Index].Count * sizeof(LANG_RULE *));
    if (Rules == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    // Create parser state rules
    for (RuleIndex = 0; RuleIndex < StaticStates[Index].Count; ++RuleIndex) {
      // Create each parser state rule
      LANG_STATIC_RULE *Rule = StaticStates[Index].Rules + RuleIndex;
      Status = CreateParseRule(Rules + RuleIndex, Rule->Callback, Rule->Options, Rule->NextState, Rule->Count, Rule->Tokens);
      if (EFI_ERROR(Status)) {
        FreePool(Rules);
        return Status;
      }
    }
    // Create each parser state
    Status = CreateParseState(Ptr + Index, StaticStates[Index].Callback, StaticStates[Index].Id, StaticStates[Index].Count, Rules);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
  // Return parser states
  *States = Ptr;
  return EFI_SUCCESS;
}
// FreeParseState
/// Free parser state
/// @param State The parser state to free
/// @return Whether the parser state was freed or not
/// @retval EFI_INVALID_PARAMETER If States is NULL
/// @retval EFI_SUCCESS           If the parser state was freed successfully
EFI_STATUS
EFIAPI
FreeParseState (
  IN LANG_STATE *State
) {
  // Check parameters
  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if ((State->Rules != NULL) && (State->Count > 0)) {
    FreeParseRules(State->Count, State->Rules);
  }
  FreePool(State);
  return EFI_SUCCESS;
}
// FreeParseStates
/// Free parser states
/// @param Count  The count of states to free
/// @param States The parser states to free
/// @return Whether the parser states were freed or not
/// @retval EFI_INVALID_PARAMETER If States is NULL or Count is zero
/// @retval EFI_SUCCESS           If the parser states were freed successfully
EFI_STATUS
EFIAPI
FreeParseStates (
  IN UINTN        Count,
  IN LANG_STATE **States
) {
  UINTN Index;
  // Check parameters
  if ((States == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Free each state
  for (Index = 0; Index < Count; ++Index) {
    if (States[Index] != NULL) {
      FreeParseState(States[Index]);
    }
  }
  return EFI_SUCCESS;
}

// SetParseCallback
/// Set the parser token parsed callback
/// @param Parser   The language parser
/// @param Callback The token parsed callback
/// @return Whether the callback was set or not
/// @retval EFI_SUCCESS If the callback was set successfully
EFI_STATUS
EFIAPI
SetParseCallback (
  IN OUT LANG_PARSER   *Parser,
  IN     LANG_CALLBACK  Callback OPTIONAL
) {
  // Check parameters
  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Parser->Callback = Callback;
  return EFI_SUCCESS;
}
// SetParseState
/// Set the language parser state
/// @param Parser The language parser
/// @param Id     The language parser state identifier
/// @return Whether the parser state was set or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL
/// @retval EFI_NOT_FOUND         If a parser state with the given identifier was not found
/// @retval EFI_SUCCESS           If the language parser state was set successfully
EFI_STATUS
EFIAPI
SetParseState (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        Id
) {
  // Check parameters
  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Set the state
  return SetNextParseState(Parser, Id, FALSE);
}
// GetParseState
/// Get the language parser state
/// @param Parser The language parser
/// @param Id     On output, the language parser state identifier
/// @return Whether the parser state was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Parser or Id is NULL
/// @retval EFI_NOT_FOUND         If there is no current parser state
/// @retval EFI_SUCCESS           If the language parser state was retrieved successfully
EFI_STATUS
EFIAPI
GetParseState (
  IN OUT LANG_PARSER *Parser,
  OUT    UINTN       *Id
) {
  // Check parameters
  if ((Parser == NULL) || (Id == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Check if there is a current state
  if ((Parser->State == NULL) || (Parser->State->Id == LANG_STATE_PREVIOUS)) {
    return EFI_NOT_FOUND;
  }
  // Return current state
  *Id = Parser->State->Id;
  return EFI_SUCCESS;
}
// GetPreviousParseState
/// Get the previous language parser state
/// @param Parser     The language parser
/// @param PreviousId The language parser state identifier
/// @return Whether the previous parser state was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Parser or PreviousId is NULL
/// @retval EFI_NOT_FOUND         If there is no previous parser state
/// @retval EFI_SUCCESS           If the previous language parser state was retrieved successfully
EFI_STATUS
EFIAPI
GetPreviousParseState (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN       *PreviousId
) {
  // Check parameters
  if ((Parser == NULL) || (PreviousId == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Check if there is a current state
  if ((Parser->PreviousStates == NULL) || (Parser->PreviousStates->State == NULL) ||
      (Parser->PreviousStates->State->Id == LANG_STATE_PREVIOUS)) {
    return EFI_NOT_FOUND;
  }
  // Return current state
  *PreviousId = Parser->PreviousStates->State->Id;
  return EFI_SUCCESS;
}
// SetParseStates
/// Set the states of a language parser
/// @param Parser The language parser
/// @param Id     The identifier of the state to set for the parser
/// @param Count  The count of parser states
/// @param States The parser states to set
/// @return Whether the parser states were set or not
/// @retval EFI_INVALID_PARAMETER If Parser or States is NULL or Count is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_NOT_FOUND         If the parser state identifier was not found
/// @retval EFI_SUCCESS           If the parser states were set successfully
EFI_STATUS
EFIAPI
SetParseStates (
  IN OUT LANG_PARSER  *Parser,
  IN     UINTN         Id,
  IN     UINTN         Count,
  IN     LANG_STATE  **States
) {
  // Check parameters
  if ((Parser == NULL) || (States == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Free old states
  FreeParseStates(Parser->Count, Parser->States);
  Parser->Count =  0;
  // Duplicate new states
  Parser->States = DuplicateParseStates(Count, States);
  if (Parser->States == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Find initial state by identifier
  Parser->Count = Count;
  Parser->State = FindParseState(Parser, Id);
  if (Parser->State == NULL) {
    // Identifier not found for initial state
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}
// GetParseStates
/// Get the current states of a parser
/// @param Parser The language parser
/// @param Count  On output, the count of parser states
/// @param States On output, the states of the parser
/// @return Whether the parser states were retrieved or not
/// @retval EFI_INVALID_PARAMETER If Parser, Count, or States is NULL
/// @retval EFI_NOT_FOUND         If the language parser has no states
/// @retval EFI_SUCCESS           If the parser states were retrieved successfully
EFI_STATUS
EFIAPI
GetParseStates (
  IN  LANG_PARSER   *Parser,
  OUT UINTN         *Count,
  OUT LANG_STATE  ***States
) {
  // Check parameters
  if ((Parser == NULL) || (Count == NULL) || (States == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Return the parser states and count
  *Count = Parser->Count;
  *States = Parser->States;
  return EFI_SUCCESS;
}

// CreateParser
/// Create a language parser
/// @param Parser   On output, the created language parser, which must be freed with FreeParser
/// @param Callback The token parsed callback
/// @return Whether the language parser was created or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the parser
/// @retval EFI_SUCCESS           If the language parser was created successfully
EFI_STATUS
EFIAPI
CreateParser (
  OUT LANG_PARSER   **Parser,
  IN  LANG_CALLBACK   Callback OPTIONAL
) {
  LANG_PARSER *Ptr;
  // Check parameters
  if ((Parser == NULL) || (*Parser != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate the language parser
  Ptr = (LANG_PARSER *)AllocateZeroPool(sizeof(LANG_PARSER));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set parsed token callback
  Ptr->Callback = Callback;
  // Return language parser
  *Parser = Ptr;
  return EFI_SUCCESS;
}
// CreateParserWithStates
/// Create a language parser with parser states
/// @param Parser   On output, the created language parser, which must be freed with FreeParser
/// @param Callback The token parsed callback
/// @param Id       The identifier of the state to set for the parser
/// @param Count    The count of parser states
/// @param States   The parser states to set for the language parser
/// @return Whether the language parser was created or not
/// @retval EFI_INVALID_PARAMETER If Parser or States is NULL or Count is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the parser
/// @retval EFI_SUCCESS           If the language parser was created successfully
EFI_STATUS
EFIAPI
CreateParserWithStates (
  OUT LANG_PARSER   **Parser,
  IN  LANG_CALLBACK   Callback OPTIONAL,
  IN  UINTN           Id,
  IN  UINTN           Count,
  IN  LANG_STATE    **States
) {
  EFI_STATUS   Status;
  LANG_PARSER *Ptr = NULL;
  // Check parameters
  if ((Parser == NULL) || (States == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the parser
  Status = CreateParser(&Ptr, Callback);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the parser states
  Status = SetParseStates(Ptr, Id, Count, States);
  if (EFI_ERROR(Status)) {
    FreeParser(Ptr);
    return Status;
  }
  // Return parser
  *Parser = Ptr;
  return EFI_SUCCESS;
}
// CreateParserFromStates
/// Create a language parser with static parser states
/// @param Parser   On output, the created language parser, which must be freed with FreeParser
/// @param Callback The token parsed callback
/// @param Id       The identifier of the state to set for the parser
/// @param Count    The count of static parser states
/// @param States   The static parser states to set for the language parser
/// @return Whether the language parser was created or not
/// @retval EFI_INVALID_PARAMETER If Parser or States is NULL or Count is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the parser
/// @retval EFI_SUCCESS           If the language parser was created successfully
EFI_STATUS
EFIAPI
CreateParserFromStates (
  OUT LANG_PARSER      **Parser,
  IN  LANG_CALLBACK      Callback OPTIONAL,
  IN  UINTN              Id,
  IN  UINTN              Count,
  IN  LANG_STATIC_STATE *States
) {
  EFI_STATUS   Status;
  LANG_PARSER *Ptr;
  // Check parameters
  if ((Parser == NULL) || (States == NULL) || (Count == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create a parser
  Ptr = NULL;
  Status = CreateParser(&Ptr, Callback);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Create the parser states
  Status = CreateParseStates(&(Ptr->States), Ptr->Count = Count, States);
  if (EFI_ERROR(Status)) {
    FreeParser(Ptr);
    return Status;
  }
  // Find initial state by identifier
  Ptr->Count = Count;
  Ptr->State = FindParseState(Ptr, Id);
  // Check if identifier found for initial state
  Status = (Ptr->State == NULL) ? EFI_NOT_FOUND : EFI_SUCCESS;
  // Return parser
  *Parser = Ptr;
  return EFI_SUCCESS;
}
// FreeParser
/// Free a language parser
/// @param Parser The language parser to free
/// @return Whether the language parser was freed or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL
/// @retval EFI_SUCCESS           If the language parser was freed successfully
EFI_STATUS
EFIAPI
FreeParser (
  IN LANG_PARSER *Parser
) {
  // Check parameters
  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Free the parser token
  if (Parser->Token != NULL) {
    FreePool(Parser->Token);
    Parser->Token = NULL;
  }
  // Free the parser states
  if (Parser->States != NULL) {
    FreeParseStates(Parser->Count, Parser->States);
    Parser->States = NULL;
  }
  // Set rest of parser to zeros
  Parser->State = NULL;
  Parser->Count = 0;
  Parser->DecodeCount = 0;
  Parser->DecodedCharacter = 0;
  Parser->TokenCount = 0;
  Parser->TokenSize = 0;
  // Free the parser
  FreePool(Parser);
  return EFI_SUCCESS;
}

// ParseLibInitialize
/// Parse library initialize use
/// @return Whether the parse library initialized successfully or not
/// @retval EFI_SUCCESS The parse library successfully initialized
EFI_STATUS
EFIAPI
ParseLibInitialize (
  VOID
) {
  return EFI_SUCCESS;
}

// ParseLibFinish
/// Parse library finish use
/// @return Whether the parse library finished successfully or not
/// @retval EFI_SUCCESS The parse library successfully finished
EFI_STATUS
EFIAPI
ParseLibFinish (
  VOID
) {
  return EFI_SUCCESS;
}

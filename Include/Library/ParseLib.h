//
/// @file Include/Library/ParseLib.h
///
/// Parse library
///

#pragma once
#ifndef __PARSE_LIBRARY_HEADER__
#define __PARSE_LIBRARY_HEADER__

#include <Library/StringLib.h>

// LANG_STATE_PREVIOUS
/// Previous language state
#define LANG_STATE_PREVIOUS ((UINTN)(-1))

// LANG_RULE_INSENSITIVE
/// Language state rule that uses case-insensitive matching
#define LANG_RULE_INSENSITIVE 0x1
// LANG_RULE_TOKEN
/// Language state rule that parses token before match, if present
#define LANG_RULE_TOKEN 0x2
// LANG_RULE_SKIP
/// Language state rule that skips match
#define LANG_RULE_SKIP 0x4
// LANG_RULE_SKIP_TOKEN
/// Language state rule that skips token before match
#define LANG_RULE_SKIP_TOKEN 0x8
// LANG_RULE_SKIP_EMPTY
/// Language state rule that skips match if token is empty
#define LANG_RULE_SKIP_EMPTY 0x10
// LANG_RULE_PUSH
/// Language state rule that pushes the current state on to the previous states stack
#define LANG_RULE_PUSH 0x20
// LANG_RULE_POP
/// Language state rule that pops the previous state before callback
#define LANG_RULE_POP 0x40

// LANG_RULE_MAX_COUNT
/// The maximum rule count per state
#define LANG_RULE_MAX_COUNT 0x10
// LANG_TOKEN_MAX_COUNT
/// The maximum token count per state rule
#define LANG_TOKEN_MAX_COUNT 0x10

// DECL_LANG_RULE
/// Declare a static state rule
/// @param NextState The state to which to change after this rule is satisfied
/// @param Options   The options of parser state rule matching
/// @param Count     The count of tokens to use for matching
#define DECL_LANG_RULE(NextState, Options, Count, ...) { NULL, NextState, Options, Count, { __VA_ARGS__ } }
// DECL_LANG_RULE_CALLBACK
/// Declare a static state rule
/// @param Callback  The token parsed callback
/// @param NextState The state to which to change after this rule is satisfied
/// @param Options   The options of parser state rule matching
/// @param Count     The count of tokens to use for matching
#define DECL_LANG_RULE_CALLBACK(Callback, NextState, Options, Count, ...) { Callback, NextState, Options, Count, { __VA_ARGS__ } }
// DECL_LANG_STATE
/// Begin a static state
/// @param State   The identifier of the state
/// @param Count   The count of rules for this state
#define DECL_LANG_STATE(State, Count) { NULL, State, Count, {
// DECL_LANG_STATE_CALLBACK
/// Begin a static state
/// @param Callback The token parsed callback
/// @param State    The identifier of the state
/// @param Count    The count of rules for this state
#define DECL_LANG_STATE_CALLBACK(Callback, State, Count) { Callback, State, Count, {
// END_LANG_STATE
/// End a static state
#define END_LANG_STATE() } }
// DECL_LANG_STATES
/// Begin static states
/// @param States The declaration name of the static states
#define DECL_LANG_STATES(States) STATIC LANG_STATIC_STATE States[] = {
// END_LANG_STATES
/// End static stats
#define END_LANG_STATES() }

// LANG_RULE
/// Language state rule
typedef struct _LANG_RULE LANG_RULE;
// LANG_STATE
/// Language state
typedef struct _LANG_STATE LANG_STATE;
// LANG_PARSER
/// Language parser
typedef struct _LANG_PARSER LANG_PARSER;

// LANG_CALLBACK
/// Token parsed callback
/// @param Parser  The language parser
/// @param StateId The current language parser state identifier
/// @param Token   The parsed token
/// @param Context The parse context
/// @return Whether the token was valid or not
typedef EFI_STATUS
(EFIAPI
*LANG_CALLBACK) (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        StateId,
  IN     CHAR16      *Token,
  IN     VOID        *Context OPTIONAL
);

// LANG_STATIC_RULE
/// Static information for state rule
typedef struct _LANG_STATIC_RULE LANG_STATIC_RULE;
struct _LANG_STATIC_RULE {

  // Callback
  /// Token parsed callback
  LANG_CALLBACK Callback;
  // Options
  /// The options of this language state rule
  UINTN   Options;
  // NextRule
  /// The identifier of the language state to which to change when this rule is satisfied
  UINTN   NextState;
  // Count
  /// The count of token strings in the set
  UINTN   Count;
  // Tokens
  /// The set of token strings
  CHAR16 *Tokens[LANG_TOKEN_MAX_COUNT];

};
// LANG_STATIC_STATE
/// Static information for state
typedef struct _LANG_STATIC_STATE LANG_STATIC_STATE;
struct _LANG_STATIC_STATE {

  // Callback
  /// Token parsed callback
  LANG_CALLBACK    Callback;
  // Id
  /// The language state identifier
  UINTN            Id;
  // Count
  /// The count of language state rules
  UINTN            Count;
  // States
  /// The language state rules
  LANG_STATIC_RULE Rules[LANG_RULE_MAX_COUNT];

};

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
);
// Parse
/// Parse a UTF-16 string for tokens
/// @param Parser  The language parser to use in parsing
/// @param Count   The count of characters in the string or NULL if the string is null-terminated
/// @param String  The string to parse
/// @param Context The parse context
/// @param SwapBytes Whether to swap the byte order of each character
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
);
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
);
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
);

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
);
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
);
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
);
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
);
// SetParseStates
/// Set the states of a language parser
/// @param Parser The language parser
/// @param Id     The identifier of the state to set for the parser
/// @param Count  The count of parser states
/// @param States The parser states to set
/// @return Whether the parser states were set or not
/// @retval EFI_INVALID_PARAMETER If Parser or States is NULL or Count is zero
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the parser states were set successfully
EFI_STATUS
EFIAPI
SetParseStates (
  IN OUT LANG_PARSER  *Parser,
  IN     UINTN         Id,
  IN     UINTN         Count,
  IN     LANG_STATE  **States
);
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
);

// CreateParseRule
/// Create parser state rule
/// @param Rule      On output, the created state rule, which must be freed by FreeParseRule
/// @param Callback  The token parsed callback
/// @param Options   The parser state rule options
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
);
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
);
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
);

// CreateParseState
/// Create parser state
/// @param State    On output, the created parser state, which must be freed with FreeParseState
/// @param Callback The token parsed callback
/// @param Id       The identifier of this parser state
/// @param Count    The count of parser state rules
/// @param Rules    The parser state rules to use for this state
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
);
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
);
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
);
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
);

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
);
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
);
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
  OUT LANG_PARSER       **Parser,
  IN  LANG_CALLBACK       Callback OPTIONAL,
  IN  UINTN               Id,
  IN  UINTN               Count,
  IN  LANG_STATIC_STATE  *States
);
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
);

#endif // __PARSE_LIBRARY_HEADER__

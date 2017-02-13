//
/// @file Library/XmlLib/XmlStates.c
///
/// XML states
///

#include "XmlStates.h"

// mXmlStates
/// Static XML states
DECL_LANG_STATES(mXmlStates)
  // XML_LANG_STATE_SIGNATURE
  DECL_LANG_STATE(XML_LANG_STATE_SIGNATURE, 3)
    DECL_LANG_RULE(LANG_RULE_INSENSITIVE | LANG_RULE_SKIP, XML_LANG_STATE_ATTRIBUTE, 1, L"<?xml"),
    DECL_LANG_RULE(LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_COMMENT, 1, L"<!--"),
    DECL_LANG_RULE(LANG_RULE_SKIP, XML_LANG_STATE_SIGNATURE, 4, L" ", L"\t", L"\r", L"\n"),
  END_LANG_STATE(),
  // XML_LANG_STATE_TAG
  DECL_LANG_STATE(XML_LANG_STATE_TAG, 8)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_TAG_NAME, 1, L"<"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_CLOSE_TAG, 1, L"</"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_DOCUMENT_TAG, 1, L"<!"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_COMMENT, 1, L"<!--"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_ENTITY, 1, L"&"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_TAG, 2, L"\t", L"\r"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP_EMPTY, XML_LANG_STATE_TAG, 1, L" "),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_TAG, 1, L"\n"),
  END_LANG_STATE(),
  // XML_LANG_STATE_TAG_NAME
  DECL_LANG_STATE(XML_LANG_STATE_TAG_NAME, 4)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_ATTRIBUTE, 3, L" ", L"\t", L"\r"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_ATTRIBUTE, 1, L"\n"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_TAG, 1, L">"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_TAG, 1, L"/>"),
  END_LANG_STATE(),
  // XML_LANG_STATE_ATTRIBUTE
  DECL_LANG_STATE(XML_LANG_STATE_ATTRIBUTE, 7)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_TAG, 2, L">", L"?>"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_ATTRIBUTE, 3, L" ", L"\t", L"\r"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_TAG, 1, L"/>"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_ATTRIBUTE, 1, L"\n"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_ATTRIBUTE_VALUE, 1, L"="),
    DECL_LANG_RULE(LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_QUOTE, 1, L"\'"),
    DECL_LANG_RULE(LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_DOUBLE_QUOTE, 1, L"\""),
  END_LANG_STATE(),
  // XML_LANG_STATE_ATTRIBUTE_VALUE
  DECL_LANG_STATE(XML_LANG_STATE_ATTRIBUTE_VALUE, 7)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_TAG, 2, L">", L"?>"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_ATTRIBUTE, 3, L" ", L"\t", L"\r"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_TAG, 1, L"/>"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, XML_LANG_STATE_ATTRIBUTE, 1, L"\n"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_ENTITY, 1, L"&"),
    DECL_LANG_RULE(LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_QUOTE, 1, L"\'"),
    DECL_LANG_RULE(LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_DOUBLE_QUOTE, 1, L"\""),
  END_LANG_STATE(),
  // XML_LANG_STATE_CLOSE_TAG
  DECL_LANG_STATE(XML_LANG_STATE_CLOSE_TAG, 1)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, XML_LANG_STATE_TAG, 1, L">"),
  END_LANG_STATE(),
  // XML_LANG_STATE_ENTITY
  DECL_LANG_STATE(XML_LANG_STATE_ENTITY, 2)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP, LANG_STATE_PREVIOUS, 4, L";", L" ", L"\t", L"\r"),
    DECL_LANG_RULE(LANG_RULE_TOKEN, LANG_STATE_PREVIOUS, 1, L"\n"),
  END_LANG_STATE(),
  // XML_LANG_STATE_QUOTE
  DECL_LANG_STATE(XML_LANG_STATE_QUOTE, 2)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_ENTITY, 1, L"&"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_POP, LANG_STATE_PREVIOUS, 1, L"\'"),
  END_LANG_STATE(),
  // XML_LANG_STATE_DOUBLE_QUOTE
  DECL_LANG_STATE(XML_LANG_STATE_DOUBLE_QUOTE, 2)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_PUSH, XML_LANG_STATE_ENTITY, 1, L"&"),
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_POP, LANG_STATE_PREVIOUS, 1, L"\""),
  END_LANG_STATE(),
  // XML_LANG_STATE_COMMENT
  DECL_LANG_STATE(XML_LANG_STATE_COMMENT, 1)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_SKIP_TOKEN, LANG_STATE_PREVIOUS, 1, L"-->"),
  END_LANG_STATE(),
  // XML_LANG_STATE_DOCUMENT_TAG
  DECL_LANG_STATE(XML_LANG_STATE_DOCUMENT_TAG, 5)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_SKIP_TOKEN, XML_LANG_STATE_TAG, 1, L">"),
    DECL_LANG_RULE(LANG_RULE_INSENSITIVE, XML_LANG_STATE_DOCUMENT_TYPE, 1, L"DOCTYPE"),
    DECL_LANG_RULE(LANG_RULE_INSENSITIVE, XML_LANG_STATE_DOCUMENT_ENTITY, 1, L"ENTITY"),
    DECL_LANG_RULE(LANG_RULE_INSENSITIVE, XML_LANG_STATE_DOCUMENT_ELEMENT, 1, L"ELEMENT"),
    DECL_LANG_RULE(LANG_RULE_INSENSITIVE, XML_LANG_STATE_DOCUMENT_ATTLIST, 1, L"ATTLIST"),
  END_LANG_STATE(),
  // XML_LANG_STATE_DOCUMENT_TYPE
  DECL_LANG_STATE(XML_LANG_STATE_DOCUMENT_TYPE, 1)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_SKIP_TOKEN, XML_LANG_STATE_TAG, 1, L">"),
  END_LANG_STATE(),
  // XML_LANG_STATE_DOCUMENT_ENTITY
  DECL_LANG_STATE(XML_LANG_STATE_DOCUMENT_ENTITY, 1)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_SKIP_TOKEN, XML_LANG_STATE_TAG, 1, L">"),
  END_LANG_STATE(),
  // XML_LANG_STATE_DOCUMENT_ELEMENT
  DECL_LANG_STATE(XML_LANG_STATE_DOCUMENT_TYPE, 1)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_SKIP_TOKEN, XML_LANG_STATE_TAG, 1, L">"),
  END_LANG_STATE(),
  // XML_LANG_STATE_DOCUMENT_ATTLIST
  DECL_LANG_STATE(XML_LANG_STATE_DOCUMENT_ATTLIST, 1)
    DECL_LANG_RULE(LANG_RULE_TOKEN | LANG_RULE_SKIP | LANG_RULE_SKIP_TOKEN, XML_LANG_STATE_DOCUMENT_ATTLIST, 1, L">"),
  END_LANG_STATE(),
END_LANG_STATES();

// XmlTreeCreate
/// Create XML document tree node
/// @param Tree On output, the created tree node, which needs freed with XmlTreeFree
/// @param Name The name of the tree node
/// @return Whether the XML document tree node was created or not
/// @retval EFI_INVALID_PARAMETER If Tree or Name is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the XML document tree node was created successfully
STATIC EFI_STATUS
EFIAPI
XmlTreeCreate (
  OUT XML_TREE **Tree,
  IN  CHAR16    *Name
) {
  XML_TREE *Ptr;
  // Check parameters
  if ((Tree == NULL) || (Name == NULL) || (*Name == '\0')) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate tree node
  Ptr = (XML_TREE *)AllocateZeroPool(sizeof(XML_TREE));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set name
  Ptr->Name = StrDup(Name);
  if (Ptr->Name == NULL) {
    FreePool(Ptr);
    return EFI_OUT_OF_RESOURCES;
  }
  // Set other members to NULL
  Ptr->Next = NULL;
  Ptr->Value = NULL;
  Ptr->Children = NULL;
  Ptr->Attributes = NULL;
  // Return created tree node
  *Tree = Ptr;
  return EFI_SUCCESS;
}
// XmlAttributeCreate
/// Create XML document tree node attribute
/// @param Attribute On output, the created tree node attribute, which needs freed with XmlAttributeFree
/// @param Name      The name of the tree node attribute
/// @return Whether the XML document tree node attribute was created or not
/// @retval EFI_INVALID_PARAMETER If Attribute or Name is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the XML document tree node attribute was created successfully
STATIC EFI_STATUS
EFIAPI
XmlAttributeCreate (
  OUT XML_LIST **Attribute,
  IN  CHAR16    *Name
) {
  XML_LIST *Ptr;
  // Check parameters
  if ((Attribute == NULL) || (Name == NULL) || (*Name == '\0')) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate tree node attribute
  Ptr = (XML_LIST *)AllocateZeroPool(sizeof(XML_LIST));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set name
  Ptr->Attribute.Name = StrDup(Name);
  if (Ptr->Attribute.Name == NULL) {
    FreePool(Ptr);
    return EFI_OUT_OF_RESOURCES;
  }
  // Set other members to NULL
  Ptr->Attribute.Value = NULL;
  Ptr->Next = NULL;
  // Return created tree node attribute
  *Attribute = Ptr;
  return EFI_SUCCESS;
}
// XmlAttributeFree
/// Free XML document tree node attribute
/// @param Attribute The tree node attribute to free
/// @return Whether the XML document tree node attribute was freed or not
/// @retval EFI_INVALID_PARAMETER If Attribute is NULL
/// @retval EFI_SUCCESS           If the XML document tree node attribute was freed successfully
STATIC EFI_STATUS
EFIAPI
XmlAttributeFree (
  IN XML_LIST *Attribute
) {
  // Check parameters
  if (Attribute == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Attribute->Attribute.Name != NULL) {
    FreePool(Attribute->Attribute.Name);
    Attribute->Attribute.Name = NULL;
  }
  if (Attribute->Attribute.Value != NULL) {
    FreePool(Attribute->Attribute.Value);
    Attribute->Attribute.Value = NULL;
  }
  FreePool(Attribute);
  return EFI_SUCCESS;
}
// XmlTreeFree
/// Free XML document tree node
/// @param Attribute The tree node to free
/// @return Whether the XML document tree node was freed or not
/// @retval EFI_INVALID_PARAMETER If Tree is NULL
/// @retval EFI_SUCCESS           If the XML document tree node was freed successfully
STATIC EFI_STATUS
EFIAPI
XmlTreeFree (
  IN XML_TREE *Tree
) {
  // Check parameters
  if (Tree == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Tree->Name != NULL) {
    FreePool(Tree->Name);
    Tree->Name = NULL;
  }
  if (Tree->Value != NULL) {
    FreePool(Tree->Value);
    Tree->Value = NULL;
  }
  while (Tree->Attributes != NULL) {
    XML_LIST *Attribute = Tree->Attributes;
    Tree->Attributes = Attribute->Next;
    XmlAttributeFree(Attribute);
  }
  while (Tree->Children != NULL) {
    XML_TREE *Child = Tree->Children;
    Tree->Children = Child->Next;
    XmlTreeFree(Child);
  }
  FreePool(Tree);
  return EFI_SUCCESS;
}

// XmlCallback
/// XML token parsed callback
/// @param Parser  The language parser
/// @param StateId The current language parser state identifier
/// @param Token   The parsed token
/// @return Whether the token was valid or not
STATIC EFI_STATUS
EFIAPI
XmlCallback (
  IN OUT LANG_PARSER *Parser,
  IN     UINTN        StateId,
  IN     CHAR16      *Token,
  IN     VOID        *Context
) {
  EFI_STATUS  Status;
  XML_STACK  *Stack;
  XML_LIST   *List;
  XML_TREE   *Tree;
  XML_PARSER *XmlParser = (XML_PARSER *)Context;
  UINTN       PreviousId = LANG_STATE_PREVIOUS;
  UINTN       TokenLength;
  if (XmlParser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Token == NULL) {
    return EFI_SUCCESS;
  }
  TokenLength = StrLen(Token);
  if (TokenLength == 0) {
    return EFI_SUCCESS;
  }
  switch (StateId) {
    case XML_LANG_STATE_TAG:
      // Value
      Stack = XmlParser->Stack;
      // Check if this is a newline to insert a special space
      if (StrCmp(Token, L"\n") == 0) {
        if ((Stack != NULL) && (Stack->Tree != NULL) && (Stack->Tree->Value != NULL)) {
          // Get the length of the current value
          UINTN Length;
          Length = StrLen(Stack->Tree->Value);
          if (Length > 0) {
            // Append a space
            CHAR16 *Value = (CHAR16 *)AllocateZeroPool((++Length + 1) * sizeof(CHAR16));
            if (Value == NULL) {
              return EFI_OUT_OF_RESOURCES;
            }
            StrCpyS(Value, Length + 1, Stack->Tree->Value);
            Value[Length - 1] = L' ';
            Value[Length] = L'\0';
            FreePool(Stack->Tree->Value);
            Stack->Tree->Value = Value;
          }
        }
      } else if ((Stack == NULL) || (Stack->Tree == NULL)) {
        return EFI_NOT_FOUND;
      } else {
        // Append to the current value
        if (Stack->Tree->Value != NULL) {
          // Get the length of the current value
          UINTN Length = StrLen(Stack->Tree->Value);
          if (Length > 0) {
            // Append the token
            UINTN Size = (Length + 1) * sizeof(CHAR16);
            Stack->Tree->Value = ReallocatePool(Size, Size + (TokenLength * sizeof(CHAR16)), Stack->Tree->Value);
            if (Stack->Tree->Value == NULL) {
              return EFI_OUT_OF_RESOURCES;
            }
            StrCpyS(Stack->Tree->Value + Length, TokenLength + 1, Token);
            Stack->Tree->Value[Length + TokenLength + 1] = L'\0';
          }
        } else {
          // Start a new value
          Stack->Tree->Value = StrDup(Token);
        }
      }
      break;

    case XML_LANG_STATE_TAG_NAME:
      // Check if this is an immdiate close tag
      if (StrCmp(Token, L"/>") == 0) {
        if (XmlParser->Stack == NULL) {
          return EFI_NOT_READY;
        }
        Stack = XmlParser->Stack;
        XmlParser->Stack = Stack->Previous;
        FreePool(Stack);
      } else {
        // New tag name
        Tree = NULL;
        // Create new tree node
        Status = XmlTreeCreate(&Tree, Token);
        if (EFI_ERROR(Status)) {
          return Status;
        }
        // Allocate a new stack object
        Stack = (XML_STACK *)AllocateZeroPool(sizeof(XML_STACK));
        if (Stack == NULL) {
          XmlTreeFree(Tree);
          return EFI_OUT_OF_RESOURCES;
        }
        // Check if there is already a stack
        if (XmlParser->Stack == NULL) {
          // Check there is no document element
          if (XmlParser->Document->Tree != NULL) {
            XmlTreeFree(Tree);
            FreePool(Stack);
            return EFI_NOT_READY;
          }
          // Set the root node
          XmlParser->Document->Tree = Tree;
          // Set the stack object
          Stack->Previous = NULL;
          Stack->Tree = Tree;
          XmlParser->Stack = Stack;
        } else if (XmlParser->Stack->Tree == NULL) {
          XmlTreeFree(Tree);
          FreePool(Stack);
          return EFI_NOT_READY;
        } else if (XmlParser->Stack->Tree->Children == NULL) {
          // Set tree as first child
          XmlParser->Stack->Tree->Children = Tree;
        } else {
          // Add to end of children
          XML_TREE *Child = XmlParser->Stack->Tree->Children;
          while (Child->Next != NULL) {
            Child = Child->Next;
          }
          Child->Next = Tree;
        }
        // Set the stack object
        Stack->Previous = XmlParser->Stack;
        Stack->Tree = Tree;
        XmlParser->Stack = Stack;
      }
      break;

    case XML_LANG_STATE_ATTRIBUTE:
      // Check if this is an immdiate close tag
      if (StrCmp(Token, L"/>") == 0) {
        if (XmlParser->Stack == NULL) {
          return EFI_NOT_READY;
        }
        Stack = XmlParser->Stack;
        XmlParser->Stack = Stack->Previous;
        FreePool(Stack);
      } else {
        // New tag attribute
        Stack = XmlParser->Stack;
        // Create attribute
        List = NULL;
        Status = XmlAttributeCreate(&List, Token);
        if (EFI_ERROR(Status)) {
          return Status;
        }
        // Check if document attribute
        if (Stack == NULL) {
          if (XmlParser->Document->Tree != NULL) {
            return EFI_NOT_READY;
          }
          // Add attribute to document
          if (XmlParser->Document->Attributes == NULL) {
            XmlParser->Document->Attributes = List;
          } else {
            XML_LIST *Attr = XmlParser->Document->Attributes;
            while (Attr->Next != NULL) {
              Attr = Attr->Next;
            }
            Attr->Next = List;
          }
        } else if (Stack->Tree == NULL) {
          // No node to add attribute
          return EFI_NOT_READY;
        } else {
          // Add attribute to tree node
          if (Stack->Tree->Attributes == NULL) {
            Stack->Tree->Attributes = List;
          } else {
            XML_LIST *Attr = Stack->Tree->Attributes;
            while (Attr->Next != NULL) {
              Attr = Attr->Next;
            }
            Attr->Next = List;
          }
        }
      }
      break;

    case XML_LANG_STATE_ATTRIBUTE_VALUE:
      // Check if this is an immdiate close tag
      if (StrCmp(Token, L"/>") == 0) {
        if (XmlParser->Stack == NULL) {
          return EFI_NOT_READY;
        }
        Stack = XmlParser->Stack;
        XmlParser->Stack = Stack->Previous;
        FreePool(Stack);
      } else {
        // Tag attribute value
        Stack = XmlParser->Stack;
        // Check if document attribute
        if (Stack == NULL) {
          if ((XmlParser->Document->Tree != NULL) || (XmlParser->Document->Attributes == NULL)) {
            return EFI_NOT_READY;
          }
          // Find the current document attribute
          List = XmlParser->Document->Attributes;
          while (List->Next != NULL) {
            List = List->Next;
          }
        } else if ((Stack->Tree == NULL) || (Stack->Tree->Attributes == NULL)) {
          return EFI_NOT_READY;
        } else {
          // Find the current attribute
          List = Stack->Tree->Attributes;
          while (List->Next != NULL) {
            List = List->Next;
          }
        }
        // Check to make sure there's not somehow already a value
        if (List->Attribute.Value != NULL) {
          return EFI_NOT_FOUND;
        }
        // Set the attribute value
        List->Attribute.Value = StrDup(Token);
      }
      break;

    case XML_LANG_STATE_CLOSE_TAG:
      // Close tag name
      Stack = XmlParser->Stack;
      if ((Stack == NULL) || (Stack->Tree == NULL) || (Stack->Tree->Name == NULL) ||
          (StrCmp(Stack->Tree->Name, Token) != 0)) {
        // TODO: Error: expected a different tag closed first
        return EFI_NOT_FOUND;
      }
      // Free stack object
      XmlParser->Stack = Stack->Previous;
      FreePool(Stack);
      break;

    case XML_LANG_STATE_ENTITY:
      // Entity name
      Status = GetPreviousParseState(Parser, &PreviousId);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // Ampersand
      if (StriCmp(Token, L"amp") == 0) {
        return XmlCallback(Parser, PreviousId, L"&", Context);
      }
      // Single quote
      if (StriCmp(Token, L"apos") == 0) {
        return XmlCallback(Parser, PreviousId, L"\'", Context);
      }
      // Greater than
      if (StriCmp(Token, L"gt") == 0) {
        return XmlCallback(Parser, PreviousId, L">", Context);
      }
      // Less than
      if (StriCmp(Token, L"lt") == 0) {
        return XmlCallback(Parser, PreviousId, L"<", Context);
      }
      // Space
      if (StriCmp(Token, L"nbsp") == 0) {
        return XmlCallback(Parser, PreviousId, L" ", Context);
      }
      // Double quote
      if (StriCmp(Token, L"quot") == 0) {
        return XmlCallback(Parser, PreviousId, L"\"", Context);
      }
      // Numeral representation of character
      if (*Token == L'#') {
        UINT32 Character = 0;
        ++Token;
        if ((*Token == L'x') || (*Token == L'X')) {
          // Hexadecimal representation of character
          ++Token;
          while (*Token != '\0') {
            if ((*Token >= L'0') && (*Token <= L'9')) {
              Character <<= 4;
              Character |= (UINT32)(*Token - L'0');
            } else if ((*Token >= 'a') || (*Token <= 'f')) {
              Character <<= 4;
              Character |= (UINT32)((*Token - L'a') + 10);
            } else if ((*Token >= 'A') || (*Token <= 'F')) {
              Character <<= 4;
              Character |= (UINT32)((*Token - L'A') + 10);
            } else {
              break;
            }
            ++Token;
          }
        } else {
          //Decimal representation of character
          while (*Token != '\0') {
            if ((*Token >= L'0') && (*Token <= L'9')) {
              Character *= 10;
              Character |= (UINT32)(*Token - L'0');
            } else {
              break;
            }
            ++Token;
          }
        }
        // Replace the character entity with the character
        if (IsUnicodeCharacter(Character)) {
          CHAR16 Str[3] = { L'\0', L'\0', L'\0' };
          if (Character >= 0x10000) {
            Str[0] = (CHAR16)(((Character >> 10) & 0x3FF) + 0xD800);
            Str[1] = (CHAR16)((Character & 0x3FF) + 0xDC00);
          } else {
            Str[0] = (CHAR16)Character;
          }
          return XmlCallback(Parser, PreviousId, Str, Context);
        }
      } else {
        // TODO: Replace entity from schema

      }
      break;

    case XML_LANG_STATE_DOCUMENT_TYPE:
      // TODO: Schema document type
    case XML_LANG_STATE_DOCUMENT_ENTITY:
      // TODO: Schema entity
    case XML_LANG_STATE_DOCUMENT_ELEMENT:
      // TODO: Schema element
    case XML_LANG_STATE_DOCUMENT_ATTLIST:
      // TODO: Schema attribute list
      break;

    default:
      return EFI_NOT_READY;
  }
  return EFI_SUCCESS;
}
// XmlCreate
/// Create an XML parser
/// @param Parser On output, the XML parser, which must be freed by XmlFree
/// @return Whether the XML parser was created or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL or *Parser is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the XML parser
/// @retval EFI_SUCCESS           If the XML parser was created successfully
EFI_STATUS
EFIAPI
XmlCreate (
  OUT XML_PARSER **Parser
) {
  EFI_STATUS  Status;
  XML_PARSER *Ptr;
  // Check parameters
  if ((Parser == NULL) || (*Parser != NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate XML  parser
  Ptr = (XML_PARSER *)AllocateZeroPool(sizeof(XML_PARSER));
  if (Ptr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Ptr->Document = NULL;
  // Allocate language parser
  Status = CreateParserFromStates(&(Ptr->Parser), XmlCallback, XML_LANG_STATE_SIGNATURE, ARRAY_SIZE(mXmlStates), mXmlStates);
  if (EFI_ERROR(Status)) {
    FreePool(Ptr);
    return Status;
  }
  // Return XML parser
  *Parser = Ptr;
  return EFI_SUCCESS;
}

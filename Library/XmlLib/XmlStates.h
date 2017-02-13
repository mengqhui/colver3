//
/// @file Library/XmlLib/XmlStates.h
///
/// XML states
///

#pragma once
#ifndef __XML_LIBRARY_STATES_HEADER__
#define __XML_LIBRARY_STATES_HEADER__

#include <Library/XmlLib.h>

// XML_STATE
/// XML parser state identifiers
typedef enum _XML_STATE XML_STATE;
enum _XML_STATE {

  XML_LANG_STATE_SIGNATURE = 0,
  XML_LANG_STATE_TAG,
  XML_LANG_STATE_TAG_NAME,
  XML_LANG_STATE_ATTRIBUTE,
  XML_LANG_STATE_ATTRIBUTE_VALUE,
  XML_LANG_STATE_CLOSE_TAG,
  XML_LANG_STATE_ENTITY,
  XML_LANG_STATE_QUOTE,
  XML_LANG_STATE_DOUBLE_QUOTE,
  XML_LANG_STATE_COMMENT,
  XML_LANG_STATE_DOCUMENT_TAG,
  XML_LANG_STATE_DOCUMENT_TYPE,
  XML_LANG_STATE_DOCUMENT_ENTITY,
  XML_LANG_STATE_DOCUMENT_ELEMENT,
  XML_LANG_STATE_DOCUMENT_ATTLIST,

};

// XML_LIST
/// XML document tree node attribute list
typedef struct _XML_LIST XML_LIST;
struct _XML_LIST {

  // Next
  /// The next attribute in the list
  XML_LIST      *Next;
  // Attribute
  /// The XML document tree node attribute
  XML_ATTRIBUTE  Attribute;

};
// XML_STACK
/// XML document tree stack
typedef struct _XML_STACK XML_STACK;
struct _XML_STACK {

  // Previous
  /// The previous tree in the stack
  XML_STACK *Previous;
  // Tree
  /// The XML document tree node
  XML_TREE  *Tree;

};

// XML_TREE
/// XML document tree node
struct _XML_TREE {

  // Next
  /// The next tree node
  XML_TREE *Next;
  // Children
  /// The child nodes
  XML_TREE *Children;
  // Name
  /// The tag name
  CHAR16   *Name;
  // Attributes
  /// List of attributes
  XML_LIST *Attributes;
  // Value
  /// Value
  CHAR16   *Value;

};
// XML_SCHEMA
/// XML document schema
struct _XML_SCHEMA {

  // Reserved
  /// Reserved
  UINTN Reserved;

};
// XML_DOCUMENT
/// XML document
struct _XML_DOCUMENT {

  // Schema
  /// XML document schema
  XML_SCHEMA *Schema;
  // Attributes
  /// XML document attributes
  XML_LIST   *Attributes;
  // Tree
  /// XML document tree root node
  XML_TREE   *Tree;
  // Encoding
  /// XML document encoding
  CHAR8      *Encoding;
  // ByteSwap
  /// The encoding bytes for unicode are swapped
  BOOLEAN     ByteSwap;

};
// XML_PARSER
/// XML parser
struct _XML_PARSER {

  // Parser
  /// Parser
  LANG_PARSER  *Parser;
  // Document
  /// XML document
  XML_DOCUMENT *Document;
  // Stack
  /// XML document tree stack
  XML_STACK    *Stack;

};

#endif // __XML_LIBRARY_STATES_HEADER__

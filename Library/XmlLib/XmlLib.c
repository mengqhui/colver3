//
/// @file Library/XmlLib/XmlLib.c
///
/// XML library
///

#include "XmlStates.h"

// XmlAttributeDuplicateMembers
/// @param Destination The destination XML document tree node attribute
/// @param Source      The source XML document tree node attribute
STATIC VOID
EFIAPI
XmlAttributeDuplicateMembers (
  IN XML_ATTRIBUTE *Destination,
  IN XML_ATTRIBUTE *Source
) {
  if (Destination != NULL) {
    if (Destination->Name != NULL) {
      FreePool(Destination->Name);
      Destination->Name = NULL;
    }
    if (Destination->Value != NULL) {
      FreePool(Destination->Value);
      Destination->Value = NULL;
    }
    if (Source != NULL) {
      Destination->Name = StrDup(Source->Name);
      Destination->Value = StrDup(Source->Value);
    }
  }
}
// XmlAttributeFreeMembers
/// Free XML document tree node attribute
/// @param Attribute The XML document tree node attribute
STATIC VOID
EFIAPI
XmlAttributeFreeMembers (
  IN XML_ATTRIBUTE *Attribute
) {
  if (Attribute != NULL) {
    if (Attribute->Name != NULL) {
      FreePool(Attribute->Name);
      Attribute->Name = NULL;
    }
    if (Attribute->Value != NULL) {
      FreePool(Attribute->Value);
      Attribute->Value = NULL;
    }
  }
}
// XmlAttributeFree
/// Free XML document tree node attribute
/// @param Attribute The XML document tree node attribute
STATIC VOID
EFIAPI
XmlAttributeFree (
  IN XML_ATTRIBUTE *Attribute
) {
  // Check parameters
  if (Attribute != NULL) {
    XmlAttributeFreeMembers(Attribute);
    FreePool(Attribute);
  }
}
// XmlAttributeListFree
/// Free XML document tree node attribute
/// @param Attribute The XML document tree node attribute
STATIC VOID
EFIAPI
XmlAttributeListFree (
  IN XML_LIST *Attribute
) {
  // Check parameters
  if (Attribute != NULL) {
    XmlAttributeFreeMembers(&(Attribute->Attribute));
    FreePool(Attribute);
  }
}
// XmlTreeFree
/// Free XML document tree node
/// @param Tree The XML document tree node
STATIC VOID
EFIAPI
XmlTreeFree (
  IN XML_TREE *Tree
) {
  if (Tree != NULL) {
    Tree->Next = NULL;
    if (Tree->Name != NULL) {
      FreePool(Tree->Name);
      Tree->Name = NULL;
    }
    while (Tree->Attributes != NULL) {
      XML_LIST *Attribute = Tree->Attributes;
      Tree->Attributes = Attribute;
      XmlAttributeListFree(Attribute);
    }
    while (Tree->Children != NULL) {
      XML_TREE *Child = Tree->Children;
      Tree->Children = Child->Next;
      XmlTreeFree(Child);
    }
    FreePool(Tree);
  }
}
// XmlSchemaDuplicate
/// @param Schema The XML schema to duplicate
/// @return The duplicated XML schema
STATIC XML_SCHEMA *
EFIAPI
XmlSchemaDuplicate (
  IN XML_SCHEMA *Schema
) {
  XML_SCHEMA *Duplicate;
  // Check parameters
  if (Schema == NULL) {
    return NULL;
  }
  Duplicate = (XML_SCHEMA *)AllocateZeroPool(sizeof(XML_SCHEMA));
  return Duplicate;
}
// XmlSchemaFree
/// Free XML schema
/// @param Schema The XML schema
STATIC VOID
EFIAPI
XmlSchemaFree (
  IN XML_SCHEMA *Schema
) {
  if (Schema != NULL) {
    FreePool(Schema);
  }
}
// XmlDocumentFree
/// Free XML document
/// @param Document The XML document
STATIC VOID
EFIAPI
XmlDocumentFree (
  IN XML_DOCUMENT *Document
) {
  if (Document != NULL) {
    if (Document->Encoding != NULL) {
      FreePool(Document->Encoding);
      Document->Encoding = NULL;
    }
    if (Document->Schema != NULL) {
      FreePool(Document->Schema);
      Document->Schema = NULL;
    }
    if (Document->Tree != NULL) {
      FreePool(Document->Tree);
      Document->Tree = NULL;
    }
    FreePool(Document);
  }
}
// XmlParserFree
/// Free XML parser
/// @param Parser The XML parser
STATIC VOID
EFIAPI
XmlParserFree (
  IN XML_PARSER *Parser
) {
  if (Parser != NULL) {
    if (Parser->Document != NULL) {
      XmlDocumentFree(Parser->Document);
      Parser->Document = NULL;
    }
    FreePool(Parser);
  }
}

// XmlDocumentCreate
/// Create an XML parser document
/// @param Document On output, the XML document, which must be freed by XmlDocumentFree
/// @param Encoding The encoding of the XML encoding or NULL for UTF-8
/// @return Whether the XML document was created or not
/// @retval EFI_INVALID_PARAMETER If Document is NULL or *Document is not NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated for the XML parser
/// @retval EFI_SUCCESS           If the XML document was created successfully
STATIC EFI_STATUS
EFIAPI
XmlDocumentCreate (
  IN XML_DOCUMENT **Document,
  IN CHAR8         *Encoding OPTIONAL
) {
  XML_DOCUMENT *Doc;
  // Check parameters
  if (Document == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Allocate document
  Doc = (XML_DOCUMENT *)AllocateZeroPool(sizeof(XML_DOCUMENT));
  if (Doc == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the document defaults
  Doc->Schema = NULL;
  Doc->Tree = NULL;
  Doc->ByteSwap = FALSE;
  Doc->Encoding = (Encoding == NULL) ? NULL : AsciiStrDup(Encoding);
  // Return the created document
  *Document = Doc;
  return EFI_SUCCESS;
}

// XmlReset
/// Reset an XML parser to initial state for reuse
/// @param Parser The XML parser to reset
/// @return Whether the XML parser was reset or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL
/// @retval EFI_SUCCESS           If the XML parser was reset successfully
EFI_STATUS
EFIAPI
XmlReset (
  IN OUT XML_PARSER *Parser
) {
  // Check parameters
  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  XmlDocumentFree(Parser->Document);
  Parser->Document = NULL;
  return EFI_SUCCESS;
}
// XmlFree
/// Free an XML parser
/// @param Parser The XML parser to free
/// @return Whether the XML parser was freed or not
/// @retval EFI_INVALID_PARAMETER If Parser is NULL
/// @retval EFI_SUCCESS           If the XML parser was freed successfully
EFI_STATUS
EFIAPI
XmlFree (
  IN XML_PARSER *Parser
) {
  // Check parameters
  if (Parser == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  XmlParserFree(Parser);
  return EFI_SUCCESS;
}

// XmlParse
/// Parse a buffer for XML and finish the XML document
/// @param Parser An XML parser used to parse
/// @param Size   The size, in bytes, of the buffer to parse
/// @param Buffer The buffer to parse
/// @return Whether the buffer was parsed or not
EFI_STATUS
EFIAPI
XmlParse (
  IN OUT XML_PARSER  *Parser,
  IN     UINTN        Size,
  IN     VOID        *Buffer
) {
  EFI_STATUS Status;
  // Parse the buffer
  Status = XmlParseStart(Parser, Size, Buffer);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Finish XML document
  return XmlParseFinish(Parser);
}
// XmlParseStart
/// Start parsing multiple buffers for XML
/// @param Parser An XML parser used to parse
/// @param Size   The size, in bytes, of the buffer to parse
/// @param Buffer The buffer to parse, the buffer may contain a leading byte order mark
/// @return Whether the buffer was parsed or not
EFI_STATUS
EFIAPI
XmlParseStart (
  IN OUT XML_PARSER  *Parser,
  IN     UINTN        Size,
  IN     VOID        *Buffer
) {
  EFI_STATUS Status;
  // Check parameters
  if ((Parser == NULL) || (Parser->Document != NULL) || (Buffer == NULL) || (Size <= sizeof(UINT32))) {
    return EFI_INVALID_PARAMETER;
  }
  // Check for byte order
  switch (*((UINT32 *)Buffer)) {

    //
    // Unicode detection
    //
    case 0x003CFEFF:
      // BOM followed by <
    case 0x0020FEFF:
      // BOM followed by space
    case 0x000DFEFF:
      // BOM followed by carriage return
    case 0x000AFEFF:
      // BOM followed by line feed

      // Skip the BOM mark
      Size -= sizeof(CHAR16);
      Buffer = (VOID *)(((CHAR16 *)Buffer) + 1);

    case 0x003F003C:
      // <?
    case 0x003C0020:
      // Space followed by <
    case 0x003C000D:
      // Carriage return followed by <
    case 0x003C000A:
      // Line feed followed by <
    case 0x00200020:
      // Two spaces
    case 0x0020000D:
      // Carriage return followed by space
    case 0x0020000A:
      // Line feed followed by space
    case 0x000D0020:
      // Space followed by carriage return
    case 0x000D000D:
      // Two carriage returns
    case 0x000D000A:
      // Line feed followed by carriage return
    case 0x000A0020:
      // Space followed by line feed
    case 0x000A000D:
      // Carriage return followed by line feed
    case 0x000A000A:
      // Two line feeds

      // Create the XML document
      Status = XmlDocumentCreate(&(Parser->Document), "UTF-16");
      if (EFI_ERROR(Status)) {
        return Status;
      }
      // Parse the buffer
      return Parse(Parser->Parser, Size / sizeof(CHAR16), (CHAR16 *)Buffer, FALSE, Parser);

    //
    // Unicode with swapped byte order detection
    //
    case 0x3C00FFFE:
      // BOM followed by <
    case 0x2000FFFE:
      // BOM followed by space
    case 0x0D00FFFE:
      // BOM followed by carriage return
    case 0x0A00FFFE:
      // BOM followed by line feed

      // Skip the BOM mark
      Size -= sizeof(CHAR16);
      Buffer = (VOID *)(((CHAR16 *)Buffer) + 1);

    case 0x3F003C00:
      // <?
    case 0x3C002000:
      // Space followed by <
    case 0x3C000D00:
      // Carriage return followed by <
    case 0x3C000A00:
      // Line feed followed by <
    case 0x20002000:
      // Two spaces
    case 0x20000D00:
      // Carriage return followed by space
    case 0x20000A00:
      // Line feed followed by space
    case 0x0D002000:
      // Space followed by carriage return
    case 0x0D000D00:
      // Two carriage returns
    case 0x0D000A00:
      // Line feed followed by carriage return
    case 0x0A002000:
      // Space followed by line feed
    case 0x0A000D00:
      // Carriage return followed by line feed
    case 0x0A000A00:
      // Two line feeds

      // Create the XML document
      Status = XmlDocumentCreate(&(Parser->Document), "UTF-16");
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Parser->Document->ByteSwap = TRUE;
      // Parse the buffer
      return Parse(Parser->Parser, Size / sizeof(CHAR16), (CHAR16 *)Buffer, TRUE, Parser);

    //
    // UTF-8 (also includes ASCII/Latin-1) detection
    //
    case 0x3CBFBBEF:
      // BOM followed by <
    case 0x20BFBBEF:
      // BOM followed by space
    case 0x0DBFBBEF:
      // BOM followed by carriage return
    case 0x0ABFBBEF:
      // BOM followed by line feed

      // Skip the BOM mark
      Size -= (3 * sizeof(CHAR8));
      Buffer = (VOID *)(((CHAR8 *)Buffer) + 3);

    case 0x783F3C20:
      // Space followed by <?x
    case 0x783F3C0D:
      // Carriage return followed by <?x
    case 0x783F3C0A:
      // Line feed followed by <?x
    case 0x6D783F3C:
      // <?xm
    case 0x3F3C2020:
      // Two spaces followed by <?
    case 0x3F3C200D:
      // Carriage return followed by space followed by <?
    case 0x3F3C200A:
      // Line feed followed space followed by <?
    case 0x3F3C0D20:
      // Space followed by carriage return followed by <?
    case 0x3F3C0D0D:
      // Two carriage returns followed by <?
    case 0x3F3C0D0A:
      // Line feed followed by carriage return followed by <?
    case 0x3F3C0A20:
      // Space followed by line feed followed by <?
    case 0x3F3C0A0D:
      // Carriage return followed by line feed followed by <?
    case 0x3F3C0A0A:
      // Two line feeds followed by <?
    case 0x3C202020:
      // Three spaces followed by <
    case 0x3C20200D:
      // Carriage return followed by two spaces followed by <
    case 0x3C20200A:
      // Line feed followed by two spaces followed by <
    case 0x3C200D20:
      // Space followed by carriage return followed by space followed by <
    case 0x3C200D0D:
      // Two carriage returns followed by space followed by <
    case 0x3C200D0A:
      // Line feed followed by carriage return followed by space followed by <
    case 0x3C200A20:
      // Space followed by line feed followed by space followed by <
    case 0x3C200A0D:
      // Carriage return followed by line feed followed by space followed by <
    case 0x3C200A0A:
      // Two line feeds followed by space followed by <
    case 0x3C0D2020:
      // Two spaces followed by carriage return followed by <
    case 0x3C0D200D:
      // Carriage return followed by space followed by carriage return followed by <
    case 0x3C0D200A:
      // Line feed followed by space followed by carriage return followed by <
    case 0x3C0D0D20:
      // Space followed by two carriage returns followed by <
    case 0x3C0D0D0D:
      // Three carriage returns followed by <
    case 0x3C0D0D0A:
      // Line feed followed by two carriage returns followed by <
    case 0x3C0D0A20:
      // Space followed by line feed followed by carriage return followed by <
    case 0x3C0D0A0D:
      // Carriage return followed by line feed followed by carriage return followed by <
    case 0x3C0D0A0A:
      // Two line feeds followed by carriage return followed by <
    case 0x3C0A2020:
      // Two spaces followed by line feed followed by <
    case 0x3C0A200D:
      // Carriage return followed by space followed by line feed followed by <
    case 0x3C0A200A:
      // Line feed followed by space followed by line feed followed by <
    case 0x3C0A0D20:
      // Space followed by carriage return followed by line feed followed by <
    case 0x3C0A0D0D:
      // Two carriage returns followed by line feed followed by <
    case 0x3C0A0D0A:
      // Line feed followed by carriage return followed by line feed followed by <
    case 0x3C0A0A20:
      // Space followed by two line feeds followed by <
    case 0x3C0A0A0D:
      // Carriage return followed by two line feeds followed by <
    case 0x3C0A0A0A:
      // Three line feeds followed by <
    case 0x20202020:
      // Four spaces
    case 0x2020200D:
      // Carriage return followed by three spaces
    case 0x2020200A:
      // Line feed followed by three spaces
    case 0x20200D20:
      // Space followed by carriage return followed by two spaces
    case 0x20200D0D:
      // Two carriage returns followed by two spaces
    case 0x20200D0A:
      // Line feed followed by carriage return followed by two spaces
    case 0x20200A20:
      // Space followed by line feed followed by two spaces
    case 0x20200A0D:
      // Carriage return followed by line feed followed by two spaces
    case 0x20200A0A:
      // Two line feeds followed by two spaces
    case 0x200D2020:
      // Two spaces followed by carriage return followed by space
    case 0x200D200D:
      // Carriage return followed by space followed by carriage return followed by space
    case 0x200D200A:
      // Line feed followed by space followed by carriage return followed by space
    case 0x200D0D20:
      // Space followed by two carriage returns followed by space
    case 0x200D0D0D:
      // Three carriage returns followed by space
    case 0x200D0D0A:
      // Line feed followed by two carriage returns followed by space
    case 0x200D0A20:
      // Space followed by line feed followed by carriage return followed by space
    case 0x200D0A0D:
      // Carriage return followed by line feed followed by carriage return followed by space
    case 0x200D0A0A:
      // Two line feeds followed by carriage return followed by space
    case 0x200A2020:
      // Two spaces followed by line feed followed by space
    case 0x200A200D:
      // Carriage return followed by space followed by line feed followed by space
    case 0x200A200A:
      // Line feed followed by space followed by line feed followed by space
    case 0x200A0D20:
      // Space followed by carriage return followed by line feed followed by space
    case 0x200A0D0D:
      // Two carriage returns followed by line feed followed by space
    case 0x200A0D0A:
      // Line feed followed by carriage return followed by line feed followed by space
    case 0x200A0A20:
      // Space followed by two line feeds followed by space
    case 0x200A0A0D:
      // Carriage return followed by two line feeds followed by space
    case 0x200A0A0A:
      // Three line feeds followed by space
    case 0x0D202020:
      // Three spaces followed by carriage return
    case 0x0D20200D:
      // Carriage return followed by two spaces followed by carriage return
    case 0x0D20200A:
      // Line feed followed by two spaces followed by carriage return
    case 0x0D200D20:
      // Space followed by carriage return followed by space followed by carriage return
    case 0x0D200D0D:
      // Two carriage returns followed by space followed by carriage return
    case 0x0D200D0A:
      // Line feed followed by carriage return followed by space followed by carriage return
    case 0x0D200A20:
      // Space followed by line feed followed by space followed by carriage return
    case 0x0D200A0D:
      // Carriage return followed by line feed followed by space followed by carriage return
    case 0x0D200A0A:
      // Two line feeds followed by space followed by carriage return
    case 0x0D0D2020:
      // Two spaces followed by two carriage returns
    case 0x0D0D200D:
      // Carriage return followed by space followed by two carriage returns
    case 0x0D0D200A:
      // Line feed followed by space followed by two carriage returns
    case 0x0D0D0D20:
      // Space followed by three carriage returns
    case 0x0D0D0D0D:
      // Four carriage returns
    case 0x0D0D0D0A:
      // Line feed followed by three carriage returns
    case 0x0D0D0A20:
      // Space followed by line feed followed by two carriage returns
    case 0x0D0D0A0D:
      // Carriage return followed by line feed followed by two carriage returns
    case 0x0D0D0A0A:
      // Two line feeds followed by two carriage returns
    case 0x0D0A2020:
      // Two spaces followed by line feed followed by carriage return
    case 0x0D0A200D:
      // Carriage return followed by space followed by line feed followed by carriage return
    case 0x0D0A200A:
      // Line feed followed by space followed by line feed followed by carriage return
    case 0x0D0A0D20:
      // Space followed by carriage return followed by line feed followed by carriage return
    case 0x0D0A0D0D:
      // Two carriage returns followed by line feed followed by carriage return
    case 0x0D0A0D0A:
      // Line feed followed by carriage return followed by line feed followed by carriage return
    case 0x0D0A0A20:
      // Space followed by two line feeds followed by carriage return
    case 0x0D0A0A0D:
      // Carriage return followed by two line feeds followed by carriage return
    case 0x0D0A0A0A:
      // Three line feeds followed by carriage return
    case 0x0A202020:
      // Three spaces followed by line feed
    case 0x0A20200D:
      // Carriage return followed by two spaces followed by line feed
    case 0x0A20200A:
      // Line feed followed by two spaces followed by line feed
    case 0x0A200D20:
      // Space followed by carriage return followed by space followed by line feed
    case 0x0A200D0D:
      // Two carriage returns followed by space followed by line feed
    case 0x0A200D0A:
      // Line feed followed by carriage return followed by space followed by line feed
    case 0x0A200A20:
      // Space followed by line feed followed by space followed by line feed
    case 0x0A200A0D:
      // Carriage return followed by line feed followed by space followed by line feed
    case 0x0A200A0A:
      // Two line feeds followed by space followed by line feed
    case 0x0A0D2020:
      // Two spaces followed by carriage return followed by line feed
    case 0x0A0D200D:
      // Carriage return followed by space followed by carriage return followed by line feed
    case 0x0A0D200A:
      // Line feed followed by space followed by carriage return followed by line feed
    case 0x0A0D0D20:
      // Space followed by two carriage returns followed by line feed
    case 0x0A0D0D0D:
      // Three carriage returns followed by line feed
    case 0x0A0D0D0A:
      // Line feed followed by two carriage returns followed by line feed
    case 0x0A0D0A20:
      // Space followed by line feed followed by carriage return followed by line feed
    case 0x0A0D0A0D:
      // Carriage return followed by line feed followed by carriage return followed by line feed
    case 0x0A0D0A0A:
      // Two line feeds followed by carriage return followed by line feed
    case 0x0A0A2020:
      // Two spaces followed by two line feeds
    case 0x0A0A200D:
      // Carriage return followed by space followed by two line feeds
    case 0x0A0A200A:
      // Line feed followed by space followed by two line feeds
    case 0x0A0A0D20:
      // Space followed by carriage return followed by two line feeds
    case 0x0A0A0D0D:
      // Two carriage returns followed by two line feeds
    case 0x0A0A0D0A:
      // Line feed followed by carriage return followed by two line feeds
    case 0x0A0A0A20:
      // Space followed by three line feeds
    case 0x0A0A0A0D:
      // Carriage return followed by three line feeds
    case 0x0A0A0A0A:
      // Four line feeds
    default:
      // Unknown encoding, assume ASCII
      break;
  }
  // Create the XML document
  Status = XmlDocumentCreate(&(Parser->Document), "UTF-8");
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Parse the buffer
  return ParseEncoding(Parser->Parser, Size / sizeof(CHAR8), (CHAR8 *)Buffer, "UTF-8", Parser);
}
// XmlParseNext
/// The next buffer to parse in parsing multiple buffers for XML
/// @param Parser An XML parser used to parse
/// @param Size   The size, in bytes, of the buffer to parse
/// @param Buffer The buffer to parse, the buffer may not contain a leading byte order mark and must be the same encoding as the start buffer
/// @return Whether the buffer was parsed or not
EFI_STATUS
EFIAPI
XmlParseNext (
  IN OUT XML_PARSER *Parser,
  IN     UINTN       Size,
  IN     VOID       *Buffer
) {
  // Check parameters
  if ((Parser == NULL) || (Parser->Document == NULL) || (Buffer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Parser->Document->Encoding != NULL) {
    if (AsciiStriCmp(Parser->Document->Encoding, "UTF-16") == 0) {
      return Parse(Parser->Parser, Size / sizeof(CHAR16), (CHAR16 *)Buffer, Parser->Document->ByteSwap, Parser);
    }
  }
  return ParseEncoding(Parser->Parser, Size / sizeof(CHAR8), (CHAR8 *)Buffer, Parser->Document->Encoding, Parser);
}
// XmlParseFinish
/// Finish parsing multiple buffers for XML and finish the XML document
/// @param Parser An XML parser to finish
/// @return Whether the XML document was finished or not
EFI_STATUS
EFIAPI
XmlParseFinish (
  IN OUT XML_PARSER *Parser
) {
  EFI_STATUS Status;
  // Check parameters
  if ((Parser == NULL) || (Parser->Document == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Parse a final space to finish any partially complete token
  Status = ParseCharacter(Parser->Parser, L' ', Parser);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check to make sure that there is an encoding
  if (Parser->Document->Encoding == NULL) {
    Parser->Document->Encoding = AsciiStrDup("UTF-16");
    if (Parser->Document->Encoding == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  return EFI_SUCCESS;
}

// XmlInspect
/// Inspect the XML document tree
/// @param Parser    The XML parser
/// @param Inspector The inspection callback
/// @param Context   The context to pass to the inspection callback
/// @param Recursive Whether the inspection should include child nodes
/// @return Whether the inspection finished or not
/// @retval EFI_INVALID_PARAMETER If Parser or Inspector is NULL
/// @retval EFI_ABORTED           If inspection was aborted by the callback
/// @retval EFI_SUCCESS           If inspection finished
EFI_STATUS
EFIAPI
XmlInspect (
  IN XML_PARSER  *Parser,
  IN XML_INSPECT  Inspector,
  IN VOID        *Context,
  IN BOOLEAN      Recursive
) {
  // Check parameters
  if ((Parser == NULL) || (Inspector == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  return XmlDocumentInspect(Parser->Document, Inspector, Context, Recursive);
}
// XmlDocumentInspect
/// Inspect the XML document tree
/// @param Document  The XML document
/// @param Inspector The inspection callback
/// @param Context   The context to pass to the inspection callback
/// @param Recursive Whether the inspection should include child nodes
/// @return Whether the inspection finished or not
/// @retval EFI_INVALID_PARAMETER If Document or Inspector is NULL
/// @retval EFI_ABORTED           If inspection was aborted by the callback
/// @retval EFI_SUCCESS           If inspection finished
EFI_STATUS
EFIAPI
XmlDocumentInspect (
  IN XML_DOCUMENT *Document,
  IN XML_INSPECT   Inspector,
  IN VOID         *Context,
  IN BOOLEAN       Recursive
) {
  // Check parameters
  if ((Document == NULL) || (Inspector == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  return XmlTreeInspect(Document->Tree, 0, 0, Inspector, Context, Recursive);
}
// XmlTreeInspect
/// Inspect the XML document tree
/// @param Tree       The XML document tree node
/// @param Level      The level of generation of tree nodes, zero for the root
/// @param LevelIndex The index of the tree node relative to the previous level
/// @param Inspector  The inspection callback
/// @param Context    The context to pass to the inspection callback
/// @param Recursive Whether the inspection should include child nodes
/// @return Whether the inspection finished or not
/// @retval EFI_INVALID_PARAMETER If Tree or Inspector is NULL
/// @retval EFI_ABORTED           If inspection was aborted by the callback
/// @retval EFI_SUCCESS           If inspection finished
EFI_STATUS
EFIAPI
XmlTreeInspect (
  IN XML_TREE    *Tree,
  IN UINTN        Level,
  IN UINTN        LevelIndex,
  IN XML_INSPECT  Inspector,
  IN VOID        *Context OPTIONAL,
  IN BOOLEAN      Recursive
) {
  BOOLEAN         Success;
  XML_TREE      **Children;
  XML_ATTRIBUTE **Attributes;
  UINTN           ChildCount;
  UINTN           AttributeCount;
  // Check parameters
  if ((Tree == NULL) || (Inspector == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Get attributes
  Attributes = NULL;
  AttributeCount = 0;
  XmlTreeGetAttributes(Tree, &Attributes, &AttributeCount);
  // Get children
  Children = NULL;
  ChildCount = 0;
  XmlTreeGetChildren(Tree, &Children, &ChildCount);
  // Inspection callback
  Success = Inspector(Tree, Level, LevelIndex, Tree->Name, Tree->Value, AttributeCount, Attributes, ChildCount, Children, Context);
  if (Attributes != NULL) {
    FreePool(Attributes);
  }
  if (Children != NULL) {
    FreePool(Children);
  }
  if (!Success) {
    return EFI_ABORTED;
  }
  // Check if recursive inspection
  if (Recursive) {
    UINTN     Index = 0;
    XML_TREE *Child = Tree->Children;
    while (Child != NULL) {
      // Inspect each child
      EFI_STATUS Status = XmlTreeInspect(Child, Level + 1, Index++, Inspector, Context, Recursive);
      if (EFI_ERROR(Status)) {
        return Status;
      }
      Child = Child->Next;
    }
  }
  return EFI_SUCCESS;
}

// XmlGetDocument
/// Get XML document
/// @param Parser   An XML parser
/// @param Document On output, the XML document
/// @return Whether the XML document was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Parser or Document is NULL
/// @retval EFI_SUCCESS           If the XML document was retrieved successfully
EFI_STATUS
EFIAPI
XmlGetDocument (
  IN  XML_PARSER    *Parser,
  OUT XML_DOCUMENT **Document
) {
  // Check parameters
  if ((Parser == NULL) || (Document == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *Document = Parser->Document;
  return EFI_SUCCESS;
}
// XmlGetSchema
/// Get XML document schema
/// @param Parser An XML parser
/// @param Schema On output, the XML document schema
/// @return Whether the XML document schema was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Parser or Tree is NULL
/// @retval EFI_SUCCESS           If the XML document schema was retrieved successfully
EFI_STATUS
EFIAPI
XmlGetSchema (
  IN  XML_PARSER  *Parser,
  OUT XML_SCHEMA **Schema
) {
  // Check parameters
  if ((Parser == NULL) || (Schema == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  return XmlDocumentGetSchema(Parser->Document, Schema);
}
// XmlGetTree
/// Get XML document tree root node
/// @param Parser An XML parser
/// @param Tree   On output, the XML document tree root node
/// @return Whether the XML document tree root node was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Parser or Tree is NULL
/// @retval EFI_SUCCESS           If the XML document tree root node was retrieved successfully
EFI_STATUS
EFIAPI
XmlGetTree (
  IN  XML_PARSER  *Parser,
  OUT XML_TREE   **Tree
) {
  // Check parameters
  if ((Parser == NULL) || (Tree == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  return XmlDocumentGetTree(Parser->Document, Tree);
}

// XmlDocumentGetEncoding
/// Get XML document encoding
/// @param Document An XML document
/// @param Encoding On output, the XML document encoding
/// @return Whether the XML document encoding was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Document or Encoding is NULL
/// @retval EFI_SUCCESS           If the XML document encoding was retrieved successfully
EFI_STATUS
EFIAPI
XmlDocumentGetEncoding (
  IN  XML_DOCUMENT  *Document,
  OUT CHAR8        **Encoding
) {
  // Check parameters
  if ((Document == NULL) || (Encoding == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *Encoding = Document->Encoding;
  return EFI_SUCCESS;
}
// XmlDocumentSetEncoding
/// Set XML document encoding
/// @param Document An XML document
/// @param Encoding The XML document encoding to set
/// @reeturn Whether the XML document encoding was set or not
/// @retval EFI_INVALID_PARAMETER If Document or Encoding is NULL
/// @retval EFI_SUCCESS           If the XML document encoding was set successfully
EFI_STATUS
EFIAPI
XmlDocumentSetEncoding (
  IN OUT XML_DOCUMENT *Document,
  IN     CHAR8        *Encoding
) {
  // Check parameters
  if ((Document == NULL) || (Encoding == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Document->Encoding != NULL) {
    FreePool(Document->Encoding);
  }
  Document->Encoding = AsciiStrDup(Encoding);
  return EFI_SUCCESS;
}
// XmlDocumentGetSchema
/// Get XML document schema
/// @param Document An XML document
/// @param Schema   On output, the XML document schema
/// @return Whether the XML document schema was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Document or Schema is NULL
/// @retval EFI_SUCCESS           If the XML document schema was retrieved successfully
EFI_STATUS
EFIAPI
XmlDocumentGetSchema (
  IN  XML_DOCUMENT  *Document,
  OUT XML_SCHEMA   **Schema
) {
  // Check parameters
  if ((Document == NULL) || (Schema == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *Schema = Document->Schema;
  return EFI_SUCCESS;
}
// XmlDocumentSetSchema
/// Set XML document schema
/// @param Document An XML document
/// @param Schema   The XML document schema to set
/// @reeturn Whether the XML document schema was set or not
/// @retval EFI_INVALID_PARAMETER If Document or Schema is NULL
/// @retval EFI_SUCCESS           If the XML document schema was set successfully
EFI_STATUS
EFIAPI
XmlDocumentSetSchema (
  IN OUT XML_DOCUMENT *Document,
  IN     XML_SCHEMA   *Schema
) {
  if ((Document == NULL) || (Schema == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Document->Schema != NULL) {
    XmlSchemaFree(Document->Schema);
  }
  Document->Schema = XmlSchemaDuplicate(Schema);
  return EFI_SUCCESS;
}
// XmlDocumentGetTree
/// Get XML document tree root node
/// @param Document An XML document
/// @param Tree     On output, the XML document tree root node
/// @return Whether the XML document tree root node was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Document or Tree is NULL
/// @retval EFI_SUCCESS           If the XML document tree root node was retrieved successfully
EFI_STATUS
EFIAPI
XmlDocumentGetTree (
  IN  XML_DOCUMENT  *Document,
  OUT XML_TREE     **Tree
) {
  // Check parameters
  if ((Document == NULL) || (Tree == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *Tree = Document->Tree;
  return EFI_SUCCESS;
}

// XmlTreeGetTag
/// Get XML document tree node tag name
/// @param Tree An XML document tree
/// @param Tag  On output, the XML document tree tag name
/// @return Whether the XML document tree tag name was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Tree or Tag is NULL
/// @retval EFI_SUCCESS           If the XML document tree tag name was retrieved successfully
EFI_STATUS
EFIAPI
XmlTreeGetTag (
  IN  XML_TREE  *Tree,
  OUT CHAR16   **Tag
) {
  // Check parameters
  if ((Tree == NULL) || (Tag == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *Tag = Tree->Name;
  return EFI_SUCCESS;
}
// XmlTreeSetTag
/// Set XML document tree node tag name
/// @param Tree An XML document tree
/// @param Tag  The XML document tree tag name to set
/// @return Whether the XML document tree tag name was set or not
/// @retval EFI_INVALID_PARAMETER If Tree or Tag is NULL
/// @retval EFI_SUCCESS           If the XML document tree tag name was set successfully
EFI_STATUS
EFIAPI
XmlTreeSetTag (
  IN OUT XML_TREE *Tree,
  IN     CHAR16   *Tag
) {
  // Check parameters
  if ((Tree == NULL) || (Tag == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Tree->Name != NULL) {
    FreePool(Tree->Name);
  }
  Tree->Name = StrDup(Tag);
  return EFI_SUCCESS;
}
// XmlTreeGetValue
/// Get XML document tree node value
/// @param Tree  An XML document tree
/// @param Value On output, the XML document tree value
/// @return Whether the XML document tree node value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Tree or Value is NULL
/// @retval EFI_SUCCESS           If the XML document tree node value was retrieved successfully
EFI_STATUS
EFIAPI
XmlTreeGetValue (
  IN  XML_TREE  *Tree,
  OUT CHAR16   **Value
) {
  // Check parameters
  if ((Tree == NULL) || (Value == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  *Value = Tree->Value;
  return EFI_SUCCESS;
}
// XmlTreeSetValue
/// Set XML document tree node value
/// @param Tree An XML document tree
/// @param Tag  The XML document tree node value to set
/// @return Whether the XML document tree node value was set or not
/// @retval EFI_INVALID_PARAMETER If Tree or Value is NULL
/// @retval EFI_SUCCESS           If the XML document tree node value was set successfully
EFI_STATUS
EFIAPI
XmlTreeSetValue (
  IN OUT XML_TREE *Tree,
  IN     CHAR16   *Value
) {
  // Check parameters
  if ((Tree == NULL) || (Value == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Tree->Value != NULL) {
    FreePool(Tree->Value);
  }
  Tree->Value = StrDup(Value);
  return EFI_SUCCESS;
}
// XmlTreeHasChildren
/// Check if XML document tree node has child nodes
/// @param Tree An XML document tree node
/// @return Whether the XML document tree node has child nodes or not
/// @retval TRUE  If the tree node has children
/// @retval FALSE If the tree node does not have children
BOOLEAN
EFIAPI
XmlTreeHasChildren (
  IN  XML_TREE *Tree
) {
  return ((Tree != NULL) && (Tree->Children != NULL));
}
// XmlTreeGetChildren
/// Get XML document tree node child nodes
/// @param Tree     An XML document tree
/// @param Children On output, the XML document tree node child nodes, which must be freed
/// @param Count    On output, the count of children
/// @return Whether the XML document tree child nodes were retrieved or not
/// @retval EFI_INVALID_PARAMETER If Tree, Children, or Count is NULL or *Children is not NULL
/// @retval EFI_NOT_FOUND         If there are no XML document tree child nodes
/// @retval EFI_SUCCESS           If the XML document tree child nodes were retrieved successfully
EFI_STATUS
EFIAPI
XmlTreeGetChildren (
  IN  XML_TREE   *Tree,
  OUT XML_TREE ***Children,
  OUT UINTN      *Count
) {
  XML_TREE **ChildList;
  XML_TREE  *List;
  UINTN      ChildCount;
  UINTN      ListCount;
  // Check parameters
  if ((Tree == NULL) || (Children == NULL) || (*Children != NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Count the child nodes
  ListCount = 0;
  for (List = Tree->Children; List != NULL; List = List->Next) {
    ++ListCount;
  }
  // If there are no children, return not found
  if (ListCount == 0) {
    return EFI_NOT_FOUND;
  }
  // Allocate new list
  ChildList = (XML_TREE **)AllocateZeroPool(ListCount * sizeof(XML_TREE *));
  if (ChildList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy child nodes list
  ChildCount = 0;
  List = Tree->Children;
  while ((List != NULL) && (ChildCount < ListCount)) {
    ChildList[ChildCount++] = List;
    List = List->Next;
  }
  // Return the child nodes list
  *Children = ChildList;
  *Count = ChildCount;
  return EFI_SUCCESS;
}
// XmlTreeGetAttributes
/// Get XML document tree node attributes
/// @param Tree       An XML document tree
/// @param Attributes On output, the XML document tree node attributes, which must be freed
/// @param Count      On output, the count of attributes
/// @return Whether the XML document tree attributes were retrieved or not
/// @retval EFI_INVALID_PARAMETER If Tree, Attributes, or Count is NULL or *Attributes is not NULL
/// @retval EFI_NOT_FOUND         If there are no XML document tree attibutes
/// @retval EFI_SUCCESS           If the XML document tree attributes were retrieved successfully
EFI_STATUS
EFIAPI
XmlTreeGetAttributes (
  IN  XML_TREE        *Tree,
  OUT XML_ATTRIBUTE ***Attributes,
  OUT UINTN           *Count
) {
  XML_ATTRIBUTE **AttributeList;
  XML_LIST       *List;
  UINTN           AttributeCount;
  UINTN           ListCount;
  // Check parameters
  if ((Tree == NULL) || (Attributes == NULL) || (*Attributes != NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Count the attributes
  ListCount = 0;
  for (List = Tree->Attributes; List != NULL; List = List->Next) {
    ++ListCount;
  }
  // If there are no attributes, return not found
  if (ListCount == 0) {
    return EFI_NOT_FOUND;
  }
  // Allocate new list
  AttributeList = (XML_ATTRIBUTE **)AllocateZeroPool(ListCount * sizeof(XML_ATTRIBUTE *));
  if (AttributeList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Copy attributes list
  AttributeCount = 0;
  List = Tree->Attributes;
  while ((List != NULL) && (AttributeCount < ListCount)) {
    AttributeList[AttributeCount++] = &(List->Attribute);
    List = List->Next;
  }
  // Return the attributes list
  *Attributes = AttributeList;
  *Count = AttributeCount;
  return EFI_SUCCESS;
}
// XmlTreeGetAttribute
/// Get an XML document tree node attribute
/// @param Tree      An XML document tree
/// @param Name      The XML document tree node attribute name
/// @param Attribute On output, the XML document tree attribute
/// @return Whether the XML document tree attribute was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Tree, Name, or Attribute is NULL
/// @retval EFI_NOT_FOUND         If the XML document tree attibute was not found
/// @retval EFI_SUCCESS           If the XML document tree attribute was retrieved successfully
EFI_STATUS
EFIAPI
XmlTreeGetAttribute (
  IN  XML_TREE       *Tree,
  IN  CHAR16         *Name,
  OUT XML_ATTRIBUTE **Attribute
) {
  XML_LIST *List;
  // Check parameters
  if ((Tree == NULL) || (Name == NULL) || (Attribute == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Search for attribute
  for (List = Tree->Attributes; List != NULL; List = List->Next) {
    if ((List->Attribute.Name != NULL) && (StrCmp(Name, List->Attribute.Name) == 0)) {
      *Attribute = &(List->Attribute);
      return EFI_SUCCESS;
    }
  }
  // Not found
  return EFI_NOT_FOUND;
}
// XmlTreeSetAttribute
/// Set an XML document tree node attribute
/// @param Tree      An XML document tree
/// @param Name      The XML document tree node attribute name
/// @param Attribute The XML document tree attribute to set
/// @return Whether the XML document tree attribute was set or not
/// @retval EFI_INVALID_PARAMETER If Tree, Name, Attribute, or Attribute->Name is NULL
/// @retval EFI_SUCCESS           If the XML document tree attribute was set successfully
EFI_STATUS
EFIAPI
XmlTreeSetAttribute (
  IN OUT XML_TREE      *Tree,
  IN     CHAR16        *Name,
  IN     XML_ATTRIBUTE *Attribute
) {
  XML_LIST *List = NULL;
  // Check parameters
  if ((Tree == NULL) || (Name == NULL) || (Attribute == NULL) || (Attribute->Name == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Search for attribute
  for (List = Tree->Attributes; List != NULL; List = List->Next) {
    if ((List->Attribute.Name != NULL) && (StrCmp(Name, List->Attribute.Name) == 0)) {
      XmlAttributeDuplicateMembers(&(List->Attribute), Attribute);
      return EFI_SUCCESS;
    }
  }
  // Allocate a new attribute
  if (List == NULL) {
    if (Tree->Attributes == NULL) {
      List = Tree->Attributes = (XML_LIST *)AllocateZeroPool(sizeof(XML_LIST));
    } else {
      for (List = Tree->Attributes; List->Next != NULL; List = List->Next);
      List = List->Next = (XML_LIST *)AllocateZeroPool(sizeof(XML_LIST));
    }
  }
  // Check the attribute was allocate
  if (List == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Return the new attribute
  List->Next = NULL;
  XmlAttributeDuplicateMembers(&(List->Attribute), Attribute);
  return EFI_SUCCESS;
}
// XmlTreeRemoveAttribute
/// Remove an attribute from XML document tree node
/// @param Tree An XML document tree
/// @param Name The name of the XML document tree node attribute to remove
/// @return Whether the XML document tree attribute was removed or not
/// @retval EFI_INVALID_PARAMETER If Tree or Name is NULL
/// @retval EFI_SUCCESS           If the XML document tree attribute was removed successfully
EFI_STATUS
EFIAPI
XmlTreeRemoveAttribute (
  IN OUT XML_TREE *Tree,
  IN     CHAR16   *Name
) {
  XML_LIST *Attribute;
  // Check parameters
  if ((Tree == NULL) || (Name == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Tree->Attributes != NULL) {
    // Check if first attribute
    if (StrCmp(Name, Tree->Attributes->Attribute.Name) == 0) {
      Attribute = Tree->Attributes;
      Tree->Attributes = Attribute->Next;
      XmlAttributeListFree(Attribute);
      return EFI_SUCCESS;
    } else {
      // Check rest of attributes
      for (Attribute = Tree->Attributes; Attribute->Next != NULL; Attribute = Attribute->Next) {
        if ((Attribute->Next->Attribute.Name!= NULL) && (StrCmp(Name, Attribute->Next->Attribute.Name) == 0)) {
          return EFI_SUCCESS;
        }
      }
    }
  }
  // Not found
  return EFI_NOT_FOUND;
}

// XmlLibInitialize
/// XML library initialize use
/// @return Whether the XML library initialized successfully or not
/// @retval EFI_SUCCESS The XML library successfully initialized
EFI_STATUS
EFIAPI
XmlLibInitialize (
  VOID
) {
  return EFI_SUCCESS;
}

// XmlLibFinish
/// XML library finish use
/// @return Whether the XML library finished successfully or not
/// @retval EFI_SUCCESS The XML library successfully finished
EFI_STATUS
EFIAPI
XmlLibFinish (
  VOID
) {
  return EFI_SUCCESS;
}

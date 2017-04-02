//
/// @file Library/ConfigLib/ConfigLib.c
///
/// Configuration library
///

#include <Library/ConfigLib.h>

#include <Library/LogLib.h>
#include <Library/SmBiosLib.h>

#include <Library/UefiBootServicesTableLib.h>

// CONFIG_PARSE
/// Parse configuration information from XML document tree
/// @param Tree   The XML document tree to parse
/// @param Source The unique source name
/// @return Whether the configuration was parsed successfully or not
/// @retval EFI_INVALID_PARAMETER If Tree is NULL
/// @retval EFI_SUCCESS           If the configuration was parsed successfully
typedef EFI_STATUS
(EFIAPI
*CONFIG_PARSE) (
  IN XML_TREE *Tree,
  IN CHAR16   *Source OPTIONAL
);
// CONFIG_FREE
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free or NULL for root (same as ConfigFree)
/// @return Whether the configuration values were freed or not
/// @retval EFI_SUCCESS If the configuration values were freed 
typedef EFI_STATUS
(EFIAPI
*CONFIG_FREE) (
  IN CHAR16 *Path OPTIONAL
);
// CONFIG_GET_LIST
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
typedef EFI_STATUS
(EFIAPI
*CONFIG_GET_LIST) (
  IN  CHAR16   *Path OPTIONAL,
  OUT CHAR16 ***List,
  OUT UINTN    *Count
);
// CONFIG_GET_COUNT
/// Get the count of children of the configuration path
/// @param Path  The configuration path or NULL for root
/// @param Count On output, the count of strings in the list
/// @return Whether the count was returned or not
/// @retval EFI_INVALID_PARAMETER If Count is NULL
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_SUCCESS           If the count was returned successfully
typedef EFI_STATUS
(EFIAPI
*CONFIG_GET_COUNT) (
  IN  CHAR16 *Path OPTIONAL,
  OUT UINTN  *Count
);
// CONFIG_GET_VALUE
/// Get a configuration value
/// @param Path  The path of the configuration value
/// @param Type  On output, the type of the configuration value
/// @param Value On output, the value of the configuration value
/// @return Whether the configuration value was retrieved or not
/// @retval EFI_INVALID_PARAMETER If Path, Type, or Value is NULL
/// @retval EFI_NOT_FOUND         If the configuration value was not found
/// @retval EFI_ACCESS_DENIED     If the configuration value is protected
/// @retval EFI_SUCCESS           If the configuration value was retrieved successfully
typedef EFI_STATUS
(EFIAPI
*CONFIG_GET_VALUE) (
  IN  CHAR16       *Path,
  OUT CONFIG_TYPE  *Type,
  OUT CONFIG_VALUE *Value OPTIONAL
);
// CONFIG_SET_VALUE
/// Set a configuration value
/// @param Path  The path of the configuration value
/// @param Type  The configuration type to set
/// @param Value The configuration value to set
/// @return Whether the configuration value was set or not
/// @retval EFI_INVALID_PARAMETER If Path or Value is NULL or Type is invalid
/// @retval EFI_SUCCESS           If the configuration value was set successfully
typedef EFI_STATUS
(EFIAPI
*CONFIG_SET_VALUE) (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value
);

// CONFIG_PROTOCOL
/// Configuration tree protcol
typedef struct _CONFIG_PROTOCOL CONFIG_PROTOCOL;
struct _CONFIG_PROTOCOL {

  // LoadFromString
  /// Load configuration information from string
  CONFIG_PARSE      Parse;
  // Free
  /// Free configuration values with a configuration path
  CONFIG_FREE       Free;
  // GetList
  /// Get a list of names for the children of the configuration path
  CONFIG_GET_LIST   GetList;
  // GetCount
  /// Get the count of children of the configuration path
  CONFIG_GET_COUNT  GetCount;
  // GetValue
  /// Get a configuration value
  CONFIG_GET_VALUE  GetValue;
  // SetValue
  /// Set a configuration value
  CONFIG_SET_VALUE  SetValue;

};

// CONFIG_TREE
/// Configuration tree node
typedef struct _CONFIG_TREE CONFIG_TREE;
struct _CONFIG_TREE {

  // Next
  /// The next node at this level
  CONFIG_TREE  *Next;
  // Children
  /// The nodes that are children of this node
  CONFIG_TREE  *Children;
  // Name
  /// The name of this node
  CHAR16       *Name;
  // Type
  /// The type of this node
  CONFIG_TYPE   Type;
  // Value
  /// The value of this node
  CONFIG_VALUE  Value;

};

// CFGXML_INSPECT_AUTO_GROUP
/// This configuration key must always be grouped, any children will be placed inside of group zero if not grouped
#define CFGXML_INSPECT_AUTO_GROUP 0x1

// CFGXML_INSPECT
/// Configuration XML inspection callback data
typedef struct _CFGXML_INSPECT CFGXML_INSPECT;
struct _CFGXML_INSPECT {

  // Source
  /// The unique source path
  CHAR16 *Source;
  // Path
  /// The path to the configuration value
  CHAR16 *Path;
  // Options
  /// The options for the configuration value
  UINTN   Options;

};

// CONFIG_LINK
/// Configuration path link
typedef struct _CONFIG_LINK CONFIG_LINK;
struct _CONFIG_LINK {

  // Path
  /// The path to link
  CHAR16 *Path;
  // Link
  /// The linked path, may be NULL to prevent a path from populating
  CHAR16 *Link;

};

// mConfigGuid
/// The configuration protocol GUID
STATIC EFI_GUID         mConfigGuid = { 0x2F4BD4A0, 0x227B, 0x4967, { 0x8B, 0xB0, 0xE6, 0xB7, 0xD5, 0xF9, 0x8F, 0x16 } };
// mConfigHandle
/// The configuration protocol handle
STATIC EFI_HANDLE       mConfigHandle = NULL;
// mConfig
/// The installed configuration protocol
STATIC CONFIG_PROTOCOL *mConfig = NULL;
// mConfigTree
/// The configuration tree root node
STATIC CONFIG_TREE     *mConfigTree = NULL;
// mConfigAutoGroups
/// The configuration auto group keys
STATIC CHAR16          *mConfigAutoGroups[] = {
  L"\\CPU\\Package",
  L"\\Memory\\Array",
  L"\\Memory\\Array\\[0-9]\\Slot"
  L"\\Memory\\Array\\[0-9][0-9]\\Slot"
  L"\\Memory\\Array\\[0-9][0-9][0-9]\\Slot"
};
// mConfigLinks
/// The configuration link keys
STATIC CONFIG_LINK      mConfigLinks[] = {
  { L"\\Memory\\Slot", L"\\Memory\\Array\\0\\Slot" }
};

// ConfigFind
/// Find a configuration tree node by path
/// @param Path The path of the configuration tree node
/// @param Tree On output, the address of the configuration tree node
/// @return Whether the 
STATIC EFI_STATUS
EFIAPI
ConfigFind2 (
  IN  CHAR16        *Path OPTIONAL,
  IN  BOOLEAN        Create,
  OUT CONFIG_TREE ***Tree
) {
  CONFIG_TREE **Node;
  CHAR16       *Next;
  CHAR16       *Name;
  // Check parameters
  if (Tree == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Check if there are any nodes
  if (mConfigTree == NULL) {
    if (Create) {
      // Create new root node if needed
      mConfigTree = (CONFIG_TREE *)AllocateZeroPool(sizeof(CONFIG_TREE));
      if (mConfigTree == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      mConfigTree->Next = NULL;
      mConfigTree->Children = NULL;
      mConfigTree->Name = NULL;
      mConfigTree->Type = CONFIG_TYPE_LIST;
    } else {
      // Not found
      return EFI_NOT_FOUND;
    }
  }
  // Check if searching for root could be NULL, "", or "\"
  Node = &mConfigTree;
  if (Path != NULL) {
    while (*Path == L'\\') {
      ++Path;
    }
    // Iterate through the configuration tree nodes
    while (*Path != L'\0') {
      // Check if there are any children
      if (!Create && ((*Node)->Children == NULL)) {
        if (StriCmp(Path, (*Node)->Name) == 0) {
          break;
        }
        // Not found
        return EFI_NOT_FOUND;
      }
      // Get the next node start
      Next = StrStr(Path, L"\\");
      if (Next == NULL) {
        // Get the node name for which to search
        Name = StrDup(Path);
        // Set to end of path if not found
        Path += StrLen(Path);
      } else if (Next == Path) {
        // Skip consecutive separators
        ++Path;
        continue;
      } else {
        // Get the node name for which to search
        Name = StrnDup(Path, Next - Path);
        // Set to after the separator
        Path = Next + 1;
      }
      if (Name == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      // Iterate through children for name
      for (Node = &((*Node)->Children); *Node != NULL; Node = &((*Node)->Next)) {
        // Try to match child name
        if ((*Node)->Name != NULL) {
          INTN Result = StriCmp(Name, (*Node)->Name);
          if (Result == 0) {
            // Found the node
            break;
          } else if (Result < 0) {
            CONFIG_TREE *Next;
            if (!Create) {
              // Not found
              FreePool(Name);
              return EFI_NOT_FOUND;
            }
            // Insert another node
            Next = (CONFIG_TREE *)AllocateZeroPool(sizeof(CONFIG_TREE));
            if (Next == NULL) {
              FreePool(Name);
              return EFI_OUT_OF_RESOURCES;
            }
            Next->Next = *Node;
            Next->Children = NULL;
            Next->Type = CONFIG_TYPE_LIST;
            // Need to duplicate name here since it will be freed later
            Next->Name = StrDup(Name);
            *Node = Next;
            break;
          }
        }
      }
      // Check if found
      if (*Node == NULL) {
        if (Create) {
          // Create new node if needed
          *Node = (CONFIG_TREE *)AllocateZeroPool(sizeof(CONFIG_TREE));
          if (*Node == NULL) {
            FreePool(Name);
            return EFI_OUT_OF_RESOURCES;
          }
          (*Node)->Next = NULL;
          (*Node)->Children = NULL;
          (*Node)->Type = CONFIG_TYPE_LIST;
          (*Node)->Name = Name;
        } else {
          // Not found
          FreePool(Name);
          return EFI_NOT_FOUND;
        }
      } else {
        FreePool(Name);
      }
    }
  }
  // Check if found
  if (*Node == NULL) {
    return EFI_NOT_FOUND;
  }
  // Return the tree node
  *Tree = Node;
  return EFI_SUCCESS;
}
// ConfigFind
/// Find a configuration tree node by path
/// @param Path The path of the configuration tree node
/// @return The configuration tree node or NULL if not found
STATIC EFI_STATUS
EFIAPI
ConfigFind (
  IN  CHAR16       *Path OPTIONAL,
  IN  BOOLEAN       Create,
  OUT CONFIG_TREE **Tree
) {
  CONFIG_TREE **Node = NULL;
  EFI_STATUS    Status;
  Status = ConfigFind2(Path, Create, &Node);
  if (!EFI_ERROR(Status) && (Node != NULL)) {
    *Tree = *Node;
  }
  return Status;
}
// ConfigTreeFree
/// Free configuration tree node
/// @param Tree The configuration tree node
/// @return Whether the configuration tree node was freed or not
/// @retval EFI_INVALID_PARAMETER If Tree is NULL
/// @retval EFI_SUCCESS           If the node was freed successfully
STATIC EFI_STATUS
EFIAPI
ConfigTreeFree (
  IN CONFIG_TREE *Tree
) {
  // Check parameters
  if (Tree == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (Tree->Name != NULL) {
    FreePool(Tree->Name);
    Tree->Name = NULL;
  }
  // Free value
  switch (Tree->Type) {
  case CONFIG_TYPE_STRING:
    // Free string value
    if (Tree->Value.String != NULL) {
      FreePool(Tree->Value.String);
      Tree->Value.String = NULL;
    }
    break;

  case CONFIG_TYPE_DATA:
    // Free data value
    if (Tree->Value.Data.Data != NULL) {
      FreePool(Tree->Value.Data.Data);
    }

  default:
    // Set every thing else to empty
    Tree->Value.Data.Size = 0;
    Tree->Value.Data.Data = NULL;
    break;
  }
  // Free any children
  if (Tree->Children != NULL) {
    CONFIG_TREE *Child = Tree->Children;
    while (Child != NULL) {
      CONFIG_TREE *Tmp = Child;
      Child = Tmp->Next;
      ConfigTreeFree(Tmp);
    }
    Tree->Children = NULL;
  }
  // Free node
  FreePool(Tree);
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS      Status;
  EFI_FILE_HANDLE Handle = NULL;
  // Check parameters
  if ((Root == NULL) && (Path == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  if (Path != NULL) {
    // Open configuration file handle
    Status = FileHandleOpen(&Handle, Root, Path, EFI_FILE_MODE_READ, 0);
  } else {
    // Get file name of file handle
    Status = FileHandleGetFileName(Root, &Path);
    if (!EFI_ERROR(Status) && (Path != NULL)) {
      // Open configuration file handle
      Status = FileHandleOpen(&Handle, Root, Path, EFI_FILE_MODE_READ, 0);
      FreePool(Path);
    }
  }
  // If file handle is open parse configuration
  if (!EFI_ERROR(Status) && (Handle != NULL)) {
    UINT64 FileSize = 0;
    UINTN  Size = 0;
    // Get file size to read the configuration
    Status = FileHandleGetSize(Handle, &FileSize);
    if (!EFI_ERROR(Status)) {
      if (FileSize == 0) {
        // Assume not found if no file size
        Status = EFI_NOT_FOUND;
      } else {
        // Allocate buffer to hold configuration string
        VOID *Config = (VOID *)AllocateZeroPool(Size = (UINTN)FileSize);
        if (Config == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
        } else {
          // Read configuration string from file
          Status = FileHandleRead(Handle, &Size, (VOID *)Config);
          if (!EFI_ERROR(Status)) {
            Status = ConfigParse(Size, Config, Path);
          }
          FreePool(Config);
        }
      }
    }
    // Close the file handle
    FileHandleClose(Handle);
  }
  return Status;
}
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
) {
  EFI_STATUS  Status;
  XML_PARSER *Parser = NULL;
  // Check parameters
  if ((Config == NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create XML parser
  Status = XmlCreate(&Parser);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Parser == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Parse the XML buffer
  Status = XmlParse(Parser, Size, Config);
  if (!EFI_ERROR(Status)) {
    XML_TREE *Tree = NULL;
    // Get the XML document tree root node
    Status = XmlGetTree(Parser, &Tree);
    if (!EFI_ERROR(Status)) {
      // Parse the configuration
      Status = ConfigParseXml(Tree, Source);
    }
  }
  // Free the XML parser
  XmlFree(Parser);
  return Status;
}

// ConfigXmlInspector
/// Configuration XML document tree inspection callback
/// @param Tree           The document tree node
/// @param Level          The level of generation of tree nodes, zero for the root
/// @param LevelIndex     The index of the tree node relative to the previous level
/// @param TagName        The tree node tag name
/// @param Value          The tree node value
/// @param AttributeCount The tree node attribute count
/// @param Attributes     The tree node attributes
/// @param ChildCount     The tree node child count
/// @param Children       The tree node children
/// @param Context        The context passed when inspection started
/// @retval TRUE  If the inspection should continue
/// @retval FALSE If the inspection should stop
STATIC BOOLEAN
EFIAPI
ConfigXmlInspector (
  IN XML_TREE       *Tree,
  IN UINTN           Level,
  IN UINTN           LevelIndex,
  IN CHAR16         *TagName,
  IN CHAR16         *Value OPTIONAL,
  IN UINTN           AttributeCount,
  IN XML_ATTRIBUTE **Attributes OPTIONAL,
  IN UINTN           ChildCount,
  IN XML_TREE      **Children OPTIONAL,
  IN VOID           *Context OPTIONAL
) {
  CFGXML_INSPECT *Parent = (CFGXML_INSPECT *)Context;
  CFGXML_INSPECT  This = { NULL, NULL, 0 };
  UINTN           Index;
  // Check parameters
  if ((Tree == NULL) || (TagName == NULL)) {
    return TRUE;
  }
  if (Parent != NULL) {
    This.Source = Parent->Source;
  }
  // Get attributes
  if ((Attributes != NULL) && (AttributeCount > 0)) {
    // Iterate through attributes
    for (Index = 0; Index < AttributeCount; ++Index) {
      if ((Attributes[Index] != NULL) && (Attributes[Index]->Name != NULL)) {
        if (StriCmp(Attributes[Index]->Name, L"arch") == 0) {
          // Check architectures match
          if ((Attributes[Index]->Value == NULL) || (StriCmp(Attributes[Index]->Value, PROJECT_ARCH) != 0)) {
            // Skip this tree node since it's intended for a different architecture
            return TRUE;
          }
        } else if (StriCmp(Attributes[Index]->Name, L"manufacturer") == 0) {
          // Check manufacturer matches
          CHAR16 *Manufacturer = ConfigGetStringWithDefault(L"\\System\\Manufacturer", NULL);
          if ((Manufacturer == NULL) || (Attributes[Index]->Value == NULL) || (StriStr(Manufacturer, Attributes[Index]->Value) != NULL)) {
            // Skip this tree node since it's intended for a different manufacturer
            return TRUE;
          }
        } else if (StriCmp(Attributes[Index]->Name, L"product") == 0) {
          // Check product matches
          CHAR16 *ProductName = ConfigGetStringWithDefault(L"\\System\\ProductName", NULL);
          if ((ProductName == NULL) || (Attributes[Index]->Value == NULL) || (StriStr(ProductName, Attributes[Index]->Value) != NULL)) {
            // Skip this tree node since it's intended for a different product
            return TRUE;
          }
        }
      }
    }
  }
  // Check for group type
  if (StriCmp(TagName, L"group") == 0) {
    if ((ChildCount == 0) || (Children == NULL)) {
      return TRUE;
    }
    // Create the index of this group
    This.Path = CatSPrint(NULL, L"%s\\%u", (Parent == NULL) ? L"" : Parent->Path, LevelIndex);
  } else if ((Level == 1) && (ChildCount == 0) && (Value != NULL) && (StriCmp(TagName, L"include") == 0)) {
    // Include another configuration but don't include self
    if ((Parent != NULL) && (StriCmp(Value, Parent->Source) != 0)) {
      ConfigLoad(NULL, Value);
    }
    return TRUE;
  } else if ((Parent != NULL) && ((Parent->Options & CFGXML_INSPECT_AUTO_GROUP) != 0)) {
    // Auto group this partial path
    This.Path = CatSPrint(NULL, L"%s\\0\\%s", (Parent == NULL) ? L"" : Parent->Path, TagName);
  } else {
    // Create full path
    This.Path = FileMakePath((Parent == NULL) ? NULL : Parent->Path, TagName);
  }
  if (This.Path == NULL) {
    return TRUE;
  }
  // Check if this path is linked
  for (Index = 0; Index < ARRAY_SIZE(mConfigLinks); ++Index) {
    // The path can match a pattern
    if ((mConfigLinks[Index].Path != NULL) && MetaiMatch(This.Path, mConfigLinks[Index].Path)) {
      // Free the old path
      FreePool(This.Path);
      // Check if this is forbid population link
      if (mConfigLinks[Index].Link == NULL) {
        return TRUE;
      }
      // Duplicate the linked path
      This.Path = StrDup(mConfigLinks[Index].Link);
      if (This.Path == NULL) {
        return TRUE;
      }
      // Found a match and created the linked path
      break;
    }
  }
  // Get children
  if ((Children != NULL) && (ChildCount > 0)) {
    // Check if this key is auto grouped
    for (Index = 0; Index < ARRAY_SIZE(mConfigAutoGroups); ++Index) {
      if (MetaiMatch(This.Path, mConfigAutoGroups[Index])) {
        This.Options |= CFGXML_INSPECT_AUTO_GROUP;
        break;
      }
    }
    // Iterate through children
    for (Index = 0; Index < ChildCount; ++Index) {
      // Inspect each child
      XmlTreeInspect(Children[Index], Level + 1, Index, ConfigXmlInspector, (VOID *)&This, FALSE);
    }
  } else if (StriCmp(TagName, L"integer") == 0) {
    // Integer value
    if (Value != NULL) {
      INTN Integer = 1;
      if (*Value == L'-') {
        Integer = -1;
        ++Value;
      }
      if ((*Value == L'0') && ((Value[1] == L'x') || (Value[1] == L'X'))) {
        Integer *= (INTN)StrHexToUintn(Value + 2);
        LOG(L"  %s=0x%0*X\n", This.Path, sizeof(UINTN) << 1, Integer);
      } else {
        Integer *= (INTN)StrDecimalToUintn(Value);
        LOG(L"  %s=%d\n", This.Path, Integer);
      }
      ConfigSetInteger(This.Path, Integer, TRUE);
    }
    FreePool(This.Path);
    return TRUE;
  } else if (StriCmp(TagName, L"unsigned") == 0) {
    // Unsigned integer value
    if (Value != NULL) {
      UINTN Unsigned;
      if ((*Value == L'0') && ((Value[1] == L'x') || (Value[1] == L'X'))) {
        Unsigned = StrHexToUintn(Value + 2);
        LOG(L"  %s=0x%0*X\n", This.Path, sizeof(UINTN) << 1, Unsigned);
      } else {
        Unsigned = StrDecimalToUintn(Value);
        LOG(L"  %s=%u\n", This.Path, Unsigned);
      }
      ConfigSetUnsigned(This.Path, Unsigned, TRUE);
    }
    FreePool(This.Path);
    return TRUE;
  } else if (StriCmp(TagName, L"data") == 0) {
    // Data base64 value
    UINTN  Size = 0;
    VOID  *Data = NULL;
    if (!EFI_ERROR(FromBase64(Value, &Size, &Data)) && (Data != NULL)) {
      if (Size > 0) {
        LOG(L"  %s=%s\n", This.Path, Value);
        ConfigSetData(This.Path, Size, Data, TRUE);
      }
      FreePool(Data);
    }
    FreePool(This.Path);
    return TRUE;
  } else if (StriCmp(TagName, L"boolean") == 0) {
    // Boolean value
    if (Value != NULL) {
      BOOLEAN Boolean = ((StriCmp(Value, L"true") == 0) || (StriCmp(Value, L"on") == 0) || (StriCmp(Value, L"yes") == 0) ||
                         ((*Value == L'0') && ((Value[1] == L'x') || (Value[1] == L'X')) && (StrHexToUintn(Value + 2) != 0)) ||
                         (StrDecimalToUintn(Value) != 0));
      LOG(L"  %s=%s\n", This.Path, Boolean ? L"true" : L"false");
      ConfigSetBoolean(This.Path, Boolean, TRUE);
    }
    FreePool(This.Path);
    return TRUE;
  } else if (StriCmp(TagName, L"true") == 0) {
    // True
    LOG(L"  %s=true\n", This.Path);
    ConfigSetBoolean(This.Path, TRUE, TRUE);
    FreePool(This.Path);
    return TRUE;
  } else if (StriCmp(TagName, L"false") == 0) {
    // False
    LOG(L"  %s=false\n", This.Path);
    ConfigSetBoolean(This.Path, FALSE, TRUE);
    FreePool(This.Path);
    return TRUE;
  } else if (Value != NULL) {
    // Value
    LOG(L"  %s=\"%s\"\n", This.Path, Value);
    ConfigSetString(This.Path, Value, TRUE);
  }
  FreePool(This.Path);
  return TRUE;
}
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
) {
  CFGXML_INSPECT   Root = { NULL, NULL, 0 };
  XML_TREE       **Children = NULL;
  CHAR16          *Name = NULL;
  UINTN            Count = 0;
  UINTN            Index;
  // Check parameters
  if (Tree == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Use configuration protocol if present
  if ((mConfig != NULL) && (mConfig->Parse != NULL)) {
    return mConfig->Parse(Tree, Source);
  }
  if (EFI_ERROR(XmlTreeGetTag(Tree, &Name)) || (Name == NULL) || (StriCmp(Name, L"configuration") != 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Inspect the XML tree
  if (!EFI_ERROR(XmlTreeGetChildren(Tree, &Children, &Count)) && (Children != NULL) && (Count > 0)) {
    Root.Source = Source;
    for (Index = 0; Index < Count; ++Index) {
      EFI_STATUS Status = XmlTreeInspect(Children[Index], 1, Index, ConfigXmlInspector, &Root, FALSE);
      if (EFI_ERROR(Status)) {
        FreePool(Children);
        return Status;
      }
    }
  }
  if (Children != NULL) {
    FreePool(Children);
  }
  return EFI_SUCCESS;
}

// ConfigFree
/// Free all configuration values
/// @return Whether the configuration values were freed or not
/// @retval EFI_SUCCESS If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigFree (
  VOID
) {
  return ConfigPartialFree(NULL);
}
// ConfigPartialFree
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free or NULL for root (same as ConfigFree)
/// @param ...  The argument list
/// @return Whether the configuration values were freed or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigPartialFree (
  IN CHAR16 *Path OPTIONAL,
  ...
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Path);
  Status = ConfigVPartialFree(Path, Args);
  VA_END(Args);
  return Status;
}
// ConfigVPartialFree
/// Free configuration values with a configuration path
/// @param Path The configuration path to use as root to free (same as ConfigFree)
/// @param Args The argument list
/// @return Whether the configuration values were freed or not
/// @retval EFI_INVALID_PARAMETER If Path is NULL
/// @retval EFI_OUT_OF_RESOURCES  If memory could not be allocated
/// @retval EFI_SUCCESS           If the configuration values were freed
EFI_STATUS
EFIAPI
ConfigVPartialFree (
  IN CHAR16  *Path OPTIONAL,
  IN VA_LIST  Args
) {
  CONFIG_TREE **Tree = NULL;
  CONFIG_TREE  *Node;
  EFI_STATUS    Status;
  CHAR16       *FullPath = NULL;
  // Check parameters
  if (Path != NULL) {
    // Create the path from the argument list
    FullPath = CatVSPrint(NULL, Path, Args);
    if (FullPath == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  // Use configuration protocol if present
  if ((mConfig != NULL) && (mConfig->Free != NULL)) {
    Status = mConfig->Free(FullPath);
    FreePool(FullPath);
    return Status;
  }
  // Find the address of the configuration tree node
  Status = ConfigFind2(FullPath, FALSE, &Tree);
  FreePool(FullPath);
  if (EFI_ERROR(Status) || (Tree == NULL) || (*Tree == NULL)) {
    return (Status == EFI_NOT_FOUND) ? EFI_SUCCESS : Status;
  }
  // Replace the node with the next
  Node = *Tree;
  *Tree = Node->Next;
  // Free the node
  return ConfigTreeFree(Node);
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Path);
  Status = ConfigVExists(Path, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath;
  CONFIG_TREE *Tree = NULL;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Try to find the configuration key
  Status = ConfigFind(FullPath, FALSE, &Tree);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Tree == NULL) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

// DefaultConfigInspector
/// Default configuration value inspector
/// @param Path    The configuration path
/// @param Type    The type of the configuration key
/// @param Value   The value of the configuration key if Type is a valid value type (not CONFIG_TYPE_UNKNOWN or CONFIG_TYPE_LIST)
/// @param Context The context passed when inspection was invoked
/// @retval TRUE  if inspection should continue
/// @retval FALSE If inspection should be aborted
STATIC BOOLEAN
DefaultConfigInspector (
  IN CHAR16       *Path,
  IN CONFIG_TYPE   Type,
  IN CONFIG_VALUE *Value OPTIONAL,
  IN VOID         *Context
) {
  if (Path != NULL) {
    if (Value != NULL) {
      switch (Type) {
        case CONFIG_TYPE_BOOLEAN:
          Log(L"  %s=%s\n", Path, Value->Boolean ? L"true" : L"false");
          break;

        case CONFIG_TYPE_INTEGER:
          Log(L"  %s=%d\n", Path, Value->Integer);
          break;

        case CONFIG_TYPE_UNSIGNED:
          Log(L"  %s=%u\n", Path, Value->Unsigned);
          break;

        case CONFIG_TYPE_STRING:
          Log(L"  %s=\"%s\"\n", Path, Value->String);
          break;

        case CONFIG_TYPE_DATA:
          Log(L"  %s=(%u)\n", Path, Value->Data.Size);
          break;

        default:
          Log(L"  %s\n", Path);
          break;
      }
    }
  }
  return TRUE;
}

// ConfigInspect
/// Inspect configuration values
/// @param Path      The configuration path
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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Recursive);
  Status = ConfigVInspect(Path, Inspector, Context, Recursive,  Args);
  VA_END(Args);
  return Status;
}
// ConfigVInspect
/// Inspect configuration values
/// @param Path      The configuration path
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
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath = NULL;
  CONFIG_TREE *Tree = NULL;
  // Check parameters
  if (Inspector == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  if (Path != NULL) {
    FullPath = CatVSPrint(NULL, Path, Args);
    if (FullPath == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  // Get the configuration key
  Status = ConfigFind(FullPath, FALSE, &Tree);
  if (EFI_ERROR(Status)) {
    if (FullPath != NULL) {
      FreePool(FullPath);
    }
    return Status;
  }
  if (Tree == NULL) {
    if (FullPath != NULL) {
      FreePool(FullPath);
    }
    return EFI_NOT_FOUND;
  }
  // Inspect this key if not the root
  if ((FullPath != NULL) && (*FullPath != L'\0')) {
    if ((Tree->Type != CONFIG_TYPE_UNKNOWN) && !Inspector(FullPath, Tree->Type, (Tree->Type == CONFIG_TYPE_LIST) ? NULL : &(Tree->Value), Context)) {
      if (FullPath != NULL) {
        FreePool(FullPath);
      }
      return EFI_ABORTED;
    }
  }
  // Recursively inspect children if needed
  if (Recursive) {
    CONFIG_TREE *Child;
    // Traverse each child
    for (Child = Tree->Children; Child != NULL; Child = Child->Next) {
      if (Child->Name != NULL) {
        // Create a new path
        CHAR16 *ChildPath = FileMakePath(FullPath, Child->Name);
        if (ChildPath == NULL) {
          Status = EFI_OUT_OF_RESOURCES;
          break;
        }
        // Inspect this child
        if (Child->Type != CONFIG_TYPE_UNKNOWN) {
          Status = ConfigInspect(ChildPath, Inspector, Context, Recursive);
        }
        FreePool(ChildPath);
      }
      if (EFI_ERROR(Status)) {
        break;
      }
    }
  }
  if (FullPath != NULL) {
    FreePool(FullPath);
  }
  return Status;
}

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
  IN  CHAR16   *Path,
  OUT CHAR16 ***List,
  OUT UINTN    *Count,
  IN  ...
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Count);
  Status = ConfigVGetList(Path, List, Count, Args);
  VA_END(Args);
  return Status;
}
// ConfigVGetList
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
ConfigVGetList (
  IN  CHAR16    *Path,
  OUT CHAR16  ***List,
  OUT UINTN     *Count,
  IN  VA_LIST    Args
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath;
  CONFIG_TREE *Node = NULL;
  // Check parameters
  if ((Path == NULL) || (List == NULL) || (*List != NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Use configuration protocol if present
  if ((mConfig != NULL) && (mConfig->GetList != NULL)) {
    Status = mConfig->GetList(FullPath, List, Count);
    FreePool(FullPath);
    return Status;
  }
  // Get the configuration tree node
  Status = ConfigFind(FullPath, FALSE, &Node);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if ((Node == NULL) || (Node->Type != CONFIG_TYPE_LIST)) {
    return EFI_NOT_FOUND;
  }
  // Add the child node names to the list
  *Count = 0;
  for (Node = Node->Children; Node != NULL; Node = Node->Next) {
    if (Node->Name != NULL) {
      StrList(List, Count, Node->Name, 0, STR_LIST_NO_DUPLICATES | STR_LIST_CASE_INSENSITIVE | STR_LIST_SORTED);
    }
  }
  if ((*Count == 0) || (*List == NULL)) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

// ConfigGetCount
/// Get the count of children of the configuration path
/// @param Path  The configuration path
/// @param Count On output, the count of children
/// @param ...   The argument list
/// @return Whether the count was returned or not
/// @retval EFI_INVALID_PARAMETER If Path or Count is NULL
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_SUCCESS           If the count was returned successfully
EFI_STATUS
EFIAPI
ConfigGetCount (
  IN  CHAR16 *Path,
  OUT UINTN  *Count,
  IN  ...
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Count);
  Status = ConfigVGetCount(Path, Count, Args);
  VA_END(Args);
  return Status;
}
// ConfigVGetCount
/// Get the count of children of the configuration path
/// @param Path  The configuration path
/// @param Count On output, the count of children
/// @param Args  The argument list
/// @return Whether the count was returned or not
/// @retval EFI_INVALID_PARAMETER If Path or Count is NULL
/// @retval EFI_NOT_FOUND         If the configuration path was not found
/// @retval EFI_SUCCESS           If the count was returned successfully
EFI_STATUS
EFIAPI
ConfigVGetCount (
  IN  CHAR16  *Path,
  OUT UINTN   *Count,
  IN  VA_LIST  Args
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath;
  CONFIG_TREE *Node = NULL;
  // Check parameters
  if ((Path == NULL) || (Count == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Use configuration protocol if present
  if ((mConfig != NULL) && (mConfig->GetCount != NULL)) {
    Status = mConfig->GetCount(FullPath, Count);
    FreePool(FullPath);
    return Status;
  }
  // Get the configuration tree node
  Status = ConfigFind(FullPath, FALSE, &Node);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if ((Node == NULL) || (Node->Type != CONFIG_TYPE_LIST)) {
    return EFI_NOT_FOUND;
  }
  // Count the child nodes
  *Count = 0;
  for (Node = Node->Children; Node != NULL; Node = Node->Next) {
    ++(*Count);
  }
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Type);
  Status = ConfigVGetType(Path, Type, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath;
  CONFIG_TREE *Node = NULL;
  // Check parameters
  if ((Path == NULL) || (Type == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get configuration tree node
  Status = ConfigFind(FullPath, FALSE, &Node);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Node == NULL) {
    return EFI_NOT_FOUND;
  }
  // Return type
  *Type = Node->Type;
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Value);
  Status = ConfigVGetValue(Path, Type, Value, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath;
  CONFIG_TREE *Node = NULL;
  // Check parameters
  if ((Path == NULL) || (Type == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Use configuration protocol if present
  if ((mConfig != NULL) && (mConfig->GetValue != NULL)) {
    Status = mConfig->GetValue(Path, Type, Value);
    FreePool(FullPath);
    return Status;
  }
  // Get configuration tree node
  Status = ConfigFind(FullPath, FALSE, &Node);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Node == NULL) {
    return EFI_NOT_FOUND;
  }
  // Return type and value
  *Type = Node->Type;
  CopyMem(Value, &(Node->Value), sizeof(CONFIG_VALUE));
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Boolean);
  Status = ConfigVGetBoolean(Path, Boolean, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if ((Path == NULL) || (Boolean == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get the configuration value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check type matches
  if (Type != CONFIG_TYPE_BOOLEAN) {
    return EFI_ABORTED;
  }
  *Boolean = Value.Boolean;
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Integer);
  Status = ConfigVGetInteger(Path, Integer, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if ((Path == NULL) || (Integer == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get the configuration value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check type matches
  if (Type != CONFIG_TYPE_INTEGER) {
    return EFI_ABORTED;
  }
  *Integer = Value.Integer;
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Unsigned);
  Status = ConfigVGetUnsigned(Path, Unsigned, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if ((Path == NULL) || (Unsigned == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get the configuration value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check type matches
  if (Type != CONFIG_TYPE_UNSIGNED) {
    return EFI_ABORTED;
  }
  *Unsigned = Value.Unsigned;
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, String);
  Status = ConfigVGetString(Path, String, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if ((Path == NULL) || (String == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get the configuration value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check type matches
  if (Type != CONFIG_TYPE_STRING) {
    return EFI_ABORTED;
  }
  *String = Value.String;
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Data);
  Status = ConfigVGetData(Path, Size, Data, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if ((Path == NULL) || (Size == NULL) || (Data == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get the configuration value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  // Check type matches
  if (Type != CONFIG_TYPE_DATA) {
    return EFI_ABORTED;
  }
  *Size = Value.Data.Size;
  *Data = Value.Data.Data;
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Value);
  Status = ConfigVGetValueWithDefault(Path, DefaultType, DefaultValue, Type, Value, Args);
  VA_END(Args);
  return Status;
}
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
) {
  CHAR16 *FullPath;
  // Check parameters
  if ((Path == NULL) || (DefaultValue == NULL) || (Type == NULL) || (Value == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Check default type is valid
  switch (DefaultType) {
    case CONFIG_TYPE_BOOLEAN:
    case CONFIG_TYPE_INTEGER:
    case CONFIG_TYPE_UNSIGNED:
    case CONFIG_TYPE_STRING:
    case CONFIG_TYPE_DATA:
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Get value
  if (EFI_ERROR(ConfigGetValue(FullPath, Type, Value))) {
    // Set default value
    *Type = DefaultType;
    CopyMem(Value, DefaultValue, sizeof(CONFIG_VALUE));
  }
  FreePool(FullPath);
  return EFI_SUCCESS;
}

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
) {
  BOOLEAN Result;
  VA_LIST Args;
  VA_START(Args, DefaultBoolean);
  Result = ConfigVGetBooleanWithDefault(Path, DefaultBoolean, Args);
  VA_END(Args);
  return Result;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  CONFIG_VALUE  Value;
  // Check parameters
  if (Path == NULL) {
    return DefaultBoolean;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return DefaultBoolean;
  }
  // Get the value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status) || (Type != CONFIG_TYPE_BOOLEAN)) {
    return DefaultBoolean;
  }
  return Value.Boolean;
}

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
) {
  INTN    Result;
  VA_LIST Args;
  VA_START(Args, DefaultInteger);
  Result = ConfigVGetIntegerWithDefault(Path, DefaultInteger, Args);
  VA_END(Args);
  return Result;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  CONFIG_VALUE  Value;
  // Check parameters
  if (Path == NULL) {
    return DefaultInteger;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return DefaultInteger;
  }
  // Get the value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status) || (Type != CONFIG_TYPE_INTEGER)) {
    return DefaultInteger;
  }
  return Value.Integer;
}

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
) {
  UINTN   Result;
  VA_LIST Args;
  VA_START(Args, DefaultUnsigned);
  Result = ConfigVGetUnsignedWithDefault(Path, DefaultUnsigned, Args);
  VA_END(Args);
  return Result;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  CONFIG_VALUE  Value;
  // Check parameters
  if (Path == NULL) {
    return DefaultUnsigned;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return DefaultUnsigned;
  }
  // Get the value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status) || (Type != CONFIG_TYPE_UNSIGNED)) {
    return DefaultUnsigned;
  }
  return Value.Unsigned;
}

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
) {
  CHAR16  *Result;
  VA_LIST  Args;
  VA_START(Args, DefaultString);
  Result = ConfigVGetStringWithDefault(Path, DefaultString, Args);
  VA_END(Args);
  return Result;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if (Path == NULL) {
    return DefaultString;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return DefaultString;
  }
  // Get the value
  Status = ConfigGetValue(FullPath, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status) || (Type != CONFIG_TYPE_STRING)) {
    return DefaultString;
  }
  return Value.String;
}

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
) {
  VOID    *Result;
  VA_LIST  Args;
  VA_START(Args, Size);
  Result = ConfigVGetDataWithDefault(Path, DefaultSize, DefaultData, Size, Args);
  VA_END(Args);
  return Result;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  CONFIG_TYPE   Type = CONFIG_TYPE_UNKNOWN;
  // Check parameters
  if ((Path == NULL) || (Size == NULL)) {
    *Size = DefaultSize;
    return DefaultData;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    *Size = DefaultSize;
    return DefaultData;
  }
  // Get the value
  Status = ConfigGetValue(Path, &Type, &Value);
  FreePool(FullPath);
  if (EFI_ERROR(Status) || (Type != CONFIG_TYPE_DATA)) {
    *Size = DefaultSize;
    return DefaultData;
  }
  *Size = Value.Data.Size;
  return Value.Data.Data;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Overwrite);
  Status = ConfigVSetValue(Path, Type, Value, Overwrite, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS   Status;
  CHAR16      *FullPath;
  CONFIG_TREE *Node = NULL;
  // Check parameters
  if ((Path == NULL) || (Value == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  switch (Type) {
    case CONFIG_TYPE_BOOLEAN:
    case CONFIG_TYPE_INTEGER:
    case CONFIG_TYPE_UNSIGNED:
      break;

    case CONFIG_TYPE_STRING:
      if (Value->String != NULL) {
        break;
      } else

    case CONFIG_TYPE_DATA:
      if ((Value->Data.Data != NULL) && (Value->Data.Size != 0)) {
        break;
      }

    default:
      return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Use configuration protocol if present
  if ((mConfig != NULL) && (mConfig->SetValue != NULL)) {
    Status = mConfig->SetValue(FullPath, Type, Value);
    FreePool(FullPath);
    return Status;
  }
  // Check if the node exists if not overwriting
  if (!Overwrite) {
    Status = ConfigFind(FullPath, FALSE, &Node);
    if (!EFI_ERROR(Status) && (Node != NULL)) {
      FreePool(FullPath);
      return EFI_SUCCESS;
    }
  }
  // Get configuration tree node
  Node = NULL;
  Status = ConfigFind(FullPath, TRUE, &Node);
  FreePool(FullPath);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (Node == NULL) {
    return EFI_NOT_FOUND;
  }
  if (Node->Children != NULL) {
    return EFI_ACCESS_DENIED;
  }
  // Set type and value
  Node->Type = Type;
  if (Type == CONFIG_TYPE_STRING) {
    // Duplicate string type
    Node->Value.String = StrDup(Value->String);
    if (Node->Value.String == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  } else if (Type == CONFIG_TYPE_DATA) {
    // Duplicate data type
    Node->Value.Data.Size = Value->Data.Size;
    Node->Value.Data.Data = AllocateZeroPool(Value->Data.Size);
    if (Node->Value.Data.Data == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    CopyMem(Node->Value.Data.Data, Value->Data.Data, Value->Data.Size);
  } else {
    // Copy all other types
    CopyMem(&(Node->Value), Value, sizeof(CONFIG_VALUE));
  }
  return EFI_SUCCESS;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Overwrite);
  Status = ConfigVSetBoolean(Path, Boolean, Overwrite, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the value
  Value.Boolean = Boolean;
  Status = ConfigSetValue(FullPath, CONFIG_TYPE_BOOLEAN, &Value, Overwrite);
  FreePool(FullPath);
  return Status;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Overwrite);
  Status = ConfigVSetInteger(Path, Integer, Overwrite, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the value
  Value.Integer = Integer;
  Status = ConfigSetValue(FullPath, CONFIG_TYPE_INTEGER, &Value, Overwrite);
  FreePool(FullPath);
  return Status;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Overwrite);
  Status = ConfigVSetUnsigned(Path, Unsigned, Overwrite, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  // Check parameters
  if (Path == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the value
  Value.Unsigned = Unsigned;
  Status = ConfigSetValue(FullPath, CONFIG_TYPE_UNSIGNED, &Value, Overwrite);
  FreePool(FullPath);
  return Status;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Overwrite);
  Status = ConfigVSetString(Path, String, Overwrite, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  // Check parameters
  if ((Path == NULL) || (String == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the value
  Value.String = String;
  Status = ConfigSetValue(FullPath, CONFIG_TYPE_STRING, &Value, Overwrite);
  FreePool(FullPath);
  return Status;
}

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
) {
  EFI_STATUS Status;
  VA_LIST    Args;
  VA_START(Args, Overwrite);
  Status = ConfigVSetData(Path, Size, Data, Overwrite, Args);
  VA_END(Args);
  return Status;
}
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
) {
  EFI_STATUS    Status;
  CHAR16       *FullPath;
  CONFIG_VALUE  Value;
  // Check parameters
  if ((Path == NULL) || (Data == NULL) || (Size == 0)) {
    return EFI_INVALID_PARAMETER;
  }
  // Create the path from the argument list
  FullPath = CatVSPrint(NULL, Path, Args);
  if (FullPath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  // Set the value
  Value.Data.Size = Size;
  Value.Data.Data = Data;
  Status = ConfigSetValue(FullPath, CONFIG_TYPE_DATA, &Value, Overwrite);
  FreePool(FullPath);
  return Status;
}

// mConfigProtocol
/// The configuration protocol
STATIC CONFIG_PROTOCOL mConfigProtocol = {
  (CONFIG_PARSE)ConfigParseXml,
  (CONFIG_FREE)ConfigPartialFree,
  (CONFIG_GET_LIST)ConfigGetList,
  (CONFIG_GET_COUNT)ConfigGetCount,
  (CONFIG_GET_VALUE)ConfigGetValue,
  (CONFIG_SET_VALUE)ConfigSetValue,
};

// ConfigLibInitialize
/// Configuration library initialize use
/// @return Whether the configuration initialized successfully or not
/// @retval EFI_SUCCESS The configuration successfully initialized
EFI_STATUS
EFIAPI
ConfigLibInitialize (
  VOID
) {
  // Check if configuration protocol is already installed
  mConfig = NULL;
  if (!EFI_ERROR(gBS->LocateProtocol(&mConfigGuid, NULL, (VOID **)&mConfig)) && (mConfig != NULL)) {
    return EFI_SUCCESS;
  }
  // Install configuration protocol
  mConfigHandle = NULL;
  mConfigProtocol.Parse = (CONFIG_PARSE)ConfigParseXml;
  mConfigProtocol.Free = (CONFIG_FREE)ConfigPartialFree;
  mConfigProtocol.GetList = (CONFIG_GET_LIST)ConfigGetList;
  mConfigProtocol.GetCount = (CONFIG_GET_COUNT)ConfigGetCount;
  mConfigProtocol.GetValue = (CONFIG_GET_VALUE)ConfigGetValue;
  mConfigProtocol.SetValue = (CONFIG_SET_VALUE)ConfigSetValue;
  return gBS->InstallMultipleProtocolInterfaces(&mConfigHandle, &mConfigGuid, (VOID *)&mConfigProtocol, NULL);
}

// ConfigLibFinish
/// Configuration library finish use
/// @return Whether the configuration was finished successfully or not
/// @retval EFI_SUCCESS The configuration successfully finished
EFI_STATUS
EFIAPI
ConfigLibFinish (
  VOID
) {
  // Uninstall configuration protocol
  mConfig = NULL;
  if (mConfigHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces(mConfigHandle, &mConfigGuid, (VOID *)&mConfigProtocol, NULL);
    mConfigHandle = NULL;
  }
  // Free all configuration tree nodes
  return ConfigFree();
}

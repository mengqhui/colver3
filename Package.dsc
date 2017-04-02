## @file
# EFI CloverPkg Package
#
#    This program and the accompanying materials
#    are licensed and made available under the terms and conditions of the BSD License
#    which accompanies this distribution. The full text of the license may be found at
#    http://opensource.org/licenses/bsd-license.php
#
#    THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
#    WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
#
##

[Defines]
  DSC_SPECIFICATION              = 0x00010006
  PLATFORM_NAME                  = $(PROJECT_NAME)
  PLATFORM_GUID                  = 6759CA12-7CEE-4368-AB2D-052ECCCBEEE9
  PLATFORM_VERSION               = $(PROJECT_VERSION_BASE)
  OUTPUT_DIRECTORY               = $(PROJECT_DIR_STAGE)
  SUPPORTED_ARCHITECTURES        = X64|IA32|IPF|ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

!if $(TARGET) == "DEBUG"
  PROJECT_DEBUG_OPTIONS=-DPROJECT_DEBUG
!endif

[Components]
  $(PROJECT_PACKAGE)/Application/GUI/GUI.inf

[LibraryClasses]

  #
  # GUI libraries
  #
  GUILib|$(PROJECT_PACKAGE)/Library/GUILib/GUILib.inf
  LogLib|$(PROJECT_PACKAGE)/Library/LogLib/LogLib.inf
  ParseLib|$(PROJECT_PACKAGE)/Library/ParseLib/ParseLib.inf
  XmlLib|$(PROJECT_PACKAGE)/Library/XmlLib/XmlLib.inf
  ConfigLib|$(PROJECT_PACKAGE)/Library/ConfigLib/ConfigLib.inf
  FontLib|$(PROJECT_PACKAGE)/Library/FontLib/FontLib.inf

  #
  # Device libraries
  #
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
  FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
  FileLib|$(PROJECT_PACKAGE)/Library/FileLib/FileLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  PciExpressLib|MdePkg/Library/BasePciExpressLib/BasePciExpressLib.inf
  PciLib|MdePkg/Library/BasePciLibPciExpress/BasePciLibPciExpress.inf
  SmbusLib|MdePkg/Library/DxeSmbusLib/DxeSmbusLib.inf

  #
  # CPU libraries
  #
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  PlatformLib|$(PROJECT_PACKAGE)/Library/PlatformLib/PlatformLib.inf
  TimerLib|$(PROJECT_PACKAGE)/Library/TimerLib/TimerLib.inf
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  SmBiosLib|$(PROJECT_PACKAGE)/Library/SmBiosLib/SmBiosLib.inf

  #
  # Misc libraries
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/UefiMemoryLib/UefiMemoryLib.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  StringLib|$(PROJECT_PACKAGE)/Library/StringLib/StringLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf

  #
  # UEFI Application libraries
  #
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf

  #
  # Debug libraries
  #
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf

!if $(TARGET) == "DEBUG"
  PlatformHookLib|MdeModulePkg/Library/BasePlatformHookLibNull/BasePlatformHookLibNull.inf
  SerialPortLib|MdeModulePkg/Library/BaseSerialPortLib16550/BaseSerialPortLib16550.inf
!else
  SerialPortLib|MdePkg/Library/BaseSerialPortLibNull/BaseSerialPortLibNull.inf
!endif

[LibraryClasses.IPF]
  PalLib|MdePkg/Library/UefiPalLib/UefiPalLib.inf

[BuildOptions]
  XCODE:*_*_*_CC_FLAGS = -Os -DDISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS) $(PROJECT_DEBUG_OPTIONS) $(PROJECT_ARCH_OPTIONS)
  GCC:*_*_*_CC_FLAGS = -Os -DDISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS) $(PROJECT_DEBUG_OPTIONS) $(PROJECT_ARCH_OPTIONS)
  MSFT:*_*_*_CC_FLAGS = /Os -DDISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS) $(PROJECT_DEBUG_OPTIONS) $(PROJECT_ARCH_OPTIONS)
  MSFT:*_*_*_DLINK_FLAGS = /INCREMENTAL:No

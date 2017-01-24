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
  SUPPORTED_ARCHITECTURES        = X64|IA32|ARM|AARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[Components]
  $(PROJECT_PACKAGE)/Application/GUI/GUI.inf

[LibraryClasses]

  #
  # GUI libraries
  #
  GUILib|$(PROJECT_PACKAGE)/Library/GUILib/GUILib.inf

  #
  # Device libraries
  #
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf

  #
  # CPU libraries
  #
  CpuLib|MdePkg/Library/BaseCpuLib/BaseCpuLib.inf
  CPUInformationLib|$(PROJECT_PACKAGE)/Library/CPUInformationLib/CPUInformationLib.inf
  DebugAgentLib|MdeModulePkg/Library/DebugAgentLibNull/DebugAgentLibNull.inf
  LocalApicLib|UefiCpuPkg/Library/BaseXApicX2ApicLib/BaseXApicX2ApicLib.inf
  MpInitLib|$(PROJECT_PACKAGE)/Library/MpInitLib/MpInitLib.inf
  MtrrLib|UefiCpuPkg/Library/MtrrLib/MtrrLib.inf
  UefiCpuLib|UefiCpuPkg/Library/BaseUefiCpuLib/BaseUefiCpuLib.inf

  #
  # Misc libraries
  #
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  BaseMemoryLib|MdePkg/Library/UefiMemoryLib/UefiMemoryLib.inf
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
  TimerLib|MdePkg/Library/BaseTimerLibNullTemplate/BaseTimerLibNullTemplate.inf

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
!if $(TARGET) == "DEBUG"
  DebugLib|MdePkg/Library/UefiDebugLibStdErr/UefiDebugLibStdErr.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!endif

[BuildOptions]
  XCODE:*_*_*_CC_FLAGS = -Os -D DISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS)
  GCC:*_*_*_CC_FLAGS = -Os -D DISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS)
  MSFT:*_*_*_CC_FLAGS = /Os /W4 /WX -D DISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS)

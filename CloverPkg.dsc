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
  SUPPORTED_ARCHITECTURES        = X64
  BUILD_TARGETS                  = DEBUG|RELEASE
  SKUID_IDENTIFIER               = DEFAULT

[Components]
  $(PROJECT_PACKAGE)/Application/GUI/GUI.inf

[BuildOptions]
  XCODE:*_*_*_CC_FLAGS = -Os -D DISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS)
  GCC:*_*_*_CC_FLAGS = -Os -D DISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS)
  MSFT:*_*_*_CC_FLAGS = /Os /W4 /WX -D DISABLE_NEW_DEPRECATED_INTERFACES $(PROJECT_BUILD_OPTIONS)

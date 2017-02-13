@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Post/30_BuildPackage.bat
rem
rem Build package
rem
rem ===========================================================================

if not defined PROJECT_ISO goto:eof

%TOOL_ALIGN% "Building ISO:" !PROJECT_ISO:%WORKSPACE%\=!

set MKISO_ARGUMENTS=-volid %PROJECT_SAFE_NAME%-%PROJECT_VERSION_BASE% -full-iso9660-filenames -root EFI -output "%PROJECT_ISO%"

%TOOL_MKISO% %MKISO_ARGUMENTS% %PROJECT_DIR_PACKAGE%\EFI 2>&1

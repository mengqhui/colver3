@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Config/20_ConfigureProject.bat
rem
rem Configure project options
rem
rem ===========================================================================

set PROJECT_NAME=

set PROJECT_ROOT_PATH=\\efi

set PROJECT_VERSION_FILE=%PROJECT_DIR_SUPPORT%\Version
set PROJECT_VERSION_MAJOR_DEFAULT=3
set PROJECT_VERSION_MINOR_DEFAULT=0
set PROJECT_VERSION_UPDATE_DEFAULT=0
set PROJECT_VERSION_TAG_DEFAULT=

set PROJECT_FIRMWARE_MAJOR=2
set PROJECT_FIRMWARE_MINOR=6
set PROJECT_FIRMWARE_UPDATE=0

set PROJECT_USER_VERSION=

rem ===========================================================================
rem Parse script arguments
rem ===========================================================================

:parseArguments

  set SCRIPT_ARGUMENT=%~1
  if not defined SCRIPT_ARGUMENT goto parseVersion
  if /i "%SCRIPT_ARGUMENT%" == "--project-name" (
    set PROJECT_NAME=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--project-version" (
    set PROJECT_USER_VERSION=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--project-version-file" (
    set PROJECT_VERSION_FILE=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--project-major" (
    set PROJECT_VERSION_MAJOR=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--project-minor" (
    set PROJECT_VERSION_MINOR=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--project-update" (
    set PROJECT_VERSION_UPDATE=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--project-tag" (
    set PROJECT_VERSION_TAG=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--firmware-major" (
    set PROJECT_FIRMWARE_MAJOR=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--firmware-minor" (
    set PROJECT_FIRMWARE_MINOR=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--firmware-update" (
    set PROJECT_FIRMWARE_UPDATE=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,15%" == "--project-name=" (
    set PROJECT_NAME=%SCRIPT_ARGUMENT:~15%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,18%" == "--project-version=" (
    set PROJECT_USER_VERSION=%SCRIPT_ARGUMENT:~18%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,23%" == "--project-version-file=" (
    set PROJECT_VERSION_FILE=%SCRIPT_ARGUMENT:~23%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,16%" == "--project-major=" (
    set PROJECT_VERSION_MAJOR=%SCRIPT_ARGUMENT:~16%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,16%" == "--project-minor=" (
    set PROJECT_VERSION_MINOR=%SCRIPT_ARGUMENT:~16%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,17%" == "--project-update=" (
    set PROJECT_VERSION_UPDATE=%SCRIPT_ARGUMENT:~17%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,14%" == "--project-tag=" (
    set PROJECT_VERSION_TAG=%SCRIPT_ARGUMENT:~14%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,17%" == "--firmware-major=" (
    set PROJECT_FIRMWARE_MAJOR=%SCRIPT_ARGUMENT:~17%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,17%" == "--firmware-minor=" (
    set PROJECT_FIRMWARE_MINOR=%SCRIPT_ARGUMENT:~17%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,18%" == "--firmware-update=" (
    set PROJECT_FIRMWARE_UPDATE=%SCRIPT_ARGUMENT:~18%
    shift
    goto parseArguments
  )
  shift
  goto parseArguments

rem ===========================================================================
rem Try to read the version from the version file
rem ===========================================================================

:parseVersion

%TOOL_UNQUOTE% PROJECT_NAME %PROJECT_NAME%
%TOOL_UNQUOTE% PROJECT_USER_VERSION %PROJECT_USER_VERSION%
%TOOL_UNQUOTE% PROJECT_VERSION_FILE %PROJECT_VERSION_FILE%
%TOOL_UNQUOTE% PROJECT_VERSION_TAG %PROJECT_VERSION_TAG%
call:checkNumber PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_UPDATE PROJECT_FIRMWARE_MAJOR PROJECT_FIRMWARE_MINOR PROJECT_FIRMWARE_UPDATE
if not defined PROJECT_USER_VERSION for /f "usebackq delims=" %%i in (` type "%PROJECT_VERSION_FILE%" `) do set PROJECT_USER_VERSION=%%i

rem ===========================================================================
rem Try to parse the version from user requested version
rem ===========================================================================

if not defined PROJECT_USER_VERSION goto getProjectName
set PROJECT_USER_VERSION=%PROJECT_USER_VERSION:"=%
set COUNTER=0

:parseUserMajorVersion

  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "" goto getUserMajorVersion
  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "." goto getUserMajorVersion
  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "-" goto getUserMajorTagVersion
  set /a COUNTER+=1
  goto parseUserMajorVersion

:getUserMajorTagVersion

  if not defined PROJECT_VERSION_MAJOR set PROJECT_VERSION_MAJOR=!PROJECT_USER_VERSION:~0,%COUNTER%!
  set /a COUNTER+=1
  goto getUserTagVersion

:getUserMajorVersion

  if %COUNTER% == 0 goto parseUserMinorVersion
  if not defined PROJECT_VERSION_MAJOR set PROJECT_VERSION_MAJOR=!PROJECT_USER_VERSION:~0,%COUNTER%!
  set /a COUNTER+=1
  set PROJECT_USER_VERSION=!PROJECT_USER_VERSION:~%COUNTER%!
  if not defined PROJECT_USER_VERSION goto getProjectName
  set COUNTER=0

:parseUserMinorVersion

  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "" goto getUserMinorVersion
  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "." goto getUserMinorVersion
  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "-" goto getUserMinorTagVersion
  set /a COUNTER+=1
  goto parseUserMinorVersion

:getUserMinorTagVersion

  if not defined PROJECT_VERSION_MINOR set PROJECT_VERSION_MINOR=!PROJECT_USER_VERSION:~0,%COUNTER%!
  set /a COUNTER+=1
  goto getUserTagVersion

:getUserMinorVersion

  if not define PROJECT_VERSION_MINOR set PROJECT_VERSION_MINOR=!PROJECT_USER_VERSION:~0,%COUNTER%!
  set /a COUNTER+=1
  set PROJECT_USER_VERSION=!PROJECT_USER_VERSION:~%COUNTER%!
  if not defined PROJECT_USER_VERSION goto getProjectName
  set COUNTER=0

:parseUserUpdateVersion

  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "" goto getUserUpdateVersion
  if "!PROJECT_USER_VERSION:~%COUNTER%,1!" == "-" goto getUserUpdateVersion
  set /a COUNTER+=1
  goto parseUserUpdateVersion

:getUserUpdateVersion

  if not defined PROJECT_VERSION_UPDATE set PROJECT_VERSION_UPDATE=!PROJECT_USER_VERSION:~0,%COUNTER%!
  set /a COUNTER+=1

:getUserTagVersion

  if defined PROJECT_VERSION_TAG (
    set PROJECT_VERSION_TAG=%PROJECT_VERSION_TAG%-!PROJECT_USER_VERSION:~%COUNTER%!
  ) else (
    set PROJECT_VERSION_TAG=!PROJECT_USER_VERSION:~%COUNTER%!
  )

rem ===========================================================================
rem Get the project name
rem ===========================================================================

:getProjectName

if not defined PROJECT_NAME set PROJECT_NAME=%PROJECT_PACKAGE:Pkg=%
set PROJECT_SAFE_NAME=%PROJECT_NAME%
%TOOL_SAFENAME% PROJECT_SAFE_NAME
%TOOL_ALIGN% Project: %PROJECT_NAME% (%PROJECT_SAFE_NAME%)

rem ===========================================================================
rem Get the build date and time
rem ===========================================================================

set PROJECT_DATETIME=
for /f "usebackq delims=" %%i in (` echo. ^| %TOOL_DATETIME% `) do set PROJECT_DATETIME=%%i
%TOOL_ALIGN% Date/Time: %PROJECT_DATETIME%

rem ===========================================================================
rem Determine the project version
rem ===========================================================================

call:checkNumber PROJECT_VERSION_MAJOR PROJECT_VERSION_MINOR PROJECT_VERSION_UPDATE
if not defined PROJECT_VERSION_MAJOR set PROJECT_VERSION_MAJOR=%PROJECT_VERSION_MAJOR_DEFAULT%
if not defined PROJECT_VERSION_MINOR set PROJECT_VERSION_MINOR=%PROJECT_VERSION_MINOR_DEFAULT%
if not defined PROJECT_VERSION_UPDATE set PROJECT_VERSION_UPDATE=%PROJECT_VERSION_UPDATE_DEFAULT%
set PROJECT_VERSION_BASE=%PROJECT_VERSION_MAJOR%.%PROJECT_VERSION_MINOR%
if %PROJECT_VERSION_UPDATE% gtr 0 set PROJECT_VERSION_BASE=%PROJECT_VERSION_BASE%.%PROJECT_VERSION_UPDATE%
set PROJECT_VERSION=%PROJECT_VERSION_BASE%
set PROJECT_VERSION_SHORT=%PROJECT_VERSION_BASE%
set PROJECT_VERSION_PUBLIC=%PROJECT_VERSION_BASE%

if defined BUILD_DEVELOPER (
  set PROJECT_VERSION=%PROJECT_VERSION%-%BUILD_DEVELOPER%
  if defined BUILD_BRANCH set PROJECT_VERSION=!PROJECT_VERSION!-%BUILD_BRANCH%
)

if not defined PROJECT_VERSION_TAG set PROJECT_VERSION_TAG=%PROJECT_VERSION_TAG_DEFAULT%
if defined PROJECT_VERSION_TAG (
  set PROJECT_VERSION=%PROJECT_VERSION%-%PROJECT_VERSION_TAG%
  set PROJECT_VERSION_PUBLIC=%PROJECT_VERSION_PUBLIC%-%PROJECT_VERSION_TAG%
)

for /f "usebackq delims=" %%i in (` "%TOOL_GIT%" rev-parse --short HEAD 2^>NUL `) do set PROJECT_VERSION_HASH=%%i
if defined PROJECT_VERSION_HASH (
  set PROJECT_VERSION=%PROJECT_VERSION%-%PROJECT_VERSION_HASH%
  set PROJECT_VERSION_SHORT=%PROJECT_VERSION_SHORT%-%PROJECT_VERSION_HASH%
  set PROJECT_VERSION_PUBLIC=%PROJECT_VERSION_PUBLIC%-%PROJECT_VERSION_HASH%
)

%TOOL_ALIGN% Version: %PROJECT_VERSION_BASE% (%PROJECT_VERSION_SHORT%) {%PROJECT_VERSION%}

rem ===========================================================================
rem Generate the version header for the project
rem ===========================================================================

set PROJECT_VERSION_HEADER=%PROJECT_DIR_BUILD%\Include\ProjectVersion.h

if defined BUILD_CLEAN goto:eof

%TOOL_MKDIR% /f "%PROJECT_VERSION_HEADER%"
if errorlevel 1 goto:eof
%TOOL_ALIGN% Generating: !PROJECT_VERSION_HEADER:%WORKSPACE%\=%!

echo // Autogenerated !PROJECT_VERSION_HEADER:%WORKSPACE%\=!>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_NAME L"%PROJECT_NAME%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_NAME_A "%PROJECT_NAME%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_SAFE_NAME L"%PROJECT_SAFE_NAME%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_SAFE_NAME_A "%PROJECT_SAFE_NAME%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_ROOT_PATH L"%PROJECT_ROOT_PATH%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_ROOT_PATH_A "%PROJECT_ROOT_PATH%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_VERSION L"%PROJECT_VERSION%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_VERSION_A "%PROJECT_VERSION%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_VERSIONCODE PACK_VERSIONCODE_FULL(%PROJECT_VERSION_MAJOR%, %PROJECT_VERSION_MINOR%, %PROJECT_VERSION_UPDATE%)>>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_DATETIME L"%PROJECT_DATETIME%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_DATETIME_A "%PROJECT_DATETIME%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_FIRMWARE_VERSION L"%PROJECT_FIRMWARE_MAJOR%.%PROJECT_FIRMWARE_MINOR%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_FIRMWARE_VERSION_A "%PROJECT_FIRMWARE_MAJOR%.%PROJECT_FIRMWARE_MINOR%">>"%PROJECT_VERSION_HEADER%"
echo #define _PROJECT_FIRMWARE_VERSIONCODE PACK_VERSIONCODE_FULL(%PROJECT_FIRMWARE_MAJOR%, %PROJECT_FIRMWARE_MINOR%, %PROJECT_FIRMWARE_UPDATE%)>>"%PROJECT_VERSION_HEADER%"

goto:eof

rem ===========================================================================
rem Make sure variable is a number
rem ===========================================================================

:checkNumber

  set NUM_INPUT=!%~1!
  if not defined NUM_INPUT goto:eof
  set NUM_INPUT=%NUM_INPUT:"=%
  set NUM_COUNTER=0
  set NUMBER=

  :parseNumber

    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "" goto returnNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "0" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "1" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "2" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "3" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "4" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "5" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "6" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "7" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "8" goto appendNumber
    if "!NUM_INPUT:~%NUM_COUNTER%,1!" == "9" goto appendNumber
    set /a NUM_COUNTER+=1
    goto parseNumber

    :appendNumber

      set NUMBER=%NUMBER%!NUM_INPUT:~%NUM_COUNTER%,1!
      set /a NUM_COUNTER+=1
      goto parseNumber

:returnNumber

  set %~1=%NUMBER%
  shift
  goto checkNumber


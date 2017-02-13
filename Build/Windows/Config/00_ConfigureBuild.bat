@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Config/00_ConfigureBuild.bat
rem
rem Configure build options
rem
rem ===========================================================================

set BUILD_CONFIG_DIR=
set BUILD_CONFIG_FILE=
set BUILD_TOOL_CHAIN_DEFAULT=VS2013x86
set BUILD_TARGET_DEFAULT=DEBUG
set BUILD_ARCH_DEFAULT=X64 IA32

set BUILD_DRYRUN=
set BUILD_CLEAN=
set BUILD_TOOL_CHAIN=
set BUILD_ARCH=
set BUILD_TARGET=
set BUILD_SOURCE=

set PROJECT_BUILD_ARGUMENTS=
set PROJECT_DIR_SUPPORT=
set PROJECT_DIR_STAGE=
set PROJECT_DIR_BUILD=

rem ===========================================================================
rem Parse build arguments
rem ===========================================================================

:parseArguments

   set SCRIPT_ARGUMENT=%~1
   if not defined SCRIPT_ARGUMENT goto setDefaultBuildOptions
   if /i "%SCRIPT_ARGUMENT%" == "clean" (
     set BUILD_CLEAN=Yes
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "cleanall" (
     set BUILD_CLEAN=All
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--dryrun" (
     set BUILD_DRYRUN=Yes
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--lite" (
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -DPROJECT_LITE
     set PROJECT_BUILD_OPTIONS=%PROJECT_BUILD_OPTIONS% -DPROJECT_LITE
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "-a" (
     set BUILD_ARCH=%~2
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "-p" (
     set BUILD_SOURCE=%~2
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "-b" (
     set BUILD_TARGET=%~2
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -b !BUILD_TARGET!
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "-t" (
     set BUILD_TOOL_CHAIN=%~2
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -t !BUILD_TOOL_CHAIN!
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--arch" (
     set BUILD_ARCH=%~2
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--platform" (
     set BUILD_SOURCE=%~2
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--buildtarget" (
     set BUILD_TARGET=%~2
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -b !BUILD_TARGET!
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--tagname" (
     set BUILD_TOOL_CHAIN=%~2
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -t !BUILD_TOOL_CHAIN!
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--build-dir" (
     set PROJECT_DIR_BUILD=%~2
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT%" == "--conf" (
     set BUILD_CONFIG_DIR=%~2
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT:~0,7%" == "--arch=" (
     set BUILD_ARCH=%SCRIPT_ARGUMENT:~7%
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT:~0,11%" == "--platform=" (
     set BUILD_SOURCE=%SCRIPT_ARGUMENT:~11%
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT:~0,14%" == "--buildtarget=" (
     set BUILD_TARGET=%SCRIPT_ARGUMENT:~14%
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -b !BUILD_TARGET!
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT:~0,10%" == "--tagname=" (
     set BUILD_TOOL_CHAIN=%SCRIPT_ARGUMENT:~10%
     set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% -t !BUILD_TOOL_CHAIN!
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT:~0,12%" == "--build-dir=" (
     set PROJECT_DIR_BUILD=%SCRIPT_ARGUMENT:~12%
     shift
     shift
     goto parseArguments
   )
   if /i "%SCRIPT_ARGUMENT:~0,7%" == "--conf=" (
     set BUILD_CONFIG_DIR=%SCRIPT_ARGUMENT:~7%
     shift
     goto parseArguments
   )
   shift
   goto parseArguments

rem ===========================================================================
rem Set the default build options
rem ===========================================================================

:setDefaultBuildOptions

%TOOL_UNQUOTE% BUILD_CONFIG_DIR %BUILD_CONFIG_DIR%
if defined BUILD_CONFIG_DIR set PROJECT_BUILD_ARGUMENTS=%PROJECT_BUILD_ARGUMENTS% --conf="%BUILD_CONFIG_DIR%"
if not defined BUILD_CONFIG_FILE (
  if defined BUILD_CONFIG_DIR (
    set BUILD_CONFIG_FILE=%BUILD_CONFIG_DIR%\target.txt
  ) else (
    set BUILD_CONFIG_FILE=%WORKSPACE%\Conf\target.txt
  )
)

rem ===========================================================================
rem Set default build options if options have no value
rem ===========================================================================

if not defined BUILD_TOOL_CHAIN set BUILD_TOOL_CHAIN=%BUILD_TOOL_CHAIN_DEFAULT%
if not defined BUILD_TARGET set BUILD_TARGET=%BUILD_TARGET_DEFAULT%
if not defined BUILD_ARCH set BUILD_ARCH=%BUILD_ARCH_DEFAULT%
if not defined BUILD_SOURCE set BUILD_SOURCE=%PROJECT_DIR_SOURCE%\Package.dsc

if not defined PROJECT_DIR_SUPPORT set PROJECT_DIR_SUPPORT=%PROJECT_DIR_SOURCE%\Build\Support
if not defined PROJECT_DIR_STAGE set PROJECT_DIR_STAGE=%PROJECT_DIR_SOURCE%\Build\Stage
%TOOL_UNQUOTE% PROJECT_DIR_BUILD %PROJECT_DIR_BUILD%
if not defined PROJECT_DIR_BUILD set PROJECT_DIR_BUILD=%PROJECT_DIR_STAGE%\%BUILD_TARGET%_%BUILD_TOOL_CHAIN%

rem ===========================================================================
rem Print the build options
rem ===========================================================================

%TOOL_ALIGN% Source: !BUILD_SOURCE:%WORKSPACE%\=!
%TOOL_ALIGN% "Build Stage:" !PROJECT_DIR_BUILD:%WORKSPACE%\=!
if defined BUILD_CLEAN (
  if /i "%BUILD_CLEAN%" == "Yes" (
    %TOOL_ALIGN% Clean: !PROJECT_DIR_BUILD:%WORKSPACE%\=!
  ) else if /i "%BUILD_CLEAN%" == "All" (
    %TOOL_ALIGN% Clean: !PROJECT_DIR_STAGE:%WORKSPACE%\=!
  )
)


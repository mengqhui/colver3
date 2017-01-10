@echo off
rem ===========================================================================
rem Configure package options
rem ===========================================================================

set PROJECT_DIR_PACKAGE=%PROJECT_DIR_SOURCE%\Package

set PROJECT_ISO=%PROJECT_DIR_PACKAGE%\%PROJECT_NAME%-%PROJECT_VERSION%.iso

rem ===========================================================================
rem Parse script arguments
rem ===========================================================================

:parseArguments

  set SCRIPT_ARGUMENT=%~1
  if not defined SCRIPT_ARGUMENT goto preparePackage
  if /i "%SCRIPT_ARGUMENT%" == "--no-iso" (
    set PROJECT_ISO=
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--iso" (
    set PROJECT_ISO=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--package-dir" (
    set PROJECT_DIR_PACKAGE=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,6%" == "--iso=" (
    set PROJECT_ISO=%SCRIPT_ARGUMENT:~6%
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,16%" == "--package-dir=" (
    set PROJECT_DIR_PACKAGE=%SCRIPT_ARGUMENTS:~16%
    shift
    goto parseArguments
  )
  shift
  goto parseArguments

rem ===========================================================================
rem Prepare package
rem ===========================================================================

:preparePackage

call "%TOOL_UNQUOTE%" PROJECT_DIR_PACKAGE %PROJECT_DIR_PACKAGE%
if not defined PROJECT_DIR_PACKAGE set PROJECT_DIR_PACKAGE=%PROJECT_DIR_SOURCE%\Package
%TOOL_ALIGN% Output: !PROJECT_DIR_PACKAGE:%WORKSPACE%\=!

call "%TOOL_UNQUOTE%" PROJECT_ISO %PROJECT_ISO%
if defined PROJECT_ISO %TOOL_ALIGN% ISO: !PROJECT_ISO:%WORKSPACE%\=!

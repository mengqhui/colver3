@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Config/40_ConfigureDocumentation.sh
rem
rem Configure documentation options
rem
rem ===========================================================================

set BUILD_DOCS=
set BUILD_DOCS_FILE=

set PROJECT_DIR_DOCS=%PROJECT_DIR_PACKAGE%\Documentation
set PROJECT_DIR_SUPPORT_DOCS=%PROJECT_DIR_SUPPORT%\Doxygen
set PROJECT_DOCS_INPUT=

set TOOL_DOXYGEN=

set DOCS_PUBLIC=

rem ===========================================================================
rem Parse script arguments
rem ===========================================================================

:parseArguments

  set SCRIPT_ARGUMENT=%~1
  if not defined SCRIPT_ARGUMENT goto configureDoxygen
  if /i "%SCRIPT_ARGUMENT%" == "--no-docs" (
    set BUILD_DOCS=
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--dev-docs" (
    set BUILD_DOCS=Yes
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--docs" (
    set BUILD_DOCS=Yes
    set DOCS_PUBLIC=Yes
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--dev-docs-only" (
    set BUILD_DOCS=Yes
    set BUILD_DRYRUN=Yes
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--docs-only" (
    set BUILD_DOCS=Yes
    set BUILD_DRYRUN=Yes
    set DOCS_PUBLIC=Yes
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--docs-dir" (
    set PROJECT_DIR_DOCS=%~2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT:~0,11%" == "--docs-dir=" (
    set PROJECT_DIR_DOCS=%SCRIPT_ARGUMENT:~11%
    shift
    goto parseArguments
  )
  shift
  goto parseArguments

rem ===========================================================================
rem Configure doxygen
rem ===========================================================================

:configureDoxygen

if not defined TOOL_DOXYGEN for /f "usebackq delims=" %%i in (` "%TOOL_WHERE%" doxygen 2^>NUL `) do set TOOL_DOXYGEN=%%~i
if not defined TOOL_DOXYGEN (
  set BUILD_DOCS=
  %TOOL_ALIGN% doxygen: Not found
  goto:eof
)
%TOOL_ALIGN% doxygen: %TOOL_DOXYGEN%

rem ===========================================================================
rem Prepare documentation
rem ===========================================================================

%TOOL_UNQUOTE% PROJECT_DIR_DOCS %PROJECT_DIR_DOCS%
if not defined PROJECT_DIR_DOCS set PROJECT_DIR_DOCS=%PROJECT_DIR_PACKAGE%\Documentation
if defined BUILD_DOCS %TOOL_ALIGN% Documentation: !PROJECT_DIR_DOCS:%WORKSPACE%\=!

%TOOL_UNQUOTE% BUILD_DOCS_FILE %BUILD_DOCS_FILE%
if not defined BUILD_DOCS_FILE set BUILD_DOCS_FILE=%PROJECT_DIR_SUPPORT_DOCS%\Doxyfile
if defined BUILD_DOCS %TOOL_ALIGN% Doxyfile: !BUILD_DOCS_FILE:%WORKSPACE%\=!

set PROJECT_DOCS_VERSION=%PROJECT_VERSION_PUBLIC%
if not defined DOCS_PUBLIC (
  if defined BUILD_DEVELOPER (
    set PROJECT_DOCS_VERSION=%PROJECT_VERSION%
    set PROJECT_DOCS_INPUT=%PROJECT_DOCS_INPUT% "$(PROJECT_DIR_SOURCE)/Library" "$(PROJECT_DIR_SOURCE)/Application"
  )
)

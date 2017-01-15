@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Support/InternalBuild.bat
rem
rem Project package internal build script
rem
rem ===========================================================================

setlocal enableDelayedExpansion

set PROJECT_BUILD_OPTIONS=
set PROJECT_BUILD_ARGUMENTS=

rem =========================================================================
rem Print start time and build information
rem =========================================================================

call:printStripe Build started at %BUILD_START_TIME% on %BUILD_START_DATE%
%TOOL_ALIGN% Parameters: %*
%TOOL_ALIGN% Workspace: %WORKSPACE%
%TOOL_ALIGN% Package: %PROJECT_PACKAGE%
if defined BUILD_LOG %TOOL_ALIGN% BuildLog: %BUILD_LOG%
echo.

rem =========================================================================
rem Execute the build configuration scripts
rem =========================================================================

call:printStripe Executing build configuration scripts
for /r "%BUILD_DIR_CONFIG%" %%v in (*.bat) do (
  set BUILD_SCRIPT=%%v
  %TOOL_ALIGN% Executing: !BUILD_SCRIPT:%BUILD_DIR_CONFIG%\=!
  call "%%v" %*
  if errorlevel 1 goto failScript
  echo.
)

rem =========================================================================
rem Check if building or cleaning
rem =========================================================================

if defined BUILD_CLEAN (

  rem =======================================================================
  rem Clean the build
  rem =======================================================================

  call:printStripe Cleaning package
  if not exist "%PROJECT_DIR_PACKAGE%" (
    %TOOL_ALIGN% Cleaned: !PROJECT_DIR_PACKAGE:%WORKSPACE%\=!
  ) else (
    call "%TOOL_CLEAN%" /r "%PROJECT_DIR_PACKAGE%"
  )
  if /i "%BUILD_CLEAN%" == "All" (
    if not exist "%PROJECT_DIR_STAGE%" (
      %TOOL_ALIGN% Cleaned: !PROJECT_DIR_STAGE:%WORKSPACE%\=!
    ) else (
      call "%TOOL_CLEAN%" /r "%PROJECT_DIR_STAGE%"
    )
    if not exist "%PROJECT_DIR_LOG%" (
      %TOOL_ALIGN% Cleaned: !PROJECT_DIR_LOG:%WORKSPACE%\=!
    ) else (
      call "%TOOL_CLEAN%" /r "%PROJECT_DIR_LOG%"
    )
  ) else (
    if not exist "%PROJECT_DIR_BUILD%" (
      %TOOL_ALIGN% Cleaned: !PROJECT_DIR_BUILD:%WORKSPACE%\=!
    ) else (
      call "%TOOL_CLEAN%" /r "%PROJECT_DIR_BUILD%"
    )
  )
  echo.
  if errorlevel 1 goto failScript
  goto exitScript

)

rem =========================================================================
rem Build package
rem =========================================================================

if defined BUILD_DRYRUN (
  call:printStripe DRY RUN: Skipping building package
  goto postBuild
)
set PROJECT_DIR_STAGE_RELATIVE=!PROJECT_DIR_STAGE:%WORKSPACE%\=!
set PROJECT_BUILD_OPTIONS=%PROJECT_BUILD_OPTIONS% /I"%PROJECT_DIR_BUILD%\Include"
set BUILD_PROJECT_ARGUMENTS=--platform="%BUILD_SOURCE%" -D "PROJECT_DIR_STAGE=%PROJECT_DIR_STAGE_RELATIVE:\=/%"
set BUILD_PROJECT_ARGUMENTS=%BUILD_PROJECT_ARGUMENTS% -D "PROJECT_NAME=%PROJECT_NAME%" -D "PROJECT_SAFE_NAME=%PROJECT_SAFE_NAME%" -D "PROJECT_PACKAGE=%PROJECT_PACKAGE%"
call:printStripe Building package
%TOOL_ALIGN% Architecture: %BUILD_ARCH%
if defined PROJECT_BUILD_ARGUMENTS %TOOL_ALIGN% Arguments: %PROJECT_BUILD_ARGUMENTS%
echo.

for %%i in (%BUILD_ARCH%) do (
  call:buildArchitecture %%i
  if errorlevel 1 goto failScript
)

rem =========================================================================
rem Execute the post build scripts
rem =========================================================================

:postBuild

call:printStripe Executing post build scripts
for /r "%BUILD_DIR_POST%" %%v in (*.bat) do (
  set BUILD_SCRIPT=%%v
  %TOOL_ALIGN% Executing: !BUILD_SCRIPT:%BUILD_DIR_POST%\=!
  call "%%v"
  if errorlevel 1 goto failScript
  echo.
)

rem ===========================================================================
rem Exit the script
rem ===========================================================================

:exitScript

  call:buildTime
  call:printStripe Build finished at %BUILD_FINISH_TIME% on %BUILD_FINISH_DATE% after %BUILD_DURATION%
  popd
  endlocal
  goto:eof

rem ===========================================================================
rem Fail the script
rem ===========================================================================

:failScript

  call:buildTime
  call:printStripe Build failed at %BUILD_FINISH_TIME% on %BUILD_FINISH_DATE% after %BUILD_DURATION%
  popd
  endlocal
  exit /b 1

rem =========================================================================
rem Build for architecture
rem =========================================================================

:buildArchitecture

  call:printStripe Building package for %~1
  set ARCH=%~1
  %TOOL_ALIGN% Stage(%~1): !PROJECT_DIR_BUILD:%WORKSPACE%\=!\%~1
  echo.
  %PYTHONPATH%\build\build.py --arch=%~1 %BUILD_PROJECT_ARGUMENTS% %PROJECT_BUILD_ARGUMENTS% 2>&1
  rem build --arch=%~1 %BUILD_PROJECT_ARGUMENTS% %PROJECT_BUILD_ARGUMENTS% 2>&1
  goto:eof

rem =========================================================================
rem Get the total build time
rem =========================================================================

:buildTime

  set BUILD_FINISH_DATE=%date%
  set BUILD_FINISH_TIME=%time: =0%
  set /a BUILD_FINISH=(1%BUILD_FINISH_TIME:~0,2%-100)*360000 + (1%BUILD_FINISH_TIME:~3,2%-100)*6000 + (1%BUILD_FINISH_TIME:~6,2%-100)*100 + (1%BUILD_FINISH_TIME:~9,2%-100)
  set /a BUILD_DURATION=%BUILD_FINISH%-%BUILD_START%
  set /a BUILD_DURATION_HOURS=%BUILD_DURATION%/360000
  if %BUILD_DURATION_HOURS% lss 10 set BUILD_DURATION_HOURS=0%BUILD_DURATION_HOURS%
  set /a BUILD_DURATION%%=360000
  set /a BUILD_DURATION_MINS=%BUILD_DURATION%/6000
  if %BUILD_DURATION_MINS% lss 10 set BUILD_DURATION_MINS=0%BUILD_DURATION_MINS%
  set /a BUILD_DURATION%%=6000
  set /a BUILD_DURATION_SECS=%BUILD_DURATION%/100
  if %BUILD_DURATION_SECS% lss 10 set BUILD_DURATION_SECS=0%BUILD_DURATION_SECS%
  set /a BUILD_DURATION%%=100
  set BUILD_DURATION=%BUILD_DURATION_HOURS%:%BUILD_DURATION_MINS%:%BUILD_DURATION_SECS%.%BUILD_DURATION%
  goto:eof

rem =========================================================================
rem Print a striped message
rem =========================================================================

:printStripe

  echo --------------------------------------------------------------------------------
  echo %*
  echo --------------------------------------------------------------------------------
  echo.
  goto:eof


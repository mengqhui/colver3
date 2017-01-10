@echo off
rem ===========================================================================
rem Clean directory script
rem ===========================================================================

setlocal enableDelayedExpansion

set CLEAN_RECURSE=
set CLEAN_PATH=
set CLEAN_ARGUMENT=%~1
if not defined CLEAN_ARGUMENT goto:eof

if /i "%CLEAN_ARGUMENT%" == "/r" (
  set CLEAN_RECURSE=Yes
  shift
)

set CLEAN_PATH=%~dpf1
if not defined CLEAN_PATH goto:eof
if defined CLEAN_RECURSE goto recursePath

:cleanPath

  del /f /q "%CLEAN_PATH%" >NUL 2>&1
  if errorlevel 1 goto notCleaned
  if exist "%CLEAN_PATH%" goto notCleaned
  %TOOL_ALIGN% Cleaned: !CLEAN_PATH:%WORKSPACE%\=!
  goto:eof

:recursePath

  if not exist "%CLEAN_PATH%\*" goto cleanPath
  for /d %%i in ("%CLEAN_PATH%\*") do (
    call "%TOOL_CLEAN%" /r "%%~i"
  )
  for %%i in ("%CLEAN_PATH%\*") do (
    call "%TOOL_CLEAN%" "%%~i"
  )
  rmdir /q "%CLEAN_PATH%" >NUL 2>&1
  if errorlevel 1 goto notCleaned
  if exist "%CLEAN_PATH%" goto notCleaned
  %TOOL_ALIGN% Cleaned: !CLEAN_PATH:%WORKSPACE%\=!
  goto:eof

:notCleaned

  %TOOL_ALIGN% "Not Cleaned:" !CLEAN_PATH:%WORKSPACE%\=!


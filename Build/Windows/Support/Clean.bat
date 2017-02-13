@echo off
rem ===========================================================================
rem Clean directory script
rem ===========================================================================

setlocal enableDeleyedExpansion

set CLEAN_RECURSE=
if /i "%~1" == "/r" (
  set CLEAN_RECURSE=Yes
  set CLEAN_PATH=%~dpf2
) else (
  set CLEAN_PATH=%~dpf1
)

if not defined CLEAN_PATH goto:eof
if not defined CLEAN_RECURSE goto cleanPath
rem if not exist "%CLEAN_PATH%\*" goto cleanPath

for /d %%i in ("%CLEAN_PATH%\*") do (
  %TOOL_CLEAN% /r "%%~i"
)
for %%i in ("%CLEAN_PATH%\*") do (
  %TOOL_CLEAN% "%%~i"
)
rmdir /q "%CLEAN_PATH%" >NUL 2>&1

:cleanPath

  del /f /q "%CLEAN_PATH%" >NUL 2>&1
  if exist "%CLEAN_PATH%" (
    %TOOL_ALIGN% "Not Cleaned:" !CLEAN_PATH:%WORKSPACE%\=!
  ) else (
    %TOOL_ALIGN% Cleaned: !CLEAN_PATH:%WORKSPACE%\=!
  )

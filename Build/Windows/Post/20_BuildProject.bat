@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Post/20_BuildProject.bat
rem
rem Build project
rem
rem ===========================================================================

if defined BUILD_DRYRUN goto:eof

set PROJECT_DIR_OUTPUT=%PROJECT_DIR_PACKAGE%\EFI

%TOOL_CLEAN% /r "%PROJECT_DIR_OUTPUT%"

call:copyFile "%PROJECT_DIR_SOURCE%\Application\GUI\Config.xml" "%PROJECT_SAFE_NAME%\%PROJECT_SAFE_NAME%.xml"
if errorlevel 1 goto:eof

for %%i in (%BUILD_ARCH%) do (
  call:prepareProject %%i
  if errorlevel 1 goto:eof
)

goto:eof

:prepareProject

  echo.
  %TOOL_ALIGN% Preparing: %~1

  call:copyFile "%PROJECT_DIR_SOURCE%\Application\GUI\Config.%~1.xml" "%PROJECT_SAFE_NAME%\%~1\%PROJECT_SAFE_NAME%.xml"
  if errorlevel 1 goto:eof
  call:copyFile "%PROJECT_DIR_BUILD%\%~1\GUI.efi" "BOOT\BOOT%~1.efi"
  if errorlevel 1 goto:eof
  for /f "usebackq tokens=*" %%i in (` dir /A-D /B "%PROJECT_DIR_BUILD%\%~1\*.efi" `) do (
    call:copyFile "%PROJECT_DIR_BUILD%\%~1\%%~i" "%PROJECT_SAFE_NAME%\%~1\%%~i"
    if errorlevel 1 goto:eof
  )

  goto:eof

:copyFile

  set COPY_FROM=%~1
  set COPY_TO=%PROJECT_DIR_OUTPUT%\%~2
  %TOOL_ALIGN% Copying: !COPY_FROM:%WORKSPACE%\=! =^> !COPY_TO:%WORKSPACE%\=!
  xcopy /F /Q /R /Y "%COPY_FROM%" "%COPY_TO%*" >NUL 2>&1
  if not exist "%COPY_TO%" (
    %TOOL_ALIGN% "Failed to copy:" !COPY_TO:%WORKSPACE%\=!
    echo.
    exit /b 1
  )

  goto:eof

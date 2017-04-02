@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Post/20_BuildProject.bat
rem
rem Build project
rem
rem ===========================================================================

if defined BUILD_DRYRUN goto:eof

set PROJECT_DIR_STAGE=%PROJECT_DIR_OUTPUT%\Stage

%TOOL_CLEAN% /r "%PROJECT_DIR_STAGE%"

call:copyFile "%PROJECT_DIR_SOURCE%\Application\GUI\Config.xml" "EFI\%PROJECT_SAFE_NAME%\%PROJECT_SAFE_NAME%.XML"
if not !errorlevel! == 0 goto:eof

for %%i in (%BUILD_ARCH%) do (
  call:prepareProject %%i
  if not !errorlevel! == 0 goto:eof
)

goto:eof

:prepareProject

  echo.
  %TOOL_ALIGN% Preparing: %~1

  if not !errorlevel! == 0 goto:eof
  call:copyFile "%PROJECT_DIR_BUILD%\%~1\GUI.efi" "EFI\BOOT\BOOT%~1.EFI"
  if not !errorlevel! == 0 goto:eof
  for /f "usebackq tokens=*" %%i in (` dir /A-D /B "%PROJECT_DIR_BUILD%\%~1\*.EFI" `) do (
    call:copyFile "%PROJECT_DIR_BUILD%\%~1\%%~i" "EFI\%PROJECT_SAFE_NAME%\%~1\%%~i"
    if not !errorlevel! == 0 goto:eof
  )

  goto:eof

:copyFile

  set COPY_FROM=%~1
  set COPY_TO=%PROJECT_DIR_STAGE%\%~2
  %TOOL_ALIGN% Copying: !COPY_FROM:%WORKSPACE%\=! =^> !COPY_TO:%WORKSPACE%\=!
  xcopy /F /Q /R /Y "%COPY_FROM%" "%COPY_TO%*" >NUL 2>&1
  if not exist "%COPY_TO%" (
    %TOOL_ALIGN% "Failed to copy:" !COPY_TO:%WORKSPACE%\=!
    echo.
    exit /b 1
  )

  goto:eof

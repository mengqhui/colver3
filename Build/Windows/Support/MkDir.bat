@echo off
rem ===========================================================================
rem Make directory script
rem ===========================================================================

if /i "%~1" == "/f" (
  set MKDIR_DIR=%~dp2
) else (
  set MKDIR_DIR=%~dpf1
)

if not defined MKDIR_DIR goto:eof

if "%MKDIR_DIR:~-1%" == "\" set MKDIR_DIR=%MKDIR_DIR:~0,-1%
if exist "%MKDIR_DIR%" goto:eof

%TOOL_ALIGN% Creating: !MKDIR_DIR:%WORKSPACE%\=!
mkdir "%MKDIR_DIR%"
if not exist "%MKDIR_DIR%" exit /b 1


@echo off
rem ===========================================================================
rem Make directory script
rem ===========================================================================

setlocal

set MKDIR_DIR=
set MKDIR_ARGUMENT=%~1
if not defined MKDIR_ARGUMENT goto endScript

if /i "%MKDIR_ARGUMENT%" == "/f" (
  set MKDIR_DIR=%~dp2
  if not defined MKDIR_DIR goto endScript
)

if not defined MKDIR_DIR set MKDIR_DIR=%~dpf1
if not defined MKDIR_DIR goto endScript

if "%MKDIR_DIR:~-1%" == "\" set MKDIR_DIR=%MKDIR_DIR:~0,-1%
if exist "%MKDIR_DIR%" goto endScript

%TOOL_ALIGN% Creating: !MKDIR_DIR:%WORKSPACE%\=!
mkdir "%MKDIR_DIR%"
if not exist "%MKDIR_DIR%" exit /b 1

:endScript

  endlocal

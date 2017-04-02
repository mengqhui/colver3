@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Config/10_ConfigureDeveloper.bat
rem
rem Configure developer options
rem
rem ===========================================================================

set BUILD_DEVELOPER=

rem ===========================================================================
rem Configure git
rem ===========================================================================

set TOOL_GIT=
for /f "usebackq delims=" %%i in (` %TOOL_WHERE% git 2^>NUL `) do set TOOL_GIT="%%~i"

if not defined TOOL_GIT (
  %TOOL_ALIGN% git: Not found
  goto:eof
)

%TOOL_ALIGN% git: %TOOL_GIT%

rem ===========================================================================
rem Get developer
rem ===========================================================================

for /f "usebackq delims=" %%i in (` %TOOL_GIT% config user.name 2^>NUL `) do for /f "usebackq delims=" %%j in (` echo. ^| %TOOL_GIT% log --max-count=1 --author="%%i" 2^>NUL `) do set BUILD_DEVELOPER=%%i
if not defined BUILD_DEVELOPER (
  %TOOL_ALIGN% Developer: Not a development build
  goto:eof
)

%TOOL_ALIGN% Developer: %BUILD_DEVELOPER%

rem ===========================================================================
rem Get branch
rem ===========================================================================

set BUILD_BRANCH=
for /f "usebackq delims=" %%i in (` %TOOL_GIT% rev-parse --abbrev-ref HEAD 2^>NUL `) do set BUILD_BRANCH=%%i
if "%BUILD_BRANCH%" == "master" set BUILD_BRANCH=

if defined BUILD_BRANCH %TOOL_ALIGN% Branch: %BUILD_BRANCH%


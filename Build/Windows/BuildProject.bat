@echo off
rem ===========================================================================
rem
rem @file Build/Windows/BuildProject.bat
rem
rem Project package build script
rem
rem ===========================================================================

setlocal enableDelayedExpansion

rem ===========================================================================
rem Setup the build script directories
rem ===========================================================================

set BUILD_DIR_SUPPORT=%~dp0Support
set BUILD_DIR_CONFIG=%~dp0Config
set BUILD_DIR_CLEAN=%~dp0Clean
set BUILD_DIR_POST=%~dp0Post

rem ===========================================================================
rem Setup the build tools
rem ===========================================================================

set TOOL_BUILD=%BUILD_DIR_SUPPORT%\InternalBuild.bat
set TOOL_CLEAN=call "%BUILD_DIR_SUPPORT%\Clean.bat"
set TOOL_MKDIR=call "%BUILD_DIR_SUPPORT%\MkDir.bat"
set TOOL_UNQUOTE=call "%BUILD_DIR_SUPPORT%\Unquote.bat"
set TOOL_SAFENAME=call "%BUILD_DIR_SUPPORT%\SafeName.bat"

set TOOL_MKISO=%BUILD_DIR_SUPPORT%\mkisofs.exe
set TOOL_CSCRIPT=%SYSTEMROOT%\System32\cscript.exe
set TOOL_FIND=%SYSTEMROOT%\System32\find.exe
set TOOL_WHERE=%SYSTEMROOT%\System32\where.exe

set TOOL_ALIGN="%TOOL_CSCRIPT%" /Nologo "%BUILD_DIR_SUPPORT%\Align.vbs"
set TOOL_DATETIME="%TOOL_CSCRIPT%" /Nologo "%BUILD_DIR_SUPPORT%\DateTime.vbs"
set TOOL_TEE="%TOOL_CSCRIPT%" /Nologo "%BUILD_DIR_SUPPORT%\Tee.vbs"

set NASM_PREFIX=%BUILD_DIR_SUPPORT%\

rem ===========================================================================
rem Check if run from console
rem ===========================================================================

set BUILD_RUNFROMCONSOLE=No
for /f "usebackq delims=" %%i in (` echo %cmdcmdline:"=-% ^| "%TOOL_FIND%" /i "%~dpf0" `) do set BUILD_RUNFROMCONSOLE=Yes

rem ===========================================================================
rem Search for the EDK2 workspace which should be parent of this script
rem ===========================================================================

pushd .
if defined WORKSPACE if exist "%WORKSPACE%" goto prepareBuild

:searchForEDK2Workspace

  if exist edksetup.bat (
    @call edksetup.bat >NUL 2>&1
    goto prepareBuild
  )
  if /i "%CD%" == "%~d0%\" (
    echo No EDK2 workspace found. Did you place the sources inside of EDK2 workspace?
    goto failScript
  )
  cd ..
  goto searchForEDK2Workspace

rem ===========================================================================
rem Prepare to build
rem ===========================================================================

:prepareBuild

  rem =========================================================================
  rem Set the build date and time
  rem =========================================================================

  set BUILD_START_DATE=%date%
  set BUILD_START_TIME=%time: =0%
  set /a BUILD_START=(1%BUILD_START_TIME:~0,2%-100)*360000 + (1%BUILD_START_TIME:~3,2%-100)*6000 + (1%BUILD_START_TIME:~6,2%-100)*100 + (1%BUILD_START_TIME:~9,2%-100)

  rem =========================================================================
  rem Get the path of this script to determine the source directory
  rem =========================================================================

  set TEMP_PACKAGE=%~dp0
  set TEMP_PACKAGE=!TEMP_PACKAGE:%WORKSPACE%\=!
  set COUNTER=0

  rem =========================================================================
  rem Get the package directory from the script path
  rem =========================================================================

  :getSourceDirectory

    if "!TEMP_PACKAGE:~%COUNTER%,1!" == "" goto checkSourceDirectory
    if "!TEMP_PACKAGE:~%COUNTER%,1!" == "\" goto checkSourceDirectory
    set /a COUNTER+=1
    goto getSourceDirectory

  rem =========================================================================
  rem Set the default package and source directory
  rem =========================================================================

  :checkSourceDirectory

    if %COUNTER% leq 0 (
      echo No package source path could be found. Are you running this script from inside a package directory of the EDK2 workspace?
      goto failScript
    )
    set PROJECT_PACKAGE=!TEMP_PACKAGE:~0,%COUNTER%!
    set PROJECT_DIR_SOURCE=%WORKSPACE%\%PROJECT_PACKAGE%
    set PACKAGES_PATH=%PROJECT_DIR_SOURCE%

  rem =========================================================================
  rem Set the default log directory
  rem =========================================================================

  set PROJECT_DIR_LOG=%PROJECT_DIR_SOURCE%\Build\Logs

  rem =========================================================================
  rem Set the default build log file
  rem =========================================================================

  set BUILD_LOG=%PROJECT_DIR_LOG%\%PROJECT_PACKAGE%-%BUILD_START_DATE:~4,2%-%BUILD_START_DATE:~7,2%-%BUILD_START_DATE:~10,4%-%BUILD_START_TIME::=.%.log

rem ===========================================================================
rem Parse script arguments for the log file, version and help
rem ===========================================================================

:parseArguments

  set SCRIPT_ARGUMENT=%~1
  if not defined SCRIPT_ARGUMENT goto startBuild
  if /i "%SCRIPT_ARGUMENT%" == "--no-log" (
    set BUILD_LOG=
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "cleanall" (
    set BUILD_LOG=
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "--build-log" (
    set BUILD_LOG=%~fp2
    shift
    shift
    goto parseArguments
  )
  if /i "%SCRIPT_ARGUMENT%" == "-?" (
    build --help
    goto exitScript
  )
  if /i "%SCRIPT_ARGUMENT%" == "-h" (
    build --help
    goto exitScript
  )
  if /i "%SCRIPT_ARGUMENT%" == "--help" (
    build --help
    goto exitScript
  )
  if /i "%SCRIPT_ARGUMENT%" == "--version" (
    build --version
    goto exitScript
  )
  shift
  goto parseArguments

rem ===========================================================================
rem Start build
rem ===========================================================================

:startBuild

  if defined BUILD_LOG (
    %TOOL_MKDIR% "%PROJECT_DIR_LOG%"
    if errorlevel 1 goto failScript
  )

  if not defined PYTHONPATH (
    set PYTHONPATH=%BASE_TOOLS_PATH%\Source\Python
  )

  cd %PROJECT_DIR_SOURCE%
  set OUTLOG=5^>NUL 2^>^&5
  if defined BUILD_LOG set OUTLOG=5^>"%BUILD_LOG%" 2^>^&5
  %OUTLOG% ( "%TOOL_BUILD%" %* | %TOOL_TEE% )

rem ===========================================================================
rem Exit the script
rem ===========================================================================

:exitScript

  popd
  if /i "%BUILD_RUNFROMCONSOLE%" == "Yes" pause
  endlocal
  goto:eof

rem ===========================================================================
rem Fail the script
rem ===========================================================================

:failScript

  popd
  if /i "%BUILD_RUNFROMCONSOLE%" == "Yes" pause
  endlocal
  exit /b 1

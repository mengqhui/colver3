@echo off
rem ===========================================================================
rem Generate documentation
rem ===========================================================================

if not defined BUILD_DOCS goto:eof
if not defined BUILD_DOCS_FILE goto:eof
if not defined PROJECT_DIR_DOCS goto:eof
if not defined TOOL_DOXYGEN goto:eof

call "%TOOL_MKDIR%" "%PROJECT_DIR_DOCS%"
echo.
if not exist "%PROJECT_DIR_DOCS%" exit /b 1

"%TOOL_DOXYGEN%" "%BUILD_DOCS_FILE%" 2>&1


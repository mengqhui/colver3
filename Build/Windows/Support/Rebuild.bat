@echo off
call "%~dp0\..\BuildProject.bat" %* clean
if errorlevel 1 goto:eof
call "%~dp0\..\BuildProject.bat" %*

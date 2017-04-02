@echo off
rem ===========================================================================
rem
rem @file Build/Windows/Post/30_BuildPackage.bat
rem
rem Build package
rem
rem ===========================================================================

rem ===========================================================================
rem Set the default arguments
rem ===========================================================================

set MKISO_ARGUMENTS=-v -r -force-uppercase -V "%PROJECT_SAFE_NAME%" -eltorito-catalog "BOOT.CATALOG" -hide "BOOT.CATALOG"
set MKISO_LEGACY=
set MKISO_EFI=
set MKISO_HIDDEN=

set BOOT_LOADERS=MBR FAT ISO

set ISO_LOADER=%PROJECT_DIR_STAGE%\BOOTISO
set ISO_LOADER_PATH=BOOTISO
set ISO_LOADER_SOURCE=%PROJECT_DIR_SUPPORT%\Boot\bootiso.s

set MBR_LOADER=%PROJECT_DIR_STAGE%\BOOTMBR
set MBR_LOADER_PATH=BOOTMBR
set MBR_LOADER_SOURCE=%PROJECT_DIR_SUPPORT%\Boot\bootmbr.s

set FAT_LOADER=%PROJECT_DIR_STAGE%\BOOTFAT
set FAT_LOADER_PATH=BOOTFAT
set FAT_LOADER_SOURCE=%PROJECT_DIR_SUPPORT%\Boot\bootfat.s

set BOOT_IMAGE_SIZE=0
set BOOT_IMAGE=%ISO_LOADER%
set BOOT_IMAGE_ADDRESS=0x07C0

set LEGACY_IMAGE_SIZE=0
set LEGACY_IMAGE_PATH=BOOTISO.IMG
set LEGACY_IMAGE_INT=%PROJECT_DIR_BUILD%\BOOTISO.IMG
set LEGACY_IMAGE=%PROJECT_DIR_STAGE%\BOOTISO.IMG

set EFI_IMAGE_SIZE=0
set EFI_IMAGE_PATH=BOOTEFI.IMG
set EFI_IMAGE=%PROJECT_DIR_STAGE%\BOOTEFI.IMG

rem ===========================================================================
rem Build boot images
rem ===========================================================================

echo.
for %%v in (%BOOT_LOADERS%) do (
  set LOADER=!%%v_LOADER!
  set LOADER_SOURCE=!%%~v_LOADER_SOURCE!
  %TOOL_ALIGN% "Boot image:" Generating !LOADER:%WORKSPACE%\=!
  %TOOL_NASM% -Xvc -Wall -Werror -O2 -f bin -o "!LOADER!" "!LOADER_SOURCE!" 2>&1
  if not !errorlevel! == 0 (
    echo.
    exit /b 1
  )
  if not exist "!LOADER!" (
    echo.
    exit /b 1
  )
  call:getBootSize "!LOADER!"
  set MKISO_HIDDEN=!MKISO_HIDDEN! -hide "BOOT%%~v"
)
call:getBootSize "%BOOT_IMAGE%" false

rem ===========================================================================
rem Build legacy image
rem ===========================================================================

echo.
%TOOL_ALIGN% "Legacy image:" Determining size of image

for %%i in (%BUILD_ARCH%) do (
  call:addArchLegacySize %%i "%PROJECT_DIR_STAGE%\BOOT%%i"
  if not !errorlevel! == 0 exit /b 1
)
call:addArchLegacySize "" "%PROJECT_DIR_STAGE%\BOOT"

if %LEGACY_IMAGE_SIZE% == 0 (
  %TOOL_ALIGN% "Legacy image:" No legacy boot executables found
  goto buildEFIImage
)

set /a BOOT_IMAGE_SECTORS=(%BOOT_IMAGE_SIZE%/512)
set MKISO_LEGACY=-eltorito-boot "%LEGACY_IMAGE_PATH%" -boot-load-size %BOOT_IMAGE_SECTORS% -boot-load-seg %BOOT_IMAGE_ADDRESS% -boot-info-table -no-emul-boot -hide "%LEGACY_IMAGE_PATH%"

set /a LEGACY_IMAGE_SECTORS=(%LEGACY_IMAGE_SIZE%/512)+100
set /a LEGACY_IMAGE_SIZE=%LEGACY_IMAGE_SECTORS%*512
%TOOL_ALIGN% "Legacy image:" Image has size %LEGACY_IMAGE_SIZE% B (%LEGACY_IMAGE_SECTORS% sectors)

set MFORMAT_ARGUMENTS=-C -c 1 -d 1 -r 1 -h 1 -s 1 -T %LEGACY_IMAGE_SECTORS% -v "%PROJECT_SAFE_NAME%" -i "%LEGACY_IMAGE_INT%" ::

%TOOL_ALIGN% "Legacy image:" Generating !LEGACY_IMAGE:%WORKSPACE%\=!

%TOOL_MFORMAT% %MFORMAT_ARGUMENTS% 2>&1
if not !errorlevel! == 0 (
  echo.
  exit /b 1
)
if not exist "%LEGACY_IMAGE_INT%" (
  echo.
  exit /b 1
)

for %%i in (%BUILD_ARCH%) do (
  call:addArchToLegacyImage %%i
  if not !errorlevel! == 0 exit /b 1
)
call:addArchToLegacyImage

copy /y /b "%BOOT_IMAGE%"+"%LEGACY_IMAGE_INT%" "%LEGACY_IMAGE%" 1>NUL 2>&1
if not !errorlevel! == 0 (
  echo.
  exit /b 1
)
if not exist "%LEGACY_IMAGE%" (
  echo.
  exit /b 1
)

rem ===========================================================================
rem Build EFI boot image
rem ===========================================================================

:buildEFIImage

echo.
%TOOL_ALIGN% "EFI image:" Determining size of image

for %%i in (%BUILD_ARCH%) do (
  call:addArchSize %%i "%PROJECT_DIR_STAGE%\EFI\BOOT\BOOT%%i.EFI"
  if not !errorlevel! == 0 exit /b 1
)

if %EFI_IMAGE_SIZE% == 0 (
  %TOOL_ALIGN% "EFI image:" No EFI boot executables found
  goto buildISO
)

set MKISO_EFI=-eltorito-alt-boot -eltorito-platform efi -eltorito-boot "%EFI_IMAGE_PATH%" -no-emul-boot -hide "%EFI_IMAGE_PATH%"

set /a EFI_IMAGE_SECTORS=(%EFI_IMAGE_SIZE%/512)+100
set /a EFI_IMAGE_SIZE=%EFI_IMAGE_SECTORS%*512
%TOOL_ALIGN% "EFI image:" Image has size %EFI_IMAGE_SIZE% B (%EFI_IMAGE_SECTORS% sectors)

set MFORMAT_ARGUMENTS=-C -h 1 -s 1 -T %EFI_IMAGE_SECTORS% -v "%PROJECT_SAFE_NAME%" -i "%EFI_IMAGE%" ::

%TOOL_ALIGN% "EFI image:" Generating !EFI_IMAGE:%WORKSPACE%\=!

%TOOL_MFORMAT% %MFORMAT_ARGUMENTS% 2>&1
if not exist "%EFI_IMAGE%" (
  echo.
  exit /b 1
)

%TOOL_ALIGN% "EFI image:" Creating directory structure

%TOOL_MMD% -i "%EFI_IMAGE%" ::EFI ::EFI/BOOT ::EFI/%PROJECT_SAFE_NAME% 2>&1
if not !errorlevel! == 0 (
  echo.
  exit /b 1
)

%TOOL_ALIGN% "EFI image:" Adding EFI/%PROJECT_SAFE_NAME%/%PROJECT_SAFE_NAME%.XML

%TOOL_MCOPY% -i "%EFI_IMAGE%" "%PROJECT_DIR_STAGE%\EFI\%PROJECT_SAFE_NAME%\%PROJECT_SAFE_NAME%.XML" ::EFI/%PROJECT_SAFE_NAME%/%PROJECT_SAFE_NAME%.XML 2>&1
if not !errorlevel! == 0 (
  echo.
  exit /b 1
)

for %%i in (%BUILD_ARCH%) do (
  call:addArchToImage %%i
  if not !errorlevel! == 0 exit /b 1
)

rem ===========================================================================
rem Build ISO
rem ===========================================================================

:buildISO

if not defined PROJECT_ISO goto:eof

echo.
%TOOL_ALIGN% "Building ISO:" !PROJECT_ISO:%WORKSPACE%\=!

%TOOL_MKISO% %MKISO_ARGUMENTS% %MKISO_LEGACY% %MKISO_EFI% %MKISO_HIDDEN% -o "%PROJECT_ISO%" "%PROJECT_DIR_STAGE%" 2>&1
if not !errorlevel! == 0 (
  echo.
  exit /b 1
)

goto:eof

rem ===========================================================================
rem Add architecture boot executable to image
rem ===========================================================================

:addArchToImage

  if not exist "%PROJECT_DIR_STAGE%\EFI\BOOT\BOOT%~1.efi" goto:eof

  %TOOL_ALIGN% "EFI image:" Adding EFI/BOOT/BOOT%~1.efi

  %TOOL_MCOPY% -i "%EFI_IMAGE%" "%PROJECT_DIR_STAGE%\EFI\BOOT\BOOT%~1.efi" ::EFI/BOOT/BOOT%~1.efi 2>&1
  if not !errorlevel! == 0 (
    echo.
    exit /b 1
  )

  goto:eof

rem ===========================================================================
rem Get architecture boot executable size
rem ===========================================================================

:addArchSize

  if not exist "%~2" goto:eof

  %TOOL_ALIGN% "EFI image:" EFI/BOOT/BOOT%~1.efi has size %~z2 B

  set /a EFI_IMAGE_SIZE+=%~z2
  if not !errorlevel! == 0 (
    echo.
    exit /b 1
  )

  goto:eof

rem ===========================================================================
rem Add architecture boot executable to legacy image
rem ===========================================================================

:addArchToLegacyImage

  if not exist "%PROJECT_DIR_STAGE%\BOOT%~1" goto:eof

  %TOOL_ALIGN% "Legacy image:" Adding BOOT%~1

  %TOOL_MCOPY% -i "%LEGACY_IMAGE_INT%" "%PROJECT_DIR_STAGE%\BOOT%~1" ::BOOT%~1 2>&1
  if not !errorlevel! == 0 (
    echo.
    exit /b 1
  )

  set MKISO_HIDDEN=%MKISO_HIDDEN% -hide "BOOT%~1"

  goto:eof

rem ===========================================================================
rem Get architecture legacy boot executable size
rem ===========================================================================

:addArchLegacySize

  if not exist "%~2" goto:eof

  %TOOL_ALIGN% "Legacy image:" BOOT%~1 has size %~z2 B

  set /a LEGACY_IMAGE_SIZE+=%~z2
  if not !errorlevel! == 0 (
    echo.
    exit /b 1
  )

  goto:eof

rem ===========================================================================
rem Get boot loader executable size
rem ===========================================================================

:getBootSize

  if not exist "%~1" goto:eof

  set BOOT_IMAGE_SIZE=%~z1

  if not "%~2" == "" goto:eof

  %TOOL_ALIGN% "Boot image:" %~nx1 has size %~z1 B

  goto:eof

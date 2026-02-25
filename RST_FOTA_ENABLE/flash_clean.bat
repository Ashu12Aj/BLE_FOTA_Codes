@echo off
setlocal

:: --- Configuration ---
set "NCS_TAG=v3.2.1"
set "TOOLCHAIN_ID=66cdf9b75e"
set "NCS_ROOT=c:\ncs"
set "NCS_TOOLCHAIN_PATH=%NCS_ROOT%\toolchains\%TOOLCHAIN_ID%"

:: --- Environment Setup ---
echo Setting up environment for NCS %NCS_TAG%...

set "ZEPHYR_BASE=%NCS_ROOT%\%NCS_TAG%\zephyr"
set "ZEPHYR_TOOLCHAIN_VARIANT=zephyr"
set "ZEPHYR_SDK_INSTALL_DIR=%NCS_TOOLCHAIN_PATH%\opt\zephyr-sdk"
set "PYTHONPATH=%NCS_TOOLCHAIN_PATH%\opt\bin;%NCS_TOOLCHAIN_PATH%\opt\bin\Lib;%NCS_TOOLCHAIN_PATH%\opt\bin\Lib\site-packages"

:: Prepend Toolchain Paths
set "PATH=%NCS_TOOLCHAIN_PATH%;%NCS_TOOLCHAIN_PATH%\mingw64\bin;%NCS_TOOLCHAIN_PATH%\bin;%NCS_TOOLCHAIN_PATH%\opt\bin;%NCS_TOOLCHAIN_PATH%\opt\bin\Scripts;%NCS_TOOLCHAIN_PATH%\opt\nanopb\generator-bin;%NCS_TOOLCHAIN_PATH%\nrfutil\bin;%NCS_TOOLCHAIN_PATH%\opt\zephyr-sdk\arm-zephyr-eabi\bin;%PATH%"

:: --- Build and Flash ---
echo Starting Build and Flash Process...

set BOARD=nrf52dk/nrf52832

echo.
echo *** Building for %BOARD% ***
:: Capture output to log file
call west build -b %BOARD% --pristine > build_log.txt 2>&1
if %errorlevel% neq 0 (
    echo Build failed! See build_log.txt for details.
    type build_log.txt
    exit /b %errorlevel%
)

echo.
echo *** Flashing and Erasing Chip ***
:: --erase clears the entire flash before programming
call west flash --erase
if %errorlevel% neq 0 (
    echo Flash failed!
    exit /b %errorlevel%
)

echo.
echo *** Resetting Device (Clears RAM) ***
nrfjprog --reset

echo.
echo Done! FOTA should be enabled and device reset.
endlocal
pause

@echo off
setlocal

:: Simple helper to compile and upload the ESPHome config for Windows.
:: Usage:
::   esphome-build-upload.bat [config.yaml]
:: Default config is cyd_ha_refactored.yaml if not provided.

:: Change to the directory of this script
cd /d "%~dp0"

:: Activate local ESPHome virtualenv if present
if exist "%USERPROFILE%\esphome-venv\Scripts\activate.bat" (
    call "%USERPROFILE%\esphome-venv\Scripts\activate.bat"
) else if exist "venv\Scripts\activate.bat" (
    call "venv\Scripts\activate.bat"
)

:: Check if esphome is installed
where esphome >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: 'esphome' CLI not found. Install with: pip install esphome
    pause
    exit /b 127
)

set "CONFIG_FILE=%~1"
if "%CONFIG_FILE%"=="" set "CONFIG_FILE=cyd_ha.yaml"

if not exist "%CONFIG_FILE%" (
    echo Error: Config file not found: %CONFIG_FILE%
    pause
    exit /b 1
)

:: Auto-download Material Design Icons font if not present
if not exist "materialdesignicons-webfont.ttf" (
    echo Downloading Material Design Icons font...
    curl.exe -L -s -o materialdesignicons-webfont.ttf "https://github.com/Templarian/MaterialDesign-Webfont/raw/master/fonts/materialdesignicons-webfont.ttf"
    if exist "materialdesignicons-webfont.ttf" (
        echo Font downloaded successfully.
    ) else (
        echo Warning: Failed to auto-download font. Please download manually.
    )
)

echo Validating %CONFIG_FILE%...
call esphome config "%CONFIG_FILE%"
if %ERRORLEVEL% neq 0 (
    echo Failed to validate config.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Compiling %CONFIG_FILE%...
call esphome compile "%CONFIG_FILE%"
if %ERRORLEVEL% neq 0 (
    echo Failed to compile.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Uploading %CONFIG_FILE%...
call esphome upload "%CONFIG_FILE%"
if %ERRORLEVEL% neq 0 (
    echo Failed to upload.
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Done.
pause

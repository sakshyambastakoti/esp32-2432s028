@echo off
title ESP32-2432S028 ID Badge Flasher (PlatformIO)
echo ========================================================
echo  ESP32-2432S028 Digital ID Badge - Auto Flasher
echo ========================================================
echo.
echo Make sure your ESP32 CYD board is connected via USB!
echo.
pause
echo.
echo Flashing firmware to ESP32...
"C:\Users\ACER\.platformio\penv\Scripts\pio.exe" run -t upload -d "%~dp0"
if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Flash failed!
    echo If it failed connecting, press and hold the BOOT button on the ESP32 and try again.
) else (
    echo.
    echo [SUCCESS] Firmware uploaded successfully!
)
echo.
pause

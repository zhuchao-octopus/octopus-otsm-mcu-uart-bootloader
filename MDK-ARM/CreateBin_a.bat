@echo off
setlocal enabledelayedexpansion

REM ================== 获取当前时间戳（yyyyMMddHHmmss），兼容 Win7~Win11 ==================
set hh=%time:~0,2%
if "%hh:~0,1%"==" " set hh=0%hh:~1,1%
set mm=%time:~3,2%
set ss=%time:~6,2%
set dt=%date:~0,4%%date:~5,2%%date:~8,2%
set timestamp=%dt%%hh%%mm%%ss%

REM ================== 文件扩展名 ==================
set hex_ext=.hex
set bin_ext=.bin
set slot=_a

REM ================== 源文件路径 ==================
set HEX_FILE=.\Objects\HK32L0xx%slot%.hex
set AXF_FILE=.\Objects\HK32L0xx%slot%.axf

REM ================== 检查源文件 ==================
if not exist "%HEX_FILE%" (
    echo ❌ ERROR: HEX file not found - %HEX_FILE%
    exit /b 1
)
if not exist "%AXF_FILE%" (
    echo ❌ ERROR: AXF file not found - %AXF_FILE%
    exit /b 1
)

REM ================== 查找第一个可用编号 ==================
set index=1
:check_exists
set "padded=00!index!"
set "padded=!padded:~-3!"
set "OUT_BASE=.\BINS\MCU_!timestamp!_!padded!"

set "HEX_OUT=!OUT_BASE!!slot!!hex_ext!"
set "BIN_OUT=!OUT_BASE!!slot!!bin_ext!"

if exist "!HEX_OUT!" (
    set /a index+=1
    goto :check_exists
)

REM ================== 生成 HEX 文件副本 ==================
copy /Y "%HEX_FILE%" "!HEX_OUT!" >nul
if errorlevel 1 (
    echo ❌ ERROR: Failed to copy HEX file.
    exit /b 1
)
echo ✅ HEX created → !HEX_OUT!

REM ================== 转换 AXF 为 BIN ==================
"C:\Keil_v5\ARM\ARMCC\bin\fromelf" --bin --output "!BIN_OUT!" "%AXF_FILE%"
if errorlevel 1 (
    echo ❌ ERROR: fromelf failed.
    exit /b 1
)
echo ✅ BIN created → !BIN_OUT!

echo 🎉 All tasks completed successfully.
echo Output files located at:
echo    !HEX_OUT!
echo    !BIN_OUT!

exit /b 0

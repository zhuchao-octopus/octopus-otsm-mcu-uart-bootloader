@echo off
setlocal enabledelayedexpansion

REM ================== 获取当前时间戳（yyyyMMddHHmmss），兼容 Win7~Win11 ==================
set "yyyy=%date:~0,4%"
set "MM=%date:~5,2%"
set "dd=%date:~8,2%"
set "HH=%time:~0,2%"
set "mm=%time:~3,2%"
set "ss=%time:~6,2%"

REM 修正小时可能有空格的情况（例如  9:12:30.45）
if "%HH:~0,1%"==" " set "HH=0%HH:~1,1%"

set "timestamp=%yyyy%%MM%%dd%%HH%%mm%%ss%"

REM ================== 文件扩展名与槽位 ==================
set "hex_ext=.hex"
set "bin_ext=.bin"
set "slot=_l"

REM ================== 源文件路径 ==================
set "HEX_FILE=.\Objects\HK32L0xx%slot%.hex"
set "AXF_FILE=.\Objects\HK32L0xx%slot%.axf"

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
set "OUT_BASE=MCU_%timestamp%_!padded!"

if exist "!OUT_BASE!%slot%%hex_ext%" (
    set /a index+=1
    goto :check_exists
)

REM ================== 生成 HEX 文件副本 ==================
copy /Y "%HEX_FILE%" "!OUT_BASE!%slot%%hex_ext%" >nul
if errorlevel 1 (
    echo ❌ ERROR: Failed to copy HEX file to "!OUT_BASE!%slot%%hex_ext%"
    exit /b 1
)
echo ✅ HEX created → "!OUT_BASE!%slot%%hex_ext%"

REM ================== 转换 AXF 为 BIN ==================
"C:\Keil_v5\ARM\ARMCC\bin\fromelf" --bin --output "!OUT_BASE!%slot%%bin_ext%" "%AXF_FILE%"
if errorlevel 1 (
    echo ❌ ERROR: fromelf failed for "!AXF_FILE!"
    exit /b 1
)
echo ✅ BIN created → "!OUT_BASE!%slot%%bin_ext%"

echo 🎉 All tasks completed successfully.
echo Output files located at: "!OUT_BASE!%slot%%hex_ext%" and "!OUT_BASE!%slot%%bin_ext%"
exit /b 0

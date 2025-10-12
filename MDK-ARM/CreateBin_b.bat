@echo off
setlocal enabledelayedexpansion

REM ==== 获取当前时间戳（yyyymmddhhmmss） ====
for /f %%i in ('wmic os get localdatetime ^| find "."') do set datetime=%%i
set ymd=!datetime:~0,8!
set hms=!datetime:~8,6!
set timestamp=!ymd!!hms!

REM ==== 文件扩展名 ====
set ext=.oupg
set slot=_b

REM ==== 源文件路径 ====
set HEX_FILE=.\Objects\HK32L0xx!slot!.hex
set AXF_FILE=.\Objects\HK32L0xx!slot!.axf

REM ==== 检查源文件 ====
if not exist "!HEX_FILE!" (
    echo ERROR: HEX file not found - !HEX_FILE!
    exit /b 1
)
if not exist "!AXF_FILE!" (
    echo ERROR: AXF file not found - !AXF_FILE!
    exit /b 1
)

REM ==== 查找第一个可用编号 ====
set index=1
:check_exists
set padded=00!index!
set padded=!padded:~-3!
set OUT_BASE=MCU_!timestamp!_!padded!

if exist "!OUT_BASE!_hex!ext!" (
    set /a index+=1
    goto :check_exists
)

REM ==== 生成 HEX 复制 ====
copy /Y "!HEX_FILE!" "!OUT_BASE!_hex!slot!!ext!" >nul
if errorlevel 1 (
    echo ERROR: Failed to copy HEX file.
    exit /b 1
)
echo ✅ HEX created: !OUT_BASE!_hex!slot!!ext!

REM ==== 生成 AXF 转换 ====
C:\Keil_v5\ARM\ARMCC\bin\fromelf --bin --output "!OUT_BASE!_bin!slot!!ext!" "!AXF_FILE!"
if errorlevel 1 (
    echo ERROR: fromelf failed.
    exit /b 1
)
echo ✅ BIN created: !OUT_BASE!_bin!slot!!ext!

exit /b 0

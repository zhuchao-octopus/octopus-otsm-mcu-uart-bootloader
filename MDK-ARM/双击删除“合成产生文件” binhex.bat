@echo off

REM 当前目录及子目录的对应后缀名的文件
for /f "delims=" %%i in ('dir /b /a-d /s "*.bin"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.hex"') do del /a/f/q "%%i"

exit
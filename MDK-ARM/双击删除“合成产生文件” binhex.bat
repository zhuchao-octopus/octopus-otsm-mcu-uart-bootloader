@echo off

REM ��ǰĿ¼����Ŀ¼�Ķ�Ӧ��׺�����ļ�
for /f "delims=" %%i in ('dir /b /a-d /s "*.bin"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.hex"') do del /a/f/q "%%i"

exit
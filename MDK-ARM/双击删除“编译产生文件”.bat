@echo off
REM ɾ����ǰĿ¼���ļ��м���Ŀ¼���ļ���
for /f "delims=" %%a in ('dir /b/s/ad') do (
    REM ��������ɾ��KEIL�������ļ���
    if "%%~na"=="DebugConfig" rd /s /q "%%a" 2>nul
    if "%%~na"=="Listings" rd /s /q "%%a" 2>nul
    if "%%~na"=="Objects" rd /s /q "%%a" 2>nul
    
    REM ��������ɾ��IAR�������ļ���
    if "%%~na"=="Debug" rd /s /q "%%a" 2>nul
    if "%%~na"=="settings" rd /s /q "%%a" 2>nul
)

REM ��ǰĿ¼����Ŀ¼�Ķ�Ӧ��׺�����ļ�
for /f "delims=" %%i in ('dir /b /a-d /s "*.bak"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.plg"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.dep"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.scvd"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.ini"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.uvguix.*"') do del /a/f/q "%%i"
for /f "delims=" %%i in ('dir /b /a-d /s "*.txt"') do del /a/f/q "%%i"

���IAR�๤��ɾ���ഴ�����ļ���
for /D %%i in ("../EWARM/*") do (
	rd /s /q "%%i"
)

exit
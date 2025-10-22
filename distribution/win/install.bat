@echo off
setlocal enabledelayedexpansion
title CopyXPaste Installer

REM ---------------------------------------------------------------------------
REM 1. Detect the binary (prefer copyxpaste.exe)
REM ---------------------------------------------------------------------------
set "BIN=copyxpaste.exe"
if not exist "%~dp0%BIN%" (
  for %%F in ("%~dp0*.exe") do (
    if /I not "%%~nxF"=="install.exe" (
      set "BIN=%%~nxF"
      goto foundexe
    )
  )
  echo [!] No .exe found in this folder.
  pause
  exit /b 1
)
:foundexe
echo [*] Found binary: %BIN%

REM ---------------------------------------------------------------------------
REM 2. Define install path
REM ---------------------------------------------------------------------------
set "TARGETDIR=%LOCALAPPDATA%\bin"
set "TARGET=%TARGETDIR%\%BIN%"
if not exist "%TARGETDIR%" (
  echo [*] Creating %TARGETDIR%
  mkdir "%TARGETDIR%" >nul 2>&1
)

REM ---------------------------------------------------------------------------
REM 3. Copy binary (overwrite existing)
REM ---------------------------------------------------------------------------
echo [*] Installing to %TARGET%
copy /Y "%~dp0%BIN%" "%TARGET%" >nul
if errorlevel 1 (
  echo [!] Copy failed.
  pause
  exit /b 1
)
echo [✓] Installed: %TARGET%

REM ---------------------------------------------------------------------------
REM 4. Add to user PATH (persistent)
REM ---------------------------------------------------------------------------
set "ADDPATH=0"
for /f "usebackq tokens=1* delims==" %%A in (`powershell -NoProfile -Command ^
  "[Environment]::GetEnvironmentVariable('Path','User')"`) do (
    set "USERPATH=%%B"
)
echo %USERPATH% | find /I "%TARGETDIR%" >nul || set "ADDPATH=1"

if "%ADDPATH%"=="1" (
  echo [*] Adding %TARGETDIR% to your User PATH...
  powershell -NoProfile -Command ^
    "$p=[Environment]::GetEnvironmentVariable('Path','User');" ^
    "if(-not $p){$p='%TARGETDIR%'}elseif(-not($p -match [regex]::Escape('%TARGETDIR%'))){$p+=';%TARGETDIR%'};" ^
    "[Environment]::SetEnvironmentVariable('Path',$p,'User')"
  echo [✓] Added to PATH.
) else (
  echo [*] %TARGETDIR% already in PATH.
)

REM ---------------------------------------------------------------------------
REM 5. Update current session PATH
REM ---------------------------------------------------------------------------
setx PATH "%TARGETDIR%;%PATH%" >nul

REM ---------------------------------------------------------------------------
REM 6. Verify command availability
REM ---------------------------------------------------------------------------
where "%BIN%" >nul 2>&1
if %errorlevel%==0 (
  echo [✓] Command available now: %BIN%
) else (
  echo [!] It will be available after you open a new terminal window.
)

echo.
echo Done! You can now run "%BIN%" from any command prompt or PowerShell.
echo Press any key to close...
pause >nul
endlocal

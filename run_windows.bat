@echo off
cd /d "%~dp0"
where g++ >nul 2>nul
if errorlevel 1 (
  echo ERROR: C++ compiler g++ was not found.
  echo Install MSYS2 from https://www.msys2.org/ and install the MinGW-w64 GCC toolchain.
  echo Then reopen VS Code and make sure the MinGW bin folder is on PATH.
  pause
  exit /b 1
)
echo Compiling Reason-Act-Reflect...
g++ -std=c++11 -O2 -Wall -Wextra main.cpp -o reason_act_reflect.exe
if errorlevel 1 (
  echo Compilation failed. Read the compiler errors above.
  pause
  exit /b 1
)
echo.
echo Starting Reason-Act-Reflect...
reason_act_reflect.exe
pause

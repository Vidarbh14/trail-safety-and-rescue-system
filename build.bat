@echo off
echo =====================================================================
echo  Compiling Mountain Trail Safety and Rescue Management System...
echo =====================================================================
g++ -std=c++14 -Wall -Wextra -Iinclude src/*.cpp main.cpp -o main.exe
if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Compilation complete. Run .\main.exe to start the system.
) else (
    echo [ERROR] Compilation failed.
)

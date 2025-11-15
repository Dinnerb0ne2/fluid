@echo off
setlocal

:: Path to executable
set EXECUTABLE=bin\blw_fluid.exe

:: Check if executable exists
if not exist %EXECUTABLE% (
    echo Error: Executable not found! Run build.cmd first.
    exit /b 1
)

:: Run the simulation
echo Starting BLW Fluid Simulation...
%EXECUTABLE%

:: Check execution status
if %errorlevel% equ 0 (
    echo Simulation exited successfully.
) else (
    echo Simulation crashed with error code %errorlevel%.
    exit /b 1
)

endlocal
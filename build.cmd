@echo off
setlocal enabledelayedexpansion

:: Project Configuration
set PROJECT_NAME=blw_fluid
set SRC_DIR=src
set INCLUDE_DIR=include
set LIB_DIR=lib
set OUTPUT_DIR=bin
set OBJ_DIR=obj

:: Compiler configuration
set CXX=g++
set CXXFLAGS=-std=c++23 -Wall -Wextra -pedantic -Iinclude -O2
set LDFLAGS=-L%LIB_DIR% -lglfw3 -lopengl32 -lgdi32 -luser32 -lkernel32
set EXECUTABLE=%OUTPUT_DIR%\%PROJECT_NAME%.exe

:: Create output directories
if not exist "%OUTPUT_DIR%" mkdir "%OUTPUT_DIR%"
if not exist "%OBJ_DIR%" mkdir "%OBJ_DIR%"

echo [INFO] Building %PROJECT_NAME% with MinGW
echo.

:: Verify compiler availability
echo [INFO] Checking compiler availability...
%CXX% --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Compiler not found in PATH
    echo [ERROR] Please ensure MinGW bin directory is in your system PATH
    exit /b 1
)

:: Display compiler and environment info
%CXX% --version
echo [INFO] Include Path: %INCLUDE_DIR%
echo [INFO] Library Path: %LIB_DIR%

echo [INFO] Step 1: Building object files...
set FILE_COUNT=0
set BUILD_SUCCESS=1

:: Recursively find and compile all .cpp files
for /r "%SRC_DIR%" %%i in (*.cpp) do (
    set /a FILE_COUNT+=1
    set OBJ_FILE="%OBJ_DIR%\%%~ni.o"
    if not exist !OBJ_FILE! (
        echo [CC] [!FILE_COUNT!] Compiling %%~nxi...
        "%CXX%" %CXXFLAGS% -c "%%i" -o !OBJ_FILE!
        if errorlevel 1 (
            echo [ERROR] Failed to compile %%~nxi
            set BUILD_SUCCESS=0
            goto :BUILD_FAILED
        )
    ) else (
        echo [INFO] Skipping already compiled file: %%~nxi
    )
)

if %FILE_COUNT%==0 (
    echo [ERROR] No source files found in %SRC_DIR%
    exit /b 1
)

echo [INFO] Successfully compiled %FILE_COUNT% source files.

:: Link executable
echo [INFO] Step 2: Linking executable...
"%CXX%" "%OBJ_DIR%\*.o" -o "%EXECUTABLE%" %LDFLAGS%

if errorlevel 1 (
    echo [ERROR] Linking failed!
    exit /b 1
)

:: Calculate file size
for %%F in ("%EXECUTABLE%") do set FILE_SIZE=%%~zF
set /a FILE_SIZE_KB=%FILE_SIZE%/1024

:: Build successful
echo [INFO] BUILD SUCCESSFUL!
echo [INFO] Executable: %EXECUTABLE%
echo [INFO] Size: %FILE_SIZE_KB% KB

:: Run the application if desired
set /p RUN_APP="Run application now? (y/n): "
if /i "!RUN_APP!"=="y" (
    echo [INFO] Starting %PROJECT_NAME%...
    "%EXECUTABLE%"
    echo [INFO] Program exited with errorlevel: !errorlevel!
)

goto :CLEANUP

:BUILD_FAILED
echo [ERROR] BUILD FAILED!
exit /b 1

:CLEANUP
:: Cleanup options
set /p CLEANUP_OPTION="Cleanup options: (1) Keep objects (2) Clean objects (3) Clean all but executable: "
if /i "!CLEANUP_OPTION!"=="2" (
    if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
    echo Object files cleaned.
) else if /i "!CLEANUP_OPTION!"=="3" (
    if exist "%OBJ_DIR%" rmdir /s /q "%OBJ_DIR%"
    echo Build artifacts cleaned.
)

echo [INFO] Build process completed.
endlocal
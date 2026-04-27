@echo off
set "QT_DIR=C:\Qt\6.11.0\mingw_64"
set "MINGW_DIR=C:\Qt\Tools\mingw1310_64"
set "CMAKE_DIR=C:\Qt\Tools\CMake_64"
set "NINJA_DIR=C:\Qt\Tools\Ninja"

set "PATH=%MINGW_DIR%\bin;%QT_DIR%\bin;%CMAKE_DIR%\bin;%NINJA_DIR%;%PATH%"

if not exist build mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_PREFIX_PATH="%QT_DIR%"
ninja
if %ERRORLEVEL% EQU 0 (
    echo Build successful!
    echo Running application...
    studentManagementSystem.exe
)
cd ..

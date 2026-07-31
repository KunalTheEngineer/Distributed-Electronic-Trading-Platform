@echo off

echo ===============================
echo Building Trading Platform...
echo ===============================

if not exist F:\TradingGUI\build (
    mkdir F:\TradingGUI\build
)

cd /d F:\TradingGUI\build

cmake ^
-DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64" ^
-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake ^
..

cmake --build .

pause
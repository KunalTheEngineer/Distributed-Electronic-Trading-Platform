@echo off
echo ===============================
echo Building Trading GUI...
echo ===============================

cd /d F:\TradingGUI\build

cmake -DCMAKE_PREFIX_PATH="C:/Qt/6.11.0/msvc2022_64" ..
cmake --build .

echo ===============================
echo Running Application...
echo ===============================

cd Debug
app.exe

pause
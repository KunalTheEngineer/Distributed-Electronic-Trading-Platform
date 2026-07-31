@echo off

echo ===============================
echo Starting Trading Server...
echo ===============================

set PATH=C:\Qt\6.11.0\msvc2022_64\bin;%PATH%

cd /d F:\TradingGUI\build\Debug

TradingServer.exe

pause
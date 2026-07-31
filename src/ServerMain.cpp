#include <iostream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "network/TcpServer.h"

int main()
{
    AllocConsole();

    FILE* fp;

    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    std::cout << "=====================================\n";
    std::cout << " Trading Server Started\n";
    std::cout << "=====================================\n";

    TcpServer server;

    server.start();

    return 0;
}
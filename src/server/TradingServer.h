#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>


#include "MatchingEngine.h"

class TradingServer
{
public:
    TradingServer();

    void start();

    void stop();

    void processOrder(const Order& order);

    bool isRunning() const;

    MatchEngine& getMatchEngine();

private:
    bool running;

    MatchEngine engine;
};
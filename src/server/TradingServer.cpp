#include "TradingServer.h"

#include <iostream>

TradingServer::TradingServer()
    : running(false)
{
}

void TradingServer::start()
{
    running = true;
    std::cout << "Trading Server Started..." << std::endl;
}

void TradingServer::stop()
{
    running = false;
    std::cout << "Trading Server Stopped..." << std::endl;
}

void TradingServer::processOrder(const Order& order)
{
    engine.processOrder(order);
}

bool TradingServer::isRunning() const
{
    return running;
}

MatchEngine& TradingServer::getMatchEngine()
{
    return engine;
}
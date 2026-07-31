#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <atomic>
#include <numeric>

#include "../shared/Trade.h"
#include "OrderBook.h"


struct PositionInfo
{
    int quantity = 0;

    double averagePrice = 0.0;

    double realizedPnL = 0.0;

    double unrealizedPnL = 0.0;
};

class MatchEngine
{

    private:

            std::mutex engineMutex;

            int nextOrderId = 1;

            long long totalLatency = 0;

            std::vector<long long> latencySamples;

            std::atomic<long long> totalOrdersProcessed{0};

            std::chrono::high_resolution_clock::time_point benchmarkStart;

    public:
    
    OrderBook book;
    std::vector<Trade> trades;
    std::unordered_map<int, PositionInfo> clientPositions;

    std::atomic<long long> latestLatency{0};

    int currentPosition = 0;

    void processOrder(Order order);

    void cancelOrder(int orderId);

    void matchOrders();

    int getTotalTrades() const;

    int getTotalVolume() const;

    double getVWAP() const;

    double getAveragePrice() const;

    std::string getOrderBookSnapshot(int clientId);

    std::string getMarketDepthSnapshot() const;

    double getBestBid() const;
    
    double getBestAsk() const;

    double getSpread() const;

    int getCurrentPosition() const
    {
        return currentPosition;
    }

    int getCurrentPosition(int clientId = 1) const
    {
        auto it = clientPositions.find(clientId);

        if (it == clientPositions.end())
            return 0;

        return it->second.quantity;
    }

    double getAveragePrice(int clientId = 1) const
    {
        auto it = clientPositions.find(clientId);

        if (it == clientPositions.end())
            return 0.0;

        return it->second.averagePrice;
    }
 
    long long getLatestLatency() const
    {
        return latestLatency.load();
    }

    double getAverageLatency() const;

    long long getP95Latency();

    long long getTotalOrdersProcessed() const;
};
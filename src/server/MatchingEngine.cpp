#include "MatchingEngine.h"
#include <map>
#include <algorithm>
#include <chrono>

void MatchEngine::processOrder(Order order)
{

    totalOrdersProcessed++;

    if (getTotalOrdersProcessed() == 1)
    {
        benchmarkStart = std::chrono::high_resolution_clock::now();
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::lock_guard<std::mutex> lock(engineMutex);

    order.id = nextOrderId++;

    book.addOrder(order);

    matchOrders();

    auto end = std::chrono::high_resolution_clock::now();

    auto latency =
        std::chrono::duration_cast<std::chrono::microseconds>(
            end - start).count();

    latestLatency.store(latency);

    totalLatency += latency;

    latencySamples.push_back(latency);

    if (getTotalOrdersProcessed() == 122000)
    {

        auto benchmarkEnd = std::chrono::high_resolution_clock::now();

        double totalSeconds =
            std::chrono::duration<double>(
                benchmarkEnd - benchmarkStart).count();

        double ordersPerSecond = getTotalOrdersProcessed() / totalSeconds;

        double tradesPerSecond = getTotalTrades() / totalSeconds;

        std::cout << "\n=====================================\n";
        std::cout << "        PERFORMANCE REPORT\n";
        std::cout << "=====================================\n";
        std::cout << "Orders Processed : " << getTotalOrdersProcessed() << '\n';
        std::cout << "Total Trades     : " << getTotalTrades() << '\n';
        std::cout << "Total Volume     : " << getTotalVolume() << '\n';
        std::cout << "Runtime          : " << totalSeconds << " sec\n";
        std::cout << "Orders / Second  : " << static_cast<long long>(ordersPerSecond) << std::endl;
        std::cout << "Trades / Second  : " << static_cast<long long>(tradesPerSecond) << std::endl;
        std::cout << "Latest Latency   : " << getLatestLatency() << " us\n";
        std::cout << "Average Latency  : " << getAverageLatency() << " us\n";
        std::cout << "P95 Latency      : " << getP95Latency() << " us\n";
        std::cout << "=====================================\n";
    }
}

void MatchEngine::cancelOrder(int orderId)
{
    std::lock_guard<std::mutex> lock(engineMutex);

    if (book.cancelOrder(orderId))
    {
        std::cout << "Order " << orderId
                  << " cancelled successfully."
                  << std::endl;
    }
    else
    {
        std::cout << "Order " << orderId
                  << " not found."
                  << std::endl;
    }
}

void MatchEngine::matchOrders()
{
    while( !book.buyOrders.empty() && !book.sellOrders.empty() ) 
    { 
        Order topBuy = book.buyOrders.top(); 
        
        Order topSell = book.sellOrders.top(); 
  
        // MATCH CONDITION 
       
        if(topBuy.price >= topSell.price) 
        {   
            // Find matched quantity 
            int tradedQty = std::min(topBuy.quantity, topSell.quantity); 

            PositionInfo& buyerPosition = clientPositions[topBuy.clientId];
            PositionInfo& sellerPosition = clientPositions[topSell.clientId];
            
             double totalCost = (buyerPosition.averagePrice * buyerPosition.quantity) +
                                (topSell.price * tradedQty);

            buyerPosition.quantity += tradedQty;

            buyerPosition.averagePrice =
                totalCost / buyerPosition.quantity;


            if (sellerPosition.quantity > 0)
            {
                sellerPosition.realizedPnL +=
                    (topSell.price - sellerPosition.averagePrice) * tradedQty;
            }

            sellerPosition.quantity -= tradedQty;

            if (sellerPosition.quantity == 0)
            {
                sellerPosition.averagePrice = 0.0;
            }
            
            trades.push_back(Trade(topSell.price, tradedQty)); 
             
            // Remove old orders 
            book.buyOrders.pop();
            book.sellOrders.pop(); 
         
            // Remaining quantity 
            topBuy.quantity -= tradedQty; 
            topSell.quantity -= tradedQty; 
            
            // Reinsert if quantity remains 
            if(topBuy.quantity > 0) 
            { 
               book.buyOrders.push(topBuy);
            } 
            
            if(topSell.quantity > 0) 
            { 
               book.sellOrders.push(topSell); 
            } 
        } 
        else 
        { 
            break; 
        } 
    }

}

std::string MatchEngine::getOrderBookSnapshot(int clientId)
{
    std::lock_guard<std::mutex> lock(engineMutex);

    std::string snapshot;

    snapshot += "BUY\n";

    auto buyQueue = book.buyOrders;

    while (!buyQueue.empty())
    {
        Order order = buyQueue.top();
        buyQueue.pop();

        snapshot +=
            std::to_string(order.id) + "," +
            std::to_string(order.price) + "," +
            std::to_string(order.quantity) + "\n";
    }

    snapshot += "END_BUY\n";

    snapshot += "SELL\n";

    auto sellQueue = book.sellOrders;

    while (!sellQueue.empty())
    {
        Order order = sellQueue.top();
        sellQueue.pop();

        snapshot +=
            std::to_string(order.id) + "," +
            std::to_string(order.price) + "," +
            std::to_string(order.quantity) + "\n";
    }

    snapshot += "END_SELL\n";

    /*snapshot += "TRADES\n";

    for (const auto& trade : trades)
    {
        snapshot +=
            std::to_string(trade.price) + "," +
            std::to_string(trade.quantity) + "\n";
    }

    snapshot += "END_TRADES\n";*/

    snapshot += "TRADES\n";

    // Send only the latest 100 trades
    int startIndex = std::max(0, static_cast<int>(trades.size()) - 100);

    for (int i = startIndex; i < static_cast<int>(trades.size()); ++i)
    {
        const auto& trade = trades[i];

        snapshot +=
            std::to_string(trade.price) + "," +
            std::to_string(trade.quantity) + "\n";
    }

    snapshot += "END_TRADES\n";

    snapshot += "STATS\n";

    snapshot +=
        std::to_string(getTotalTrades()) + "," +
        std::to_string(getTotalVolume()) + "," +
        std::to_string(getVWAP()) + "," +
        std::to_string(getAveragePrice(clientId)) + "," +
        std::to_string(getLatestLatency()) + "," +
        std::to_string(getAverageLatency()) + "," +
        std::to_string(getP95Latency()) + "\n";
        
    snapshot += "END_STATS\n";

    snapshot += "MARKET\n";

    snapshot +=
        std::to_string(getBestBid()) + "," +
        std::to_string(getBestAsk()) + "," +
        std::to_string(getSpread()) + "," +
        std::to_string(getCurrentPosition(clientId)) + "\n";

    snapshot += "END_MARKET\n";

    snapshot += "DEPTH\n";
    snapshot += getMarketDepthSnapshot();
    snapshot += "END_DEPTH\n";
    
    snapshot += "END_SNAPSHOT\n";

    return snapshot;
}

std::string MatchEngine::getMarketDepthSnapshot() const
{
    std::string depth;

    std::map<double, int, std::greater<double>> buyDepth;

    auto buyQueue = book.buyOrders;

    while (!buyQueue.empty())
    {
        Order buy = buyQueue.top();
        buyQueue.pop();

        buyDepth[buy.price] += buy.quantity;
    }

    std::map<double, int> sellDepth;

    auto sellQueue = book.sellOrders;

    while (!sellQueue.empty())
    {
        Order sell = sellQueue.top();
        sellQueue.pop();

        sellDepth[sell.price] += sell.quantity;
    }

    auto buyIt = buyDepth.begin();
    auto sellIt = sellDepth.begin();

    while (buyIt != buyDepth.end() || sellIt != sellDepth.end())
    {
        // BUY SIDE
        if (buyIt != buyDepth.end())
        {
            depth +=
                std::to_string(buyIt->second) + "," +
                std::to_string(buyIt->first);
            ++buyIt;
        }
        else
        {
            depth += ",";
        }

        depth += ",";

        // SELL SIDE
        if (sellIt != sellDepth.end())
        {
            depth +=
                std::to_string(sellIt->first) + "," +
                std::to_string(sellIt->second);
            ++sellIt;
        }
        else
        {
            depth += ",";
        }

        depth += "\n";
    }

    return depth;
}

double MatchEngine::getBestBid() const
{
    if (book.buyOrders.empty())
        return 0.0;

    return book.buyOrders.top().price;
}

double MatchEngine::getBestAsk() const
{
    if (book.sellOrders.empty())
        return 0.0;

    return book.sellOrders.top().price;
}

double MatchEngine::getSpread() const
{
    if (book.buyOrders.empty() || book.sellOrders.empty())
        return 0.0;

    return getBestAsk() - getBestBid();
}

int MatchEngine::getTotalTrades() const
{
    return static_cast<int>(trades.size());
}

int MatchEngine::getTotalVolume() const
{
    int totalVolume = 0;

    for (const auto& trade : trades)
    {
        totalVolume += trade.quantity;
    }

    return totalVolume;
}

double MatchEngine::getVWAP() const
{
    if (trades.empty())
        return 0.0;

    double totalValue = 0.0;
    int totalVolume = 0;

    for (const auto& trade : trades)
    {
        totalValue += trade.price * trade.quantity;
        totalVolume += trade.quantity;
    }

    return totalValue / totalVolume;
}

double MatchEngine::getAveragePrice() const
{
    if (trades.empty())
        return 0.0;

    double totalValue = 0.0;
    int totalVolume = 0;

    for (const auto& trade : trades)
    {
        totalValue += trade.price * trade.quantity;
        totalVolume += trade.quantity;
    }

    return totalValue / totalVolume;
}

double MatchEngine::getAverageLatency() const
{
    if (latencySamples.empty())
        return 0.0;

    return static_cast<double>(totalLatency) /
           latencySamples.size();
}

long long MatchEngine::getP95Latency()
{
    if (latencySamples.empty())
        return 0;

    std::vector<long long> sorted = latencySamples;

    std::sort(sorted.begin(), sorted.end());

    size_t index =
        static_cast<size_t>(0.95 * sorted.size());

    if (index >= sorted.size())
        index = sorted.size() - 1;

    return sorted[index];
}

long long MatchEngine::getTotalOrdersProcessed() const
{
    return totalOrdersProcessed.load();
}
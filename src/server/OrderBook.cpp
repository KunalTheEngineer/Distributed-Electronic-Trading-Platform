#include "OrderBook.h"

void OrderBook::addOrder(const Order& order)
{
        if(order.type == OrderType::BUY)
        {
            buyOrders.push(order);
        }
        else
        {
            sellOrders.push(order);
        }
}

bool OrderBook::cancelOrder(int orderId)
{
    bool found = false;

    // Rebuild BUY queue
    std::priority_queue<Order, std::vector<Order>, BuyComparator> tempBuy;

    while (!buyOrders.empty())
    {
        Order order = buyOrders.top();
        buyOrders.pop();

        if (order.id == orderId)
        {
            found = true;
            continue;
        }

        tempBuy.push(order);
    }

    buyOrders = std::move(tempBuy);

    // Rebuild SELL queue
    std::priority_queue<Order, std::vector<Order>, SellComparator> tempSell;

    while (!sellOrders.empty())
    {
        Order order = sellOrders.top();
        sellOrders.pop();

        if (order.id == orderId)
        {
            found = true;
            continue;
        }

        tempSell.push(order);
    }

    sellOrders = std::move(tempSell);

    return found;
}
#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include "Trade.h"

#include "OrderBook.h"

class MatchEngine
{
public:

    OrderBook book;
    std::vector<Trade> trades;

    void processOrder(const Order& order)
    {
        book.addOrder(order);

        matchOrders();
    }

    void matchOrders()
    {
        while(
            !book.buyOrders.empty() &&
            !book.sellOrders.empty()
        )
        {
            Order topBuy = book.buyOrders.top();
            Order topSell = book.sellOrders.top();

            // MATCH CONDITION
            if(topBuy.price >= topSell.price)
            {
                // Find matched quantity
                int tradedQty = std::min(topBuy.quantity, topSell.quantity);

                std::cout
                    << "TRADE EXECUTED -> "
                    << "Price: "
                    << topSell.price
                    << " Qty: "
                    << tradedQty
                    << std::endl;

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
};
#pragma once
#include<queue>
#include<vector>
#include "../models/Order.h"
#include "Comparators.h"

class OrderBook
{
    public:

        std::priority_queue<Order, std::vector<Order>, BuyComparator> buyOrders;
        std::priority_queue<Order, std::vector<Order>, SellComparator> sellOrders;
  
        void addOrder(const Order& order)
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
};
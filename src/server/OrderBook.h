#pragma once
#include<queue>
#include<vector>
#include "../shared/Order.h"
#include "Comparators.h"

class OrderBook
{
    public:

        std::priority_queue<Order, std::vector<Order>, BuyComparator> buyOrders;
        std::priority_queue<Order, std::vector<Order>, SellComparator> sellOrders;
  
        void addOrder(const Order& order);

        bool cancelOrder(int orderId);
      
};
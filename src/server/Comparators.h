#pragma once
#include "../shared/Order.h"

struct BuyComparator
{
    bool operator()(const Order& a, const Order& b)
    {
        return a.price < b.price;
    }
};

struct SellComparator
{
    bool operator()(const Order& a, const Order& b)
    {
        return a.price > b.price;
    }
};
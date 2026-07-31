#pragma once

#include <string>

enum class OrderType
{
    BUY,
    SELL,
    MARKET_BUY,
    MARKET_SELL
};

class Order
{
public:

    int id;
    int clientId;
    std::string symbol;
    double price;
    int quantity;
    OrderType type;

    Order()
    : id(0),
      clientId(0),
      symbol(""),
      price(0.0),
      quantity(0),
      type(OrderType::BUY)
    {   
    }

    Order(
        int id,
        int clientId,
        const std::string& symbol,
        double price,
        int quantity,
        OrderType type)
        :
        id(id),
        clientId(clientId),
        symbol(symbol),
        price(price),
        quantity(quantity),
        type(type)
    {
    }
};
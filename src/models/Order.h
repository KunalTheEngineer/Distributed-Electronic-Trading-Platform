#pragma once

enum class OrderType
{
    BUY,
    SELL
};

class Order
{
public:

    int id;
    double price;
    int quantity;
    OrderType type;

    Order(
        int id,
        double price,
        int quantity,
        OrderType type
    )
    {
        this->id = id;
        this->price = price;
        this->quantity = quantity;
        this->type = type;
    }
};
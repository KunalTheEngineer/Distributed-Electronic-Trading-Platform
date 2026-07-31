#pragma once

class Trade
{
    public:
        double price;
        int quantity;

        Trade(double price, int quantity)
        {
            this->price = price;
            this->quantity = quantity;
        }
};
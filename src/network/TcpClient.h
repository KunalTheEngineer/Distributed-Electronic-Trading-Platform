#pragma once

#include <boost/asio.hpp>
#include <memory>
#include "../shared/Order.h"

class TcpClient
{
public:
    TcpClient();

    ~TcpClient();

    bool connect(const std::string& ipAddress,
                 unsigned short port);

    void sendMessage(const std::string& message);

    void sendOrder(const Order& order);

    std::string receiveMessage();

    void sendCancelOrder(int orderId);


private:
    boost::asio::io_context ioContext;
    std::unique_ptr<boost::asio::ip::tcp::socket> socket;

    boost::asio::streambuf receiveBuffer;
};
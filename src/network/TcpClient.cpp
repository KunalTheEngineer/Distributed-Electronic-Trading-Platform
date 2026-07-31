#include "TcpClient.h"
#include <array>

using boost::asio::ip::tcp;

TcpClient::TcpClient()
    : ioContext()
{
    socket = std::make_unique<tcp::socket>(ioContext);
}

TcpClient::~TcpClient()
{
    if (socket && socket->is_open())
    {
        boost::system::error_code ec;

        socket->shutdown(
            boost::asio::ip::tcp::socket::shutdown_both,
            ec);

        socket->close(ec);
    }
}

bool TcpClient::connect(const std::string& ipAddress, unsigned short port)
{
    try
    {
        tcp::endpoint endpoint(
            boost::asio::ip::make_address(ipAddress),
            port);

        socket->connect(endpoint);

        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }
}

void TcpClient::sendMessage(const std::string& message)
{
    boost::asio::write(
        *socket,
        boost::asio::buffer(message)
    );
}

void TcpClient::sendOrder(const Order& order)
{
    std::string side;

    switch (order.type)
    {
    case OrderType::BUY:
        side = "BUY";
        break;

    case OrderType::SELL:
        side = "SELL";
        break;

    case OrderType::MARKET_BUY:
        side = "MKT_BUY";
        break;

    case OrderType::MARKET_SELL:
        side = "MKT_SELL";
        break;
    }

    std::string message =
        side + "," +
        order.symbol + "," +
        std::to_string(order.quantity) + "," +
        std::to_string(order.price) +
        "\n";

    

    sendMessage(message);
}

std::string TcpClient::receiveMessage()
{
    boost::asio::read_until(
        *socket,
        receiveBuffer,
        "END_SNAPSHOT\n"
    );

    std::istream input(&receiveBuffer);

    std::string snapshot;
    std::string line;

    while (std::getline(input, line))
    {
        snapshot += line + "\n";

        if (line == "END_SNAPSHOT")
            break;
    }

    return snapshot;
}

void TcpClient::sendCancelOrder(int orderId)
{
    std::string message =
        "CANCEL," + std::to_string(orderId) + "\n";

    sendMessage(message);
}



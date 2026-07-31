#include "ClientSession.h"
#include "../network/MessageParser.h"
#include "../server/MatchingEngine.h"

#include <iostream>

ClientSession::ClientSession(
    boost::asio::ip::tcp::socket socket,
    MatchEngine& engine,
    int clientId,
    std::function<void()> onMarketUpdated)
    : socket(std::move(socket)),
      engine(engine),
      clientId(clientId),
      onMarketUpdated(std::move(onMarketUpdated))
{
}

void ClientSession::start()
{
    doRead();
}

void ClientSession::sendMessage(const std::string& message)
{
    bool startWrite = false;

    {
        std::lock_guard<std::mutex> lock(sendMutex);

        outgoingMessages.push_back(message);

        if (!writeInProgress)
        {
            writeInProgress = true;
            startWrite = true;
        }
    }

    if (startWrite)
    {
        doWrite();
    }
}

void ClientSession::sendSnapshot()
{
    sendMessage(engine.getOrderBookSnapshot(clientId));
}

void ClientSession::doWrite()
{
   
    boost::asio::async_write(
        socket,
        boost::asio::buffer(outgoingMessages.front()),
        [this](boost::system::error_code ec, std::size_t /*length*/)
        {
            bool startNextWrite = false;

            {
                std::lock_guard<std::mutex> lock(sendMutex);

                if (ec)
                {
                    writeInProgress = false;
                    return;
                }

                outgoingMessages.pop_front();

                if (!outgoingMessages.empty())
                {
                    startNextWrite = true;
                }
                else
                {
                    writeInProgress = false;
                }
            }

            if (startNextWrite)
            {
                doWrite();
            }
        });
}

void ClientSession::doRead()
{
    boost::asio::async_read_until(
        socket,
        readBuffer,
        '\n',
        [this](boost::system::error_code ec, std::size_t /*bytesTransferred*/)
        {
            if (ec)
            {
                std::cout << "Client Disconnected" << std::endl; 
                return;
            }

            std::istream input(&readBuffer);

            std::string message;

            std::getline(input, message);

            if (message.rfind("CANCEL,", 0) == 0)
            {
                int orderId = std::stoi(message.substr(7));

                engine.cancelOrder(orderId);
            }
            else
            {
                Order order = MessageParser::parse(message, clientId);

                engine.processOrder(order);
            }

            onMarketUpdated();

            doRead();
        });
}

void ClientSession::stop()
{
    boost::system::error_code ec;

    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
    socket.close(ec);
}
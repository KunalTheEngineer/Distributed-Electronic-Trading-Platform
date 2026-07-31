#pragma once

#include <boost/asio.hpp>

#include "../server/MatchingEngine.h"
#include <functional>
#include <deque>
#include <mutex>

class ClientSession
{
public:
        ClientSession(boost::asio::ip::tcp::socket socket,
                        MatchEngine& engine,
                        int clientId, std::function<void()> onMarketUpdated);

        int getClientId() const
        {
            return clientId;
        }

    void start();

    void sendSnapshot();

    void doWrite();

    void stop();

private:

        void doRead();

boost::asio::streambuf readBuffer;

        void sendMessage(const std::string& message);

        boost::asio::ip::tcp::socket socket;

        MatchEngine& engine;

        int clientId;

        std::function<void()> onMarketUpdated;
        
        std::deque<std::string> outgoingMessages;

        bool writeInProgress = false;

        std::mutex sendMutex;

};
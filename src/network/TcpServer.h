#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

#include "../server/ClientSession.h"
#include "../server/TradingServer.h"

class TcpServer
{
public:
    TcpServer();

    ~TcpServer();

    void start();

    void startAccept();
    

private:

    void acceptClient();

    boost::asio::io_context ioContext;

    std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor;

    TradingServer tradingServer;   

    std::vector<std::shared_ptr<ClientSession>> clients;

    std::mutex clientsMutex;
       
    std::thread publisherThread;

    std::vector<std::thread> ioThreads;
    
    std::atomic<bool> publisherRunning{ true };

    std::atomic<bool> running{true};

    void publishSnapshots();

    int nextClientId = 1;
};
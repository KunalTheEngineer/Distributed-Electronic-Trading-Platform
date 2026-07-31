#include "TcpServer.h"
#include <utility>

#include <iostream>
#include <Windows.h>

using boost::asio::ip::tcp;

TcpServer::TcpServer()
    : ioContext()
{
    acceptor = std::make_unique<tcp::acceptor>(ioContext);

}

TcpServer::~TcpServer()
{
    publisherRunning = false;

    if (publisherThread.joinable())
    {
        publisherThread.join();
    }

    {
        std::lock_guard<std::mutex> lock(clientsMutex);

        for (auto& client : clients)
        {
            if (client)
            {
                client->stop();
            }
        }
    }

    ioContext.stop();   // <-- Add this line

    for (auto& thread : ioThreads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
}

void TcpServer::start()
{

    acceptor->open(tcp::v4());

     boost::system::error_code ec;

    acceptor->bind(tcp::endpoint(tcp::v4(), 50000), ec);

    if(ec)
    {
    MessageBoxA(nullptr,
                ec.message().c_str(),
                "Bind Error",
                MB_OK);

    return;
    }

    acceptor->listen();

    publisherThread = std::thread(&TcpServer::publishSnapshots, this);

    std::cout << "=====================================\n";
    std::cout << "Trading TCP Server Started\n";
    std::cout << "Listening on port 50000...\n";
    std::cout << "=====================================\n";

    startAccept();

    const unsigned int threadCount =
    std::max(2u, std::thread::hardware_concurrency());

     std::cout << "IO Thread Pool Size : "
            << threadCount << std::endl;

    for (unsigned int i = 0; i < threadCount; i++)
    {
        ioThreads.emplace_back([this]()
        {
            ioContext.run();
        });
    }

    for (auto& thread : ioThreads)
    {
        thread.join();
    }

}

void TcpServer::acceptClient()
{
}

void TcpServer::publishSnapshots()
{
    using namespace std::chrono_literals;

    while (publisherRunning)
    {
        {
            std::lock_guard<std::mutex> lock(clientsMutex);

            for (const auto& client : clients)
            {
                if (client)
                {
                    client->sendSnapshot();
                }
            }
        }

        std::this_thread::sleep_for(50ms);
    }
}

void TcpServer::startAccept()
{
    auto socket = std::make_shared<tcp::socket>(ioContext);

    acceptor->async_accept(
        *socket,
        [this, socket](boost::system::error_code ec)
        {
            if (!ec)
            {
                int clientId = nextClientId++;

                    std::cout << "=====================================\n";
                    std::cout << "Client Connected\n";
                    std::cout << "Assigned Client ID : " << clientId << "\n";
                    std::cout << "=====================================\n";

                    auto session = std::make_shared<ClientSession>(
                    std::move(*socket),
                    tradingServer.getMatchEngine(),
                    clientId,
                    []()
                    {
                        // Snapshot publishing handled by publisher thread
                    });

                {
                    std::lock_guard<std::mutex> lock(clientsMutex);
                    clients.push_back(session);
                }

                    session->start();
                }
            startAccept();
        });
}

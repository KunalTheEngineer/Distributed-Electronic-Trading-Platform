/*#include <iostream>
#include <random>
#include <chrono>

#include "network/TcpClient.h"

constexpr int TOTAL_ORDERS = 100000;

int main()
{
    TcpClient client;

    if (!client.connect("127.0.0.1", 50000))
    {
        std::cout << "Failed to connect to Trading Server.\n";
        return 1;
    }

    auto start = std::chrono::high_resolution_clock::now();

    int sentOrders = 0;

    for (int i = 0; i < TOTAL_ORDERS; i++)
{
    Order order;

    order.clientId = 1;
    order.symbol = "AAPL";
    order.price = 100.0;
    order.quantity = 10;

    if (i % 2 == 0)
    {
        order.type = OrderType::BUY;
    }
    else
    {
        order.type = OrderType::SELL;
    }

    client.sendOrder(order);

    sentOrders++;

}

     auto end = std::chrono::high_resolution_clock::now();

double elapsedSeconds =
    std::chrono::duration<double>(end - start).count();

double ordersPerSecond =
    TOTAL_ORDERS / elapsedSeconds;

std::cout << "\n=====================================\n";
std::cout << "        LOAD TEST REPORT\n";
std::cout << "=====================================\n";
std::cout << "Orders Sent      : " << TOTAL_ORDERS << '\n';
std::cout << "Elapsed Time     : " << elapsedSeconds << " sec\n";
std::cout << "Orders / Second  : " << static_cast<int>(ordersPerSecond) << '\n';
std::cout << "=====================================\n";

// Benchmark completed

std::cout << "Press ENTER to exit...";
std::cin.get();

return 0;
}*/
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

#include "network/TcpClient.h"

constexpr int NUM_CLIENTS = 5;
constexpr int ORDERS_PER_CLIENT = 50000;
constexpr int TOTAL_ORDERS = NUM_CLIENTS * ORDERS_PER_CLIENT;

void clientWorker(int clientId)
{
    TcpClient client;

    if (!client.connect("127.0.0.1", 50000))
    {
        std::cout << "Client " << clientId << " failed to connect.\n";
        return;
    }

    for (int i = 0; i < ORDERS_PER_CLIENT; i++)
    {
        Order order;

        order.clientId = clientId;
        order.symbol = "AAPL";
        order.price = 100.0;
        order.quantity = 10;

        if (i % 2 == 0)
            order.type = OrderType::BUY;
        else
            order.type = OrderType::SELL;

        client.sendOrder(order);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> clients;

    for (int i = 1; i <= NUM_CLIENTS; i++)
    {
        clients.emplace_back(clientWorker, i);
    }

    for (auto& t : clients)
    {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();

    double elapsedSeconds =
        std::chrono::duration<double>(end - start).count();

    double ordersPerSecond =
        TOTAL_ORDERS / elapsedSeconds;

    std::cout << "\n=====================================\n";
    std::cout << "        LOAD TEST REPORT\n";
    std::cout << "=====================================\n";
    std::cout << "Clients          : " << NUM_CLIENTS << '\n';
    std::cout << "Orders Sent      : " << TOTAL_ORDERS << '\n';
    std::cout << "Elapsed Time     : " << elapsedSeconds << " sec\n";
    std::cout << "Orders / Second  : " << static_cast<int>(ordersPerSecond) << '\n';
    std::cout << "=====================================\n";

    std::cout << "\nPress ENTER to exit...";
    std::cin.get();

    return 0;
}
# 🚀 Distributed Electronic Trading Platform

> A high-performance Distributed Electronic Trading Platform built in **Modern C++**, featuring a **multi-threaded matching engine**, **Boost.Asio TCP networking**, **real-time order book management**, a **Qt-based trading dashboard**, and a **load testing framework**.

---

## 📌 Overview

The **Distributed Electronic Trading Platform** is a simulation of a modern electronic exchange that demonstrates how client orders are transmitted over a TCP network, processed by a high-performance matching engine, and reflected in a real-time order book.

The project is designed with a modular architecture, separating networking, order matching, server management, shared data models, and the graphical user interface into independent components. It showcases concepts commonly used in electronic trading systems such as low-latency communication, concurrent client handling, price-time priority (FIFO) order matching, partial order execution, and performance benchmarking.

The platform also includes a dedicated **Load Tester** capable of simulating multiple trading clients to evaluate throughput and latency under concurrent workloads.

---

## ✨ Key Features

### Matching Engine

- Price-Time Priority (FIFO) Matching
- Limit Buy & Sell Orders
- Partial Order Execution
- Order Cancellation
- Trade History
- Position Tracking
- VWAP (Volume Weighted Average Price)
- Market Depth
- Performance Metrics
- Latency Measurement

---

### Networking

- TCP Client-Server Architecture
- Boost.Asio Networking
- Multi-Client Support
- Concurrent Session Handling
- Thread-Safe Communication

---

### GUI

- Qt 6 Trading Dashboard
- Real-Time Order Book
- Trade History Display
- Market Depth Visualization

---

### Performance

- Multi-threaded Order Processing
- Concurrent Client Simulation
- Load Testing Framework
- Throughput Benchmarking
- Average / Latest / P95 Latency Tracking

---

## 🛠 Technology Stack

| Category | Technologies |
|-----------|--------------|
| Language | C++17 |
| Networking | Boost.Asio |
| GUI | Qt 6 |
| Build System | CMake |
| Concurrency | C++ Threads |
| Version Control | Git & GitHub |


---

# 🏗️ System Architecture

The platform follows a modular distributed architecture where multiple trading clients communicate with a centralized matching engine over TCP using Boost.Asio.

```text
                          +----------------------+
                          |   Trading Clients    |
                          |  (Multiple Clients)  |
                          +----------+-----------+
                                     |
                                     |
                             TCP (Boost.Asio)
                                     |
                                     ▼
                      +-----------------------------+
                      |       TCP Trading Server     |
                      +-----------------------------+
                                     |
                      Creates Client Sessions
                                     |
                                     ▼
                     +------------------------------+
                     |        Client Session         |
                     |  (One Session per Client)     |
                     +---------------+--------------+
                                     |
                                     ▼
                     +------------------------------+
                     |       Matching Engine         |
                     +---------------+--------------+
                                     |
                 +-------------------+-------------------+
                 |                                       |
                 ▼                                       ▼
          +--------------+                     +----------------+
          |  Order Book  |                     | Trade History  |
          +--------------+                     +----------------+
                 |
                 ▼
          +--------------+
          | Market Depth |
          +--------------+
                 |
                 ▼
         +------------------+
         |   Qt Dashboard   |
         +------------------+
```

---

# ⚙️ Core Components

| Component | Responsibility |
|-----------|----------------|
| Trading Client | Sends buy/sell orders to the server |
| TCP Server | Accepts multiple client connections |
| Client Session | Manages communication with an individual client |
| Matching Engine | Executes price-time priority matching |
| Order Book | Maintains active buy and sell orders |
| Trade History | Stores executed trades |
| Qt Dashboard | Displays market data and trading activity |
| Load Tester | Simulates concurrent trading clients |

---

# 🔄 Order Execution Flow

```text
Client Places Order
        │
        ▼
TCP Client
        │
        ▼
Boost.Asio TCP Server
        │
        ▼
Client Session
        │
        ▼
Message Parser
        │
        ▼
Matching Engine
        │
        ├──────────────► Match Found
        │                    │
        │                    ▼
        │              Execute Trade
        │                    │
        │                    ▼
        │             Update Order Book
        │                    │
        ▼                    ▼
 Queue Remaining      Update Trade History
        │                    │
        └──────────────► Refresh GUI
```

---

# 🧵 Multithreading Model

The trading platform is designed to process multiple client connections concurrently.

### Thread Responsibilities

| Thread | Responsibility |
|---------|----------------|
| Main Thread | Starts the server and initializes resources |
| Boost.Asio I/O Threads | Handle asynchronous TCP communication |
| Client Sessions | Process requests from connected clients |
| Matching Engine | Executes order matching safely |
| GUI Thread | Updates the Qt dashboard |
| Load Tester Threads | Simulate concurrent trading clients |

The networking layer is implemented using **Boost.Asio** asynchronous operations, allowing multiple clients to communicate with the server concurrently while the matching engine processes incoming orders.

---

# 📂 Repository Structure

```text
Distributed-Electronic-Trading-Platform
│
├── docs/
│   ├── architecture/
│   ├── diagrams/
│   └── screenshots/
│
├── src/
│   ├── client/
│   ├── network/
│   ├── server/
│   └── shared/
│
├── TradingLoadTester/
│
├── Build.bat
├── RunServer.bat
├── RunClient.bat
├── CMakeLists.txt
└── README.md
```
---

# 📊 Performance Highlights

The platform includes a dedicated load-testing utility capable of simulating multiple concurrent trading clients.

Performance metrics collected include:

- Latest Order Processing Latency
- Average Latency
- P95 Latency
- Total Executed Trades
- Total Trading Volume
- Concurrent Client Support
- Throughput Benchmarking
- Multi-threaded Processing

These metrics help evaluate the behaviour of the matching engine under concurrent workloads and provide visibility into system performance.

---

---

# 💡 Engineering Concepts Demonstrated

- Distributed Client–Server Architecture
- TCP Socket Programming using Boost.Asio
- Asynchronous Networking
- Multi-threaded Programming
- Thread Synchronization
- Price-Time Priority (FIFO) Matching
- Order Book Management
- Partial Order Execution
- Concurrent Session Handling
- Performance Benchmarking
- Modular Software Design
- Modern C++17 Practices

---

# 🚀 Getting Started

## Prerequisites

Before building the project, ensure the following tools are installed:

- C++17 Compatible Compiler (MSVC 2022 Recommended)
- CMake 3.20 or later
- Qt 6.x
- Boost Library (Boost.Asio)
- Git

---

## Clone Repository

```bash
git clone https://github.com/KunalTheEngineer/Distributed-Electronic-Trading-Platform.git

cd Distributed-Electronic-Trading-Platform
```

---

## Configure Project

```bash
cmake -S . -B build
```

---

## Build Project

```bash
cmake --build build --config Release
```

---

## Run Server

```bash
RunServer.bat
```

---

## Run Client

```bash
RunClient.bat
```

---

## Run Load Tester

```bash
TradingLoadTester.exe
```

---

# 🔮 Future Enhancements

The platform can be extended with several production-grade features, including:

- Support for Market Buy and Market Sell Orders
- Stop-Loss and Stop-Limit Orders
- FIX Protocol Integration
- WebSocket Market Data Streaming
- Persistent Database Storage
- User Authentication and Authorization
- Risk Management Module
- Real-Time Analytics Dashboard
- Docker-Based Deployment
- Cross-Platform Support (Linux & macOS)
- Unit and Integration Testing
- Continuous Integration (GitHub Actions)

---

# 📚 Learning Outcomes

Through this project, I gained hands-on experience with:

- Designing distributed client-server applications
- Implementing TCP networking using Boost.Asio
- Developing a multi-threaded matching engine
- Applying price-time priority (FIFO) algorithms
- Managing concurrent client sessions
- Building desktop applications using Qt
- Measuring latency and benchmarking performance
- Organizing large-scale C++ projects with CMake
- Using Git and GitHub for version control

- ---

# 👨‍💻 Author

**Kunal Thakare**

Software Engineering Enthusiast with interests in:

- Modern C++
- Distributed Systems
- Low-Latency Trading Systems
- System Design
- Backend Development

### Connect with Me

- GitHub: https://github.com/KunalTheEngineer
- LinkedIn: https://www.linkedin.com/in/kunalthakareer/

If you found this project useful, consider giving it a ⭐ on GitHub.

---

# 🙏 Acknowledgements

This project was developed as a learning exercise to explore distributed systems, networking, concurrency, and modern C++ software design. It draws inspiration from the architecture and concepts used in electronic trading platforms while remaining an educational implementation.

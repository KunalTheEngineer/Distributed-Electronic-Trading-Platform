# Real-Time Trading Engine

A real-time multithreaded trading engine built using C++ and Qt.

## Features

- FIFO Order Matching Engine
- Buy & Sell Limit Orders
- Market Orders
- Trade History
- Market Depth
- Position Tracking
- PnL Calculation
- Execution Latency Tracking
- Order Cancellation
- Thread-Safe Order Queue
- Multithreaded Matching Engine
- Qt GUI Dashboard

## Technologies Used

- C++
- Qt Framework
- STL
- Multithreading
- Mutex
- Condition Variables
- CMake
- Git & GitHub

## Architecture

```text
GUI
 ↓
Order Queue
 ↓
Matching Engine Thread
 ↓
Order Book
 ↓
Trade Execution
```

## Project Structure

```text
TradingGUI/
│
├── src/
│   ├── main.cpp
│   ├── engine/
│   └── models/
│
├── build/
├── CMakeLists.txt
├── run.bat
└── .gitignore
```

## Key Concepts Implemented

- FIFO Matching
- Market Depth Aggregation
- Order Book Management
- Low Latency Processing
- Thread Synchronization
- Queue-Based Processing

## Future Enhancements

- Socket-Based Market Data Feed
- Exchange Simulator
- CSV Trade Export
- Performance Analytics
- Real-Time Charts

## Author

Kunal Thakare

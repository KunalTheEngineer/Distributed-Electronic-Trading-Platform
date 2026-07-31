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

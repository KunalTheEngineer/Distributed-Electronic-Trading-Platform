cmake_minimum_required(VERSION 3.16)

project(TradingGUI)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ==========================================
# Shared Source Files
# ==========================================

set(COMMON_SOURCES
    src/server/TradingServer.cpp
    src/server/MatchingEngine.cpp
    src/server/OrderBook.cpp

    src/network/TcpServer.cpp
    src/network/TcpClient.cpp

    src/server/ClientSession.cpp
    src/network/MessageParser.cpp
)

# ==========================================
# Trading Client
# ==========================================

add_executable(TradingClient WIN32
    src/main.cpp
    ${COMMON_SOURCES}
)

target_link_libraries(TradingClient
    Qt6::Widgets
    Boost::system
)

target_compile_definitions(TradingClient PRIVATE _WIN32_WINNT=0x0A00)

# ==========================================
# Trading Server
# ==========================================

add_executable(TradingServer
    src/ServerMain.cpp
    ${COMMON_SOURCES}
)

add_subdirectory(TradingLoadTester)

target_link_libraries(TradingServer
    Boost::system
)

target_compile_definitions(TradingServer PRIVATE _WIN32_WINNT=0x0A00)
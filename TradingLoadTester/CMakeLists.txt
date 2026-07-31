add_executable(TradingLoadTester
    main.cpp

    ${CMAKE_SOURCE_DIR}/src/network/TcpClient.cpp
)

target_include_directories(TradingLoadTester PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(TradingLoadTester PRIVATE
    Boost::system
)

target_compile_definitions(TradingLoadTester PRIVATE
    _WIN32_WINNT=0x0A00
)
#include "MessageParser.h"

#include <sstream>
#include <vector>

Order MessageParser::parse(const std::string& message, int clientId)
{
    std::stringstream stream(message);

    std::string token;

    std::vector<std::string> tokens;

    while (std::getline(stream, token, ','))
    {
        tokens.push_back(token);
    }

    if (tokens.size() != 4)
    {
        throw std::runtime_error("Invalid message: [" + message + "]");
    }

    OrderType type;

    if (tokens[0] == "BUY")
    {
        type = OrderType::BUY;
    }
    else
    {
        type = OrderType::SELL;
    }


    return Order(
        1,
        clientId,
        tokens[1],
        std::stod(tokens[3]),
        std::stoi(tokens[2]),
        type
    );
}

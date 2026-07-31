#pragma once

#include <string>
#include "../shared/Order.h"

class MessageParser
{
public:
        static Order parse(const std::string& message, int clientId);
};
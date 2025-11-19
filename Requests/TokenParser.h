#pragma once

#include <string>
#include <vector>

class TokenParser {
public:
    static std::vector<std::string> extractTokens(const std::string& jsonText);
};



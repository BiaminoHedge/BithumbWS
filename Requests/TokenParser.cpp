#include "TokenParser.h"

#include <unordered_set>
#include <cctype>

static bool IsTokenChar(char c)
{
    unsigned char uc = static_cast<unsigned char>(c);
    return std::isalnum(uc) || c == '.' || c == '_' || c == '-';
}

std::vector<std::string> TokenParser::extractTokens(const std::string& jsonText)
{
    std::unordered_set<std::string> seen;
    std::vector<std::string> tokens;

    const std::string key = "\"title\"";
    std::size_t pos = 0;
    while (true) {
        pos = jsonText.find(key, pos);
        if (pos == std::string::npos) break;
        pos += key.size();

        while (pos < jsonText.size() && std::isspace(static_cast<unsigned char>(jsonText[pos]))) pos++;
        if (pos >= jsonText.size() || jsonText[pos] != ':') continue;
        pos++;
        while (pos < jsonText.size() && std::isspace(static_cast<unsigned char>(jsonText[pos]))) pos++;
        if (pos >= jsonText.size() || jsonText[pos] != '"') continue;
        pos++;

        std::string title;
        bool esc = false;
        for (; pos < jsonText.size(); ++pos) {
            char c = jsonText[pos];
            if (esc) { title.push_back(c); esc = false; }
            else if (c == '\\') esc = true;
            else if (c == '"') { pos++; break; }
            else title.push_back(c);
        }

        for (std::size_t i = 0; i < title.size(); ++i) {
            if (title[i] == '(') {
                std::size_t j = i + 1;
                std::string tok;
                while (j < title.size() && IsTokenChar(title[j])) { tok.push_back(title[j]); j++; }
                if (j < title.size() && title[j] == ')' && !tok.empty()) {
                    if (seen.insert(tok).second) tokens.push_back(tok);
                    i = j;
                }
            }
        }
    }
    return tokens;
}



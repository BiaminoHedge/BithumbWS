#include "ProxyManager.h"
#include <fstream>
#include <algorithm>

static inline std::string trim(const std::string& s) {
    size_t a = 0; while (a < s.size() && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '\n')) ++a;
    size_t b = s.size(); while (b > a && (s[b-1] == ' ' || s[b-1] == '\t' || s[b-1] == '\r' || s[b-1] == '\n')) --b;
    return s.substr(a, b - a);
}

ProxyManager::ProxyManager(const std::string& proxiesFilePath) {
    loadFromFile(proxiesFilePath);
}

void ProxyManager::loadFromFile(const std::string& path) {
    std::ifstream in(path);
    if (!in.good()) return;
    std::string line;
    while (std::getline(in, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (line[0] == '#') continue;
        proxies_.push_back(line);
    }
}

std::string ProxyManager::getCurrentProxy() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxies_.empty()) return {};
    return proxies_.front();
}

void ProxyManager::rotate() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (proxies_.size() <= 1) return;
    std::string first = proxies_.front();
    proxies_.pop_front();
    proxies_.push_back(first);
}

bool ProxyManager::hasProxy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return !proxies_.empty();
}



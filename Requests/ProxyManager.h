#pragma once

#include <string>
#include <deque>
#include <mutex>

class ProxyManager {
public:
    explicit ProxyManager(const std::string& proxiesFilePath);

    // Возвращает текущий прокси в формате host:port:user:pass или пустую строку, если прокси нет
    std::string getCurrentProxy();

    // Переносит текущий прокси в хвост очереди
    void rotate();

    // Есть ли хотя бы один прокси
    bool hasProxy() const;

private:
    void loadFromFile(const std::string& path);

    std::deque<std::string> proxies_;
    mutable std::mutex mutex_;
};



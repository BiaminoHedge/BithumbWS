#include <iostream>
#include <string>
#include <curl/curl.h>

#include "Server/WsBroadcastServer.h"
#include "Requests/BithumbFetcher.h"
#include "Requests/ProxyManager.h"
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <vector>
#include <deque>
#include <fstream>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif

// Потокобезопасная очередь строковых сообщений
struct ThreadSafeQueue {
    std::queue<std::string> q;
    std::mutex m;
    std::condition_variable cv;

    void push(std::string value) {
        {
            std::lock_guard<std::mutex> lock(m);
            q.push(std::move(value));
        }
        cv.notify_one();
    }

    std::string wait_and_pop() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [&]{ return !q.empty(); });
        std::string value = std::move(q.front());
        q.pop();
        return value;
    }
};

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    WsBroadcastServer server;
    std::cout << "Starting server on 0.0.0.0:8080\n";

    // Запускаем сервер в отдельном потоке, так как start() блокирует
    std::thread serverThread([&server]{ server.start("0.0.0.0", 8080); });

    // Очередь ответов от потоков
    ThreadSafeQueue responses;
    unsigned long long requestCount = 0;
    std::mutex counterMutex;
    // Менеджер прокси читает список из файла (один на строку: host:port:user:pass)
    ProxyManager proxyManager("./proxies.txt"); // пут под докер, вставить свои

    // каждый поток делает запрос и кладёт ответ в очередь
    constexpr int thread_count = 10;
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    const std::string userAgent = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36";

    // Подготовка cookie-файлов: создаём N файлов из шаблона cookie_main
    {
        const std::string cookieDir = std::string("./cookies/");
        const std::string baseCookiePath = cookieDir + "cookie_main.txt"; // основной файл с куками
        std::ifstream baseIn(baseCookiePath.c_str(), std::ios::in | std::ios::binary);
        if (baseIn.good()) {
            std::ostringstream ss;
            ss << baseIn.rdbuf();
            const std::string baseContent = ss.str();
            for (int i = 0; i < thread_count; ++i) {
                const std::string targetPath = cookieDir + std::string("cookies_thread_") + std::to_string(i) + ".txt";
                std::ofstream out(targetPath.c_str(), std::ios::out | std::ios::binary | std::ios::trunc);
                if (out.good()) {
                    out.write(baseContent.data(), static_cast<std::streamsize>(baseContent.size()));
                }
            }
        }
    }
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([&, i]() {
            BithumbFetcher fetcherLocal;
            std::string cookieFile = std::string("./cookies/cookies_thread_") + std::to_string(i) + ".txt"; // пут под докер, вставить свои
            fetcherLocal.setCookieFile(cookieFile);
            fetcherLocal.setUserAgent(userAgent);
            // Текущий прокси для потока
            std::string currentProxy;
            while (true) {
                // Запрос
                currentProxy = proxyManager.getCurrentProxy();
                if (!currentProxy.empty()) {
                    fetcherLocal.setProxyFromString(currentProxy);
                }
                int http = fetcherLocal.fetch();
                {
                    std::lock_guard<std::mutex> lock(counterMutex);
                    ++requestCount;
                }
                int curlCode = fetcherLocal.getLastCurlCode();
                if (curlCode == 0 && http >= 200 && http < 300) {
                    responses.push(fetcherLocal.getResponse());
                }
                // Ротация прокси при ошибке соединения или HTTP 403
                if (!currentProxy.empty() && (curlCode != 0 || http == 403)) {
                    proxyManager.rotate();
                    std::lock_guard<std::mutex> lock(counterMutex);
                    std::cout << "Rotate proxy due to error. CURL=" << http << ", proxy=" << currentProxy << std::endl;
                }
            }
        });
    }

    // забирает ответы из очереди, сравнивает и шлёт клиентам при изменении (отправляет все токены)
    std::thread consumerThread([&]() {
        std::string lastSent;
        while (true) {
            std::string msg = responses.wait_and_pop();
            if (msg != lastSent || requestCount % 10 == 0) {
                server.broadcast(msg);
                std::cout << msg << std::endl;
                lastSent = std::move(msg);
            } //else if (requestCount % 100 == 0) {
            //     server.broadcast(msg + "100й запрос");
            //     std::cout << msg << std::endl;
            // }
        }
    });

    serverThread.join();
    for (auto &t : threads) t.join();
    consumerThread.join();
}


#pragma once

#include <string>

class BithumbFetcher {
public:
    BithumbFetcher() = default;
    void setCookieFile(const std::string& path) { cookieFilePath_ = path; }
    void setUserAgent(const std::string& ua) { userAgent_ = ua; }

    void setProxyFromString(const std::string& spec);
    void disableProxy() { useProxy_ = false; }
    int fetch();
    const std::string& getResponse() const { return response_; }
    long getLastHttpCode() const { return lastHttpCode_; }
    int getLastCurlCode() const { return lastCurlCode_; }

private:
    static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* s);
    void configureRequest(void* curlHandle, std::string& writeBuffer);

    std::string response_;
    std::string cookieFilePath_;
    std::string userAgent_;
    bool useProxy_ = false;
    std::string proxyHostPort_;
    std::string proxyUserPwd_;
    long lastHttpCode_ = -1;
    int lastCurlCode_ = 0;
};



#include "BithumbFetcher.h"
#include <curl/curl.h>
#include <iostream>
#include "TokenParser.h"
void BithumbFetcher::setProxyFromString(const std::string& spec)
{
    proxyHostPort_.clear();
    proxyUserPwd_.clear();

    auto a = spec.find(':');
    if (a == std::string::npos) return;
    auto b = spec.find(':', a + 1);
    // host:port:user:pass
    proxyHostPort_ = spec.substr(0, b);
    proxyUserPwd_ = spec.substr(b + 1);
}


size_t BithumbFetcher::writeCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    try {
        s->append((char*)contents, newLength);
        return newLength;
    } catch (...) {
        return 0;
    }
}

void BithumbFetcher::configureRequest(void* curlHandle, std::string& writeBuffer) {
    std::string url = "https://feed.bithumb.com/_next/data/HMaq2yVQy0nQtliwaAqW6/notice.json?category=9&page=1";

    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, &BithumbFetcher::writeCallback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &writeBuffer);

    curl_slist* headers = nullptr;
    {
        std::string ua = userAgent_.empty() ? std::string("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/120.0.0.0 Safari/537.36") : userAgent_;
        std::string uaHeader = std::string("User-Agent: ") + ua;
        headers = curl_slist_append(headers, uaHeader.c_str());
    }
    headers = curl_slist_append(headers, "Accept: application/json, text/plain, */*");
    headers = curl_slist_append(headers, "Accept-Language: ko-KR,ko;q=0.9,en;q=0.8");
    //headers = curl_slist_append(headers, "Accept-Encoding: gzip, deflate, br");
    headers = curl_slist_append(headers, "Referer: https://feed.bithumb.com/notice?category=9&page=1");
    headers = curl_slist_append(headers, "Origin: https://feed.bithumb.com");
    headers = curl_slist_append(headers, "Pragma: no-cache");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    headers = curl_slist_append(headers, "x-nextjs-data: 1");

    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);

    if (!cookieFilePath_.empty()) {
        // Читаем и сохраняем куки между запусками в один и тот же файл
        curl_easy_setopt(curlHandle, CURLOPT_COOKIEFILE, cookieFilePath_.c_str());
        //curl_easy_setopt(curlHandle, CURLOPT_COOKIEJAR,  cookieFilePath_.c_str());
    } else {
        // Включаем in-memory куки (без записи на диск)
        curl_easy_setopt(curlHandle, CURLOPT_COOKIEFILE, "");
    }

    if (proxyHostPort_.empty()) {
        throw std::runtime_error("Invalid proxy address");
    }
    curl_easy_setopt(curlHandle, CURLOPT_PROXY, proxyHostPort_.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_PROXYUSERPWD, proxyUserPwd_.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);

    curl_easy_setopt(curlHandle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 30L);
    //curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curlHandle, CURLOPT_ENCODING, "gzip, deflate, br");
}

int BithumbFetcher::fetch() {
    response_.clear();
    CURL* curl = curl_easy_init();
    configureRequest(curl, response_);
    CURLcode res = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    lastHttpCode_ = httpCode;
    lastCurlCode_ = static_cast<int>(res);

    if (res != CURLE_OK) {
        std::cerr << "Ошибка cURL: " << curl_easy_strerror(res) << std::endl;
    } else {
        // Преобразуем ответ в JSON-массив токенов для отправки клиентам
        auto toks = TokenParser::extractTokens(response_);
        if (!toks.empty()) {
            //std::cout << toks[0] << std::endl; // если не выводит скорее всего 403
            std::string out = "[";
            for (size_t i = 0; i < toks.size(); ++i) {
                out += '"';
                out += toks[i];
                out += '"';
                if (i + 1 < toks.size()) out += ',';
            }
            out += ']';
            response_.swap(out);
        }
    }
    curl_easy_cleanup(curl);
    // Возвращаем HTTP-код ответа (0, если запроса не было/ошибка уровня cURL)
    return static_cast<int>(httpCode);
}



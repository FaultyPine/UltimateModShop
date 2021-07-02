#pragma once
#include <sys/select.h>
#include <curl/curl.h>

// Credit this cute curl object stuff to Blujay. Thanks dude
struct  CURL_builder {
    CURL* request;
    curl_slist* headers;

    CURL_builder() : request(nullptr), headers(nullptr) { request = curl_easy_init(); }
    ~CURL_builder() {
        if (request != nullptr)
            curl_easy_cleanup(request);
        if (headers != nullptr)
            curl_slist_free_all(headers);
    }

    operator bool() { return request != nullptr; } // for convenience
    template <typename T>
    CURL_builder& SetOPT(CURLoption opt, T param) { curl_easy_setopt(request, opt, param); return *this; } // generic to mimic the #define macro CURL uses
    CURL_builder& SetHeaders(std::vector<std::string> _headers) {
        if (headers != nullptr)
            curl_slist_free_all(headers);
        headers = nullptr;
        for (std::string x : _headers)
            headers = curl_slist_append(headers, x.c_str());
        SetOPT(CURLOPT_HTTPHEADER, headers);
        return *this;
    }
    CURL_builder& SetURL(const std::string& url) { SetOPT(CURLOPT_URL, url.c_str()); return *this; }
    CURLcode Perform() { return curl_easy_perform(request); }
};
#include "curl.h"

using json = nlohmann::json;

// Credit this curl stuff to Blujay. Thanks dude
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

size_t jsonWriteCallback(char* to_write, size_t size, size_t byte_count, void* user_data) {
    *(std::stringstream*)user_data << to_write;
    return size * byte_count;
}

int download_progress(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded) {
    int percent_complete = (int)((NowDownloaded/TotalToDownload)*100.0);

    // if you don't return 0, the transfer will be aborted
    return 0; 
}

CURLcode curl::curl_init() { curl_global_init(CURL_GLOBAL_DEFAULT); }

/// 'path' is a relative path to wherever the project is being run from.
CURLcode curl::DownloadFile(std::string url, std::string path) {
    CURL_builder curl;
    CURLcode result = CURLE_GOT_NOTHING;

    std::vector<std::string> headers;
        headers.push_back("Accept: application/octet-stream");

    if (curl) {
        FILE* file = fopen(path.c_str(), "wb");
        result =
        curl.SetHeaders(headers)
            .SetURL(url)
            .SetOPT(CURLOPT_WRITEDATA, file)
            .SetOPT(CURLOPT_USERAGENT, "UMS-User")
            .SetOPT(CURLOPT_FOLLOWLOCATION, 1L)
            .SetOPT(CURLOPT_SSL_VERIFYPEER, 0L)
            .SetOPT(CURLOPT_SSL_VERIFYHOST, 0L)
            .SetOPT(CURLOPT_NOPROGRESS, 0L)
            .SetOPT(CURLOPT_PROGRESSFUNCTION, download_progress)
            .Perform();
        fclose(file);
        if (result != CURLE_OK) {
            if (std::filesystem::exists(path))
                std::filesystem::remove(path);
        }
    }
    return result;
}

json curl::DownloadJson(std::string url) {
    json res = NULL;
    std::stringstream buffer;
    CURL_builder curl;
    CURLcode result = CURLE_GOT_NOTHING;

    std::vector<std::string> headers;
        headers.push_back("Accept: application/octet-stream");

    if (curl) {
        result =
            curl.SetURL(url)
                .SetOPT(CURLOPT_WRITEDATA, &buffer)
                .SetOPT(CURLOPT_WRITEFUNCTION, jsonWriteCallback)
                .SetOPT(CURLOPT_USERAGENT, "UMS-User")
                .Perform();
        if (result == CURLE_OK) {
            try { res = json::parse(buffer.str()); }
            catch (json::parse_error& e) { brls::Logger::error("Unable to parse json!"); }
        }
    }
    return res;
}
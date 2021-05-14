#include "curl.h"

using json = nlohmann::json;

MemoryStruct::~MemoryStruct() {
    free(this->memory);
}
MemoryStruct::MemoryStruct() {
    this->memory = nullptr;
    this->size = 0;
}

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


size_t writeCallback(char* to_write, size_t size, size_t byte_count, void* user_data) {

    // handle null termination
    char * responseString = new char[byte_count + 1]();
    strncpy(responseString, to_write, byte_count);
    responseString[byte_count] = '\0';

    *(std::stringstream*)user_data << responseString;
    delete responseString;
    return size * byte_count;
}

int prev_percent_complete = -1;
int download_progress(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded) {
    int percent_complete = (int)((NowDownloaded/TotalToDownload)*100.0);
    if (percent_complete >= 0 && percent_complete <= 100 && percent_complete != prev_percent_complete) {
        brls::Logger::debug("Downloading... {}%", percent_complete);
        prev_percent_complete = percent_complete;
    }

    // if you don't return 0, the transfer will be aborted
    return 0; 
}

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
    json res;
    std::stringstream buffer;
    CURL_builder curl;
    CURLcode result = CURLE_GOT_NOTHING;

    std::vector<std::string> headers;
        headers.push_back("Accept: application/json");

    if (curl) {
        result =
            curl.SetURL(url)
                .SetOPT(CURLOPT_WRITEDATA, &buffer)
                .SetOPT(CURLOPT_WRITEFUNCTION, writeCallback)
                .SetOPT(CURLOPT_USERAGENT, "UMS-User")
                .Perform();
        if (result == CURLE_OK) {
            try {
                std::string buffer_str = buffer.str();
                if (json::accept(buffer_str))
                    res = json::parse(buffer_str);
                else {
                    brls::Logger::error("Invalid json! {}\n{}", url, buffer_str);
                }
            }
            catch (json::parse_error& e) {
                std::string crash_msg = std::string("Unable to parse json!") + std::string(e.what()) + "\n" + buffer.str() + "\n" + url;
                brls::Application::crash(crash_msg);
            }
        }
        else {
            brls::Logger::error("Failed to perform curl operation. CURLcode = {}", curl_easy_strerror(result));
        }
    }
    else {
        brls::Logger::error("Failed to init curl");
    }
    return res;
}

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userdata) // <- userdata is 'chunk' (MemoryStruct)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userdata;

  char *ptr = (char*)realloc(mem->memory, mem->size + realsize + 1);

  if (ptr == NULL)
  {
      brls::Logger::error("Out of memory!");
      return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

MemoryStruct curl::DownloadToMem(std::string url) {
    CURL_builder curl;

    struct MemoryStruct chunk;
    chunk.memory = (char*)malloc(1);
    chunk.size = 0;

    std::vector<std::string> headers;
        headers.push_back("Accept: application/octet-stream");

    if (curl) {
        CURLcode result =
            curl.SetURL(url)
                .SetOPT(CURLOPT_WRITEDATA, (void *)&chunk)
                .SetOPT(CURLOPT_WRITEFUNCTION, write_memory_callback)
                .SetOPT(CURLOPT_USERAGENT, "UMS-User")
                .Perform();
        if (result != CURLE_OK) {
            brls::Logger::error("Failed to download to mem!");
        }
    }
    return chunk;
}
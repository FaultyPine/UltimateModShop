#pragma once

#include "utils.h"
#include <curl/curl.h>

using json = nlohmann::json;


namespace curl {

    CURLcode curl_init();

    CURLcode DownloadFile(std::string url, std::string path);

    json DownloadJson(std::string url);

}
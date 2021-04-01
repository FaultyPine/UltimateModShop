#pragma once

#include "ums_utils.h"
#include <sys/select.h>
#include <curl/curl.h>

using json = nlohmann::json;


namespace curl {

    CURLcode DownloadFile(std::string url, std::string path);

    json DownloadJson(std::string url);

}
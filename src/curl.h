#pragma once

#include "ums_utils.h"
#include "curl_builder.h"

using json = nlohmann::json;


namespace curl {

    CURLcode DownloadFile(std::string url, std::string path, CURL_builder* curl = nullptr);

    json DownloadJson(std::string url, CURL_builder* curl = nullptr);

    MemoryStruct DownloadToMem(std::string url, CURL_builder* curl = nullptr);
}
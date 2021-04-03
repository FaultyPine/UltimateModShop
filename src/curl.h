#pragma once

#include "ums_utils.h"
#include <sys/select.h>
#include <curl/curl.h>

using json = nlohmann::json;

struct MemoryStruct
{
  char *memory;
  size_t size;

  ~MemoryStruct();
};

namespace curl {

    CURLcode DownloadFile(std::string url, std::string path);

    json DownloadJson(std::string url);

    MemoryStruct DownloadToMem(std::string url);
}
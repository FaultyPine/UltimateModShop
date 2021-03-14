#pragma once
#include <switch.h>
#include <filesystem>
#include <sys/select.h>
#include <curl/curl.h>

#include <iostream>
#include <cstring>
#include <vector>
#include <cmath>
#include <thread>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <time.h>
#include <filesystem>
#include <stdio.h>
#include "json.hpp"
#include <elzip/elzip.hpp>

#include "console.h"

using json = nlohmann::json;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

#define APP_VERSION "0.0.0"

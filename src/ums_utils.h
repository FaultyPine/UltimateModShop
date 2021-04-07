#pragma once

#include <borealis.hpp>

#include <filesystem>

#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <math.h>
#include <thread>
#include <chrono>
//#include <elzip/elzip.hpp>
#include "curl.h"

#define s(x) std::string(x)

#ifdef __SWITCH__
#include <switch.h>
#define SD_ROOT "sdmc:/"
#else
#define SD_ROOT "pc_tmp/"
#endif

using json = nlohmann::json;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

#define NO_GB_REQUESTS true

#define APP_VERSION "0.0.0"

#define SKYLINE_EXEFS_URL "https://github.com/skyline-dev/skyline/releases/download/beta/skyline.zip"

#define UMS_PATH s(SD_ROOT) + "switch/UltimateModShop/"
#define MODS_PATH s(SD_ROOT) + "ultimate/mods/"
#define SMASH_PATH s(SD_ROOT) + "atmosphere/contents/01006A800016E000/"
#define ROMFS_PATH s(SMASH_PATH) + "romfs/"
#define EXEFS_PATH s(SMASH_PATH) + "exefs/"
#define SKYLINE_PLUGINS_PATH s(ROMFS_PATH) + "skyline/plugins/"


void setup();

void stub();

std::string replaceAll(std::string str, const std::string &from, const std::string &to);

std::string EpochToHumanReadable(long long since_epoch);

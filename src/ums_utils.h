#pragma once

#include <borealis.hpp>

#include <filesystem>

#include <iostream>
#include <fstream>
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
#include "installed_json.h"

#define s(x) std::string(x)

#ifdef __SWITCH__
#include <switch.h>
#define SD_ROOT "sdmc:/"
#else
#define SD_ROOT "pc_tmp/"
#endif

using json = nlohmann::json;


// frwd declare and extern our InstalledJson object - since this is going to be used all over the project in many different senarios
class InstalledJson;
extern InstalledJson* installed_mods;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

#define NO_GB_REQUESTS false

#define APP_VERSION "0.0.0"

#define SKYLINE_EXEFS_URL "https://github.com/skyline-dev/skyline/releases/download/beta/skyline.zip"

#define UMS_PATH s(SD_ROOT) + "switch/UltimateModShop/"
#define UMS_INSTALLED_JSON_PATH s(UMS_PATH) + "installed_json.json"
#define MODS_PATH s(SD_ROOT) + "ultimate/mods/"
#define SMASH_PATH s(SD_ROOT) + "atmosphere/contents/01006A800016E000/"
#define ROMFS_PATH s(SMASH_PATH) + "romfs/"
#define EXEFS_PATH s(SMASH_PATH) + "exefs/"
#define SKYLINE_PLUGINS_PATH s(ROMFS_PATH) + "skyline/plugins/"

/// setup things that don't necessarily have to do with brls stuff
void setup();

void stub();

std::string replaceAll(std::string str, const std::string &from, const std::string &to);

std::string EpochToHumanReadable(long long since_epoch);

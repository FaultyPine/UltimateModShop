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
#include "zip/ZipUtil.hpp"

struct MemoryStruct
{
  char *memory;
  size_t size;
  MemoryStruct();
  ~MemoryStruct();
};

#include "curl.h"
#include "installed_mods.h"

#define stdstr(x) std::string(x)

#ifdef __SWITCH__
#include <switch.h>
#define SD_ROOT "sdmc:/"
#else
#define SD_ROOT "pc_tmp/"
#endif

using json = nlohmann::json;


// frwd declare and extern our InstalledMods object - since this is going to be used all over the project in many different senarios
class InstalledMods;
extern InstalledMods* installed_mods;

// The main parent of all views. Used for inter-view communication when they don't have parents set up, like in ctors (kinda icky, but is the only way I could think of)
extern brls::Box* main_box;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

#define REDUCED_NET_REQUESTS false

#define APP_VERSION "0.0.0"

#define SKYLINE_EXEFS_URL "https://github.com/skyline-dev/skyline/releases/download/beta/skyline.zip"

#define UMS_PATH stdstr(SD_ROOT) + "switch/UltimateModShop/"
#define UMS_INSTALLED_JSON_PATH stdstr(UMS_PATH) + "installed_json.json"
#define MODS_PATH stdstr(SD_ROOT) + "ultimate/mods/"
#define SMASH_PATH stdstr(SD_ROOT) + "atmosphere/contents/01006A800016E000/"
#define ROMFS_PATH stdstr(SMASH_PATH) + "romfs/"
#define EXEFS_PATH stdstr(SMASH_PATH) + "exefs/"
#define SKYLINE_PLUGINS_PATH stdstr(ROMFS_PATH) + "skyline/plugins/"

/// setup things that don't necessarily have to do with brls stuff
void setup();

void stub();

std::string replaceAll(std::string str, const std::string &from, const std::string &to);

std::string EpochToHumanReadable(long long since_epoch);

bool str_contains(std::string str, std::string substr);

std::vector<std::filesystem::path> jsonFileTreeToPaths(json j, std::vector<std::filesystem::path> paths = {});

void setHintText(std::string text = "");

void setMotdText(std::string text = "");
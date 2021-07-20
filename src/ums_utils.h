#pragma once

#include <borealis.hpp>
#include "ums_pch.h"
#include "zip/ZipUtil.hpp"

// a container for ptr + size  (does not free the mem)
struct MemoryStruct
{
  char *memory;
  size_t size;
  MemoryStruct();
};

#include "curl.h"
#include "installed_mods.h"
#include "gb.h"
#include "bg_task.h"

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
#define UMS_GITHUB_RELEASE_URL "https://github.com/FaultyPine/UltimateModShop/releases/latest/download/"
#define UMS_GITHUB_RELEASE_SW "UltimateModShop.nro"
#define UMS_GITHUB_RELEASE_PC "UltimateModShop"

#define stdstr(x) std::string(x)

#ifdef __SWITCH__
#include <switch.h>
#define SD_ROOT "sdmc:/"
#else
#define SD_ROOT "pc_tmp/"
#endif

// this icky
#define UMS_PATH                  stdstr(SD_ROOT)              + "switch/UltimateModShop/"
#define UMS_NRO_PATH              stdstr(UMS_PATH)             + "UltimateModShop.nro"
#define UMS_INSTALLED_JSON_PATH   stdstr(UMS_PATH)             + "installed_json.json"
#define ULTIMATE_ARC_PATH         stdstr(SD_ROOT)              + "ultimate/"
#define MODS_WORKSPACE_PATH       stdstr(ULTIMATE_ARC_PATH)    + "mods/"
#define SMASH_PATH                stdstr(SD_ROOT)              + "atmosphere/contents/01006A800016E000/"
#define ROMFS_PATH                stdstr(SMASH_PATH)           + "romfs/"
#define EXEFS_PATH                stdstr(SMASH_PATH)           + "exefs/"
#define SKYLINE_PLUGINS_PATH      stdstr(ROMFS_PATH)           + "skyline/plugins/"
#define SKYLINE_PLUGIN_DEP_PATH   stdstr(SKYLINE_PLUGINS_PATH) + "dependencies/"

#define TEMP_EXTRACTION_DIRNAME   "tmp_dir"
#define TEMP_EXTRACTION_DIR       stdstr(SD_ROOT)     + TEMP_EXTRACTION_DIRNAME + "/"

// TODO: namespace this stuff

/// setup things that don't necessarily have to do with brls stuff
void setup();

void stub();

float lerp(float a, float b, float f);

std::string replaceAll(std::string str, const std::string &from, const std::string &to);

std::string EpochToHumanReadable(long long since_epoch);

std::string readable_fs(double bytes);

bool str_contains(std::string data, std::string toSearch, size_t pos = 0);

bool strHasEnding (std::string const &fullString, std::string const &ending);

std::string cleanGBDescriptionText(const std::string& str);

void setHintText(std::string text = "", int seconds = 2);

void setTopText(std::string text = "");

void setBrlsImageAsync(std::string thumbnail_url, brls::Image* image);

// used to time entire functions
struct Timer {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<float> duration;
    std::string func;

    Timer(const std::string& _func = "") {
        start = std::chrono::high_resolution_clock::now();
        func = _func;
    }
    ~Timer() {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;

        float ms = duration.count() * 1000.0;
        if (func.empty())
            brls::Logger::debug("Timer took {} ms", ms);
        else
            brls::Logger::debug("Timer took {} ms in function: {}", ms, func);
    }
};
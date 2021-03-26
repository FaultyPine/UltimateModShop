#pragma once

#ifdef __SWITCH__
#include <switch.h>
#endif

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
#include "curl.h"

using json = nlohmann::json;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

#define APP_VERSION "0.0.0"


namespace gb {
    static constexpr char* GB_API_URL = "https://api.gamebanana.com/";
    static constexpr char* GB_SMASH_GAMEID = "6498";

    const std::vector<char*> AllowedItemTypes = {
        "Effect",
        "Gamefile",
        "Gui",
        "GuiCategory",
        "Map",
        "Model",
        "Project",
        "Skin",
        "Sound",
        "Texture",
    };

    struct GbSubmission {
        std::string name;
        std::string itemid;
        std::string itemtype;
    };

    std::vector<GbSubmission> GetNewSubmissions();

}

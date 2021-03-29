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
#include <time.h>
#include <regex>
#include "curl.h"

using json = nlohmann::json;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

static constexpr char* APP_VERSION = "0.0.0";


std::string replaceAll(std::string str, const std::string &from, const std::string &to);

std::string EpochToHumanReadable(ulong since_epoch);
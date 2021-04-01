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
#include <math.h>
#include "curl.h"

using json = nlohmann::json;

#define START_BREAKABLE do {
#define END_BREAKABLE   } while (false);

static const char APP_VERSION[] = "0.0.0";

void stub();

std::string replaceAll(std::string str, const std::string &from, const std::string &to);

std::string EpochToHumanReadable(long long since_epoch);

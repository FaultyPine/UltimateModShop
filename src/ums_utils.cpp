#include "ums_utils.h"

void stub() { }

std::string replaceAll(std::string str, const std::string &from, const std::string &to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string EpochToHumanReadable(long long since_epoch) {
    time_t rawtime = since_epoch;
    struct tm  ts;
    char       buf[80];

    // Format time, "ddd yyyy-mm-dd hh:mm:ss zzz"
    ts = *localtime(&rawtime);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    return std::string(buf);
}

// init for switch/pc
void setup() {
    /**
     * init paths
     * check for skyline (if not, install)
     */
    std::string paths[] = {
        SD_ROOT,
        UMS_PATH,
        MODS_PATH,
        SMASH_PATH,
        ROMFS_PATH,
        EXEFS_PATH,
        SKYLINE_PLUGINS_PATH,
    };
    for (std::string path : paths) {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }

    const std::string skyline_zip_name = "skyline.zip";
    if (!std::filesystem::exists(std::string(EXEFS_PATH) + "subsdk9") || !std::filesystem::exists(std::string(EXEFS_PATH) + "main.npdm")) {
        //brls::Logger::debug("Installing skyline...");
        //curl::DownloadFile(SKYLINE_EXEFS_URL, std::string(SMASH_PATH) + skyline_zip_name);
        // unzip
        //std::filesystem::remove(std::string(SMASH_PATH) + skyline_zip_name);
    }
}
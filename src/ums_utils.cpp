#include "ums_utils.h"

InstalledMods* installed_mods;

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

bool str_contains(std::string data, std::string toSearch, size_t pos)
{
    // Convert complete given String to lower case
    std::transform(data.begin(), data.end(), data.begin(), ::tolower);
    // Convert complete given Sub String to lower case
    std::transform(toSearch.begin(), toSearch.end(), toSearch.begin(), ::tolower);
    // Find sub string in given string
    return data.find(toSearch, pos) != std::string::npos;
}

std::vector<std::filesystem::path> jsonFileTreeToPaths(json j, std::vector<std::filesystem::path> paths) {
    if (j.is_object()) {
        for (auto& [key, value] : j.items()) {
            paths.push_back(key);
            jsonFileTreeToPaths(value, paths);
        }
    }
    else if (j.is_array()) {
        
    }
}

void setHintText(std::string text) {
    brls::Box* hint_box = (brls::Box*)main_box->getView("hint_box");
    if (hint_box != nullptr) {
        if (text.empty()) {
            hint_box->setVisibility(brls::Visibility::GONE);
        }
        else {
            hint_box->setVisibility(brls::Visibility::VISIBLE);
            ((brls::Label*)hint_box->getView("hint_text"))->setText(text);
        }
    }
}

void setMotdText(std::string text) {
    brls::Box* motd_box = (brls::Box*)main_box->getView("motd_box");
    if (motd_box != nullptr) {
        if (text.empty()) {
            motd_box->setVisibility(brls::Visibility::GONE);
        }
        else {
            motd_box->setVisibility(brls::Visibility::VISIBLE);
            ((brls::Label*)motd_box->getView("motd_text"))->setText(text);
        }
    }
}

bool strHasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
    return subject;
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
        MODS_WORKSPACE_PATH,
        SMASH_PATH,
        ROMFS_PATH,
        EXEFS_PATH,
        SKYLINE_PLUGINS_PATH,
        SKYLINE_PLUGIN_DEP_PATH,
    };
    for (std::string path : paths) {
        if (!std::filesystem::exists(path)) {
            std::filesystem::create_directories(path);
        }
    }

    const std::string skyline_zip_name = "skyline.zip";
    if (!std::filesystem::exists(std::string(EXEFS_PATH) + "subsdk9") || !std::filesystem::exists(std::string(EXEFS_PATH) + "main.npdm")) {
        brls::Logger::debug("Installing skyline...");
        std::string path = std::string(SMASH_PATH) + skyline_zip_name;
        curl::DownloadFile(SKYLINE_EXEFS_URL, path);
        UnZip::ArchiveExtract(path, SMASH_PATH);
        std::filesystem::remove(path);
    }

    installed_mods = new InstalledMods();
}
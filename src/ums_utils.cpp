#include "ums_utils.h"

InstalledMods* installed_mods;

void stub() { }

/// when f is 1, value is b, when f is 0, value is a
float lerp(float a, float b, float f) 
{
    return (a * (1.0 - f)) + (b * f);
}

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


std::string readable_fs(double bytes) {
    char buf[10]; // 10 should be fine
    int i = 0;
    const char* units[] = {"B", "kB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    while (bytes > 1024) {
        bytes /= 1024;
        i++;
    }
    sprintf(buf, "%.*f %s", i, bytes, units[i]);
    return std::string(buf);
}

void hint_text_wait(brls::Box* hint_box, int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
    hint_box->setVisibility(brls::Visibility::INVISIBLE);
}

void setHintText(std::string text, int seconds) {
    brls::Box* hint_box = (brls::Box*)main_box->getView("hint_box");
    if (hint_box != nullptr) {
        if (text.empty()) {
            hint_box->setVisibility(brls::Visibility::GONE);
        }
        else {
            hint_box->setVisibility(brls::Visibility::VISIBLE);
            ((brls::Label*)hint_box->getView("hint_text"))->setText(text);
            std::thread hint_thread([hint_box, seconds](){
                hint_text_wait(hint_box, seconds);
            });
            hint_thread.detach();
        }
    }
}

void setTopText(std::string text) {
    brls::Box* motd_box = (brls::Box*)main_box->getView("top_box");
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

// true when modpage is being closed/has closed. false when modpage is being created/is active
// ensures images don't try to load when modpage no longer exists (if user backs out of modpage before all images load in)
extern bool modpage_is_closing;

void brlsImageAsync(std::string thumbnail_url, brls::Image* image, bool is_modpage) {
    std::thread t([thumbnail_url, image, is_modpage]() {
        MemoryStruct s = curl::DownloadToMem(thumbnail_url);
        if (s.memory != nullptr && s.size > 0 && image) {
            BgTask::pushCallbackToQueue([s, image, is_modpage]() {
                if ((is_modpage && !modpage_is_closing) || !is_modpage) {
                    image->setImageFromMem((unsigned char*)s.memory, s.size);
                }
            });
        }
    }); t.detach();
}
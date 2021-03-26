#include "utils.h"

std::vector<gb::GbSubmission> gb::GetNewSubmissions() {
    std::stringstream url; url << gb::GB_API_URL << "Core/List/New?page=1&gameid=" << gb::GB_SMASH_GAMEID;
    json submissions = curl::DownloadJson(url.str());
    std::vector<gb::GbSubmission> ret;
    if (submissions != NULL) {
        for (auto entry : submissions) {
            gb::GbSubmission tmp;
            tmp.name = "Placeholder";
            tmp.itemtype = entry[0].get<std::string>();
            tmp.itemid = entry[1].get<int>();
            ret.push_back(tmp);
        }
    }
    return ret;
}
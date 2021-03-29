#include "gb.h"

std::string gb::ConcatFields(std::vector<gb::GbField> fields) {
    std::string ret = "";
    for (std::string s : fields) {
        ret.append(s + ",");
    }
    ret.pop_back();
    return ret;
}

gb::GbSubmissions gb::GetNewSubmissions(int page, std::string game) {
    brls::Logger::debug("Gettings new submissions for Smash...");
    std::stringstream url; 
    url << gb::GB_API_URL << gb::Endpoints::CoreListNew << "page=" << std::to_string(page) << "&gameid=" << game;
    json submissions = curl::DownloadJson(url.str());
    GbSubmissions ret = {};
    if (submissions != NULL) {
        for (auto entry : submissions) {
            std::string entry_itemtype = entry[0].get<std::string>();
            if (std::find(gb::AllowedItemTypes.begin(), gb::AllowedItemTypes.end(), entry_itemtype) != gb::AllowedItemTypes.end()) { // if the submission is in our list of "Allowed" itemtypes
                gb::GbSubmission* tmp = new gb::GbSubmission { .itemid = std::to_string(entry[1].get<int>()), .itemtype = entry_itemtype };
                ret.push_back(tmp);
            }
        }
    }
    return ret;
}

/// partial_submission must have itemtype and itemid filled in
/// appends data from 'fields' to partial_submission's 'submission_data' field
void gb::GetSubmissionData(gb::GbSubmission* partial_submission, std::vector<gb::GbField> fields) {
    if (partial_submission == nullptr)
        brls::Application::crash("Cannot get submission data from nullptr!");
    if (partial_submission->itemtype.empty() || partial_submission->itemid.empty()) {
        brls::Logger::error("Cannot get submission data without 'itemtype' or 'itemid'!");
        return;
    }
    std::string s_fields = gb::ConcatFields(fields);
    std::stringstream url;
    url << gb::GB_API_URL << gb::Endpoints::CoreItemData << 
        "itemtype=" << partial_submission->itemtype << 
        "&itemid=" << partial_submission->itemid << 
        "&fields=" << s_fields <<
        "&flags=" << gb::GB_UNESCAPED_SLASHES_FLAG;


    json _submission_data = curl::DownloadJson(url.str());
    partial_submission->submission_data += _submission_data;
}


void gb_test() {
    gb::GbSubmissions subs = gb::GetNewSubmissions(1);
    for (size_t i = 0; i < subs.size(); i++) {
        gb::GbSubmission* sub = subs.at(i);
        gb::GetSubmissionData(sub, {gb::Fields::Files, gb::Fields::Title});
        //printf(sub->submission_data.dump(4).c_str());
        //printf("\n____________\n");
    }
}
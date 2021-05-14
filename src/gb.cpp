#include "gb.h"

std::string gb::ConcatFields(std::vector<gb::GbField> fields) {
    std::string ret = "";
    for (std::string s : fields) {
        ret.append(s + ",");
    }
    if (ret.size() > 0)
        ret.pop_back();
    return ret;
}

gb::GbSubmissions gb::GetNewSubmissions(int page, std::string game) {
    brls::Logger::debug("Gettings new submissions...");
    std::stringstream url; 
    url << gb::GB_API_URL << gb::Endpoints::CoreListNew << "page=" << std::to_string(page) << "&gameid=" << game;
    
    // filter new submissions by only the AllowedItemTypes we want
    url << "&itemtype=";
    for (const char* allowed_type : gb::AllowedItemTypes) {
        url << allowed_type << ",";
    }
    std::string url_str = url.str();
    url_str.pop_back(); // remove last comma

    json submissions = curl::DownloadJson(url_str);
    GbSubmissions ret = {};
    if (!submissions.empty()) {
        for (auto entry : submissions) {
            // entry is ["itemtype", itemid]
            std::string entry_itemtype = entry[0].get<std::string>();
            gb::GbSubmission* tmp = new gb::GbSubmission { .itemid = std::to_string(entry[1].get<int>()), .itemtype = entry_itemtype };
             ret.push_back(tmp);
        }
    }
    return ret;
}

void gb::GetSubmissionData(gb::GbSubmission* partial_submission, std::vector<gb::GbField> fields) {
    if (partial_submission == nullptr || partial_submission->itemtype.empty() || partial_submission->itemid.empty() || fields.empty()) {
        brls::Application::crash("Invalid args to get submission data!");
        return;
    }
    std::string s_fields = gb::ConcatFields(fields);
    std::stringstream url;
    url << gb::GB_API_URL << gb::Endpoints::CoreItemData << 
        "itemtype=" << partial_submission->itemtype << 
        "&itemid=" << partial_submission->itemid << 
        "&fields=" << s_fields <<
        "&flags=" << gb::GB_UNESCAPED_SLASHES_FLAG <<
        "&return_keys=true";

    //brls::Logger::debug("Gb request: {}", url.str());
    json _submission_data = curl::DownloadJson(url.str());
    if (_submission_data == NULL) {
        brls::Logger::error("Submission data was NULL!");
        return;
    }
    partial_submission->submission_data += _submission_data;
}


void gb::GetSubmissionDataMulticall(gb::GbSubmissions* partial_submissions, const std::vector<gb::GbField>& fields) {
    if (partial_submissions->empty() || fields.empty()) {
        brls::Application::crash("Invalid args to get submissions data!");
        return;
    }
    std::string s_fields = gb::ConcatFields(fields);
    std::stringstream url;
    url << gb::GB_API_URL << gb::Endpoints::CoreItemData;

    for (int i = 0; i < partial_submissions->size(); i++) {
        gb::GbSubmission* sub = partial_submissions->at(i);
        std::string idx = std::to_string(i);
        url << "itemtype[" << idx << "]=" << sub->itemtype << "&" <<
        "itemid[" << idx << "]=" << sub->itemid << "&" <<
        "fields[" << idx << "]=" << s_fields + "&";
    }
    url << "flags=" << gb::GB_UNESCAPED_SLASHES_FLAG;
    url << "&return_keys=true";
    url << "&format=json_min";

    //brls::Logger::debug("Gb request: {}", url.str());
    json _submission_data = curl::DownloadJson(url.str());
    if (_submission_data.empty()) {
        brls::Logger::error("Submission data was empty!\nData: {}", _submission_data.dump(4));
        return;
    }
    //brls::Logger::debug(_submission_data.dump(4));

    for (int i = 0; i < partial_submissions->size(); i++) {
        partial_submissions->at(i)->submission_data = _submission_data[i];
    }
}

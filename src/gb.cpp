#include "gb.h"


std::string gb::Embeddables::ConstructEmbeddableImageUrl(std::string itemid, std::string variant, bool is_sound) {
    std::string url = gb::Embeddables::EmbeddableModBaseURL;
    if (is_sound) {
        gb::Embeddables::EmbeddableSoundBaseURL;
    }
    url += itemid + "_" + variant + ".jpg";
    return url;
}


gb::GbSubmissions* gb::GetNewSubmissions(int page, int numItemsPerPage, std::string game) {
    brls::Logger::debug("Gettings new submissions (page {})...", page);
    std::stringstream url;
    url << gb::NewSubsReq << gb::RequestArgs::NumPerPage << std::to_string(numItemsPerPage) << "&" << gb::RequestArgs::PageNum << std::to_string(page);

    json submissions = curl::DownloadJson(url.str());
    GbSubmissions* ret = new std::vector<gb::GbSubmission*>;
    if (!submissions.empty()) {
        for (auto entry : submissions[gb::Fields::Records]) {
            gb::GbSubmission* tmp = new gb::GbSubmission { .submission_data = entry };
            ret->push_back(tmp);
        }
    }
    return ret;
}

gb::GbSubmissions* gb::GetSubmissionsFromCategory(int page, int category, int numItemsPerPage, std::string game) {
    brls::Logger::debug("Gettings new submissions (page {} category {})...", page, category);
    std::stringstream url;
    url << gb::CategoryReq << std::to_string(category) << "&" << gb::RequestArgs::NumPerPage << std::to_string(numItemsPerPage) << "&" << gb::RequestArgs::PageNum << std::to_string(page);
    
    json submissions = curl::DownloadJson(url.str());
    GbSubmissions* ret = new std::vector<gb::GbSubmission*>;
    if (!submissions.empty()) {
        for (auto entry : submissions[gb::Fields::Records]) {
            gb::GbSubmission* tmp = new gb::GbSubmission { .submission_data = entry };
            ret->push_back(tmp);
        }
    }
    return ret;
}

std::string gb::getItemIdFromProfileURL(std::string profileURL) {
    return profileURL.substr(profileURL.rfind("/")+1);
}

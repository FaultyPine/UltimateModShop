#include "gb.h"


std::string gb::Embeddables::ConstructEmbeddableImageUrl(const std::string& itemid, const std::string& variant, bool is_sound) {
    std::string url = gb::Embeddables::EmbeddableModBaseURL;
    if (is_sound) {
        gb::Embeddables::EmbeddableSoundBaseURL;
    }
    url += itemid + "_" + variant + ".jpg";
    return url;
}

gb::GbSubmissions* createSubmissions(const std::string& url) {
    json submissions = curl::DownloadJson(url);
    gb::GbSubmissions* ret = new std::vector<gb::GbSubmission*>;
    if (!submissions.empty()) {
        const json& iterate = submissions.contains(gb::Fields::Records) ? submissions[gb::Fields::Records] : submissions;
        for (const auto& entry : iterate) {
            gb::GbSubmission* tmp = new gb::GbSubmission { .submission_data = entry };
            ret->push_back(tmp);
        }
    }
    return ret;
}


gb::GbSubmissions* gb::GetNewSubmissions(int page, int numItemsPerPage, const std::string& game) {
    std::stringstream url;
    url << gb::NewSubsReq << gb::RequestArgs::NumPerPage << std::to_string(numItemsPerPage) << "&" << gb::RequestArgs::PageNum << std::to_string(page) << "&" << gb::RequestArgs::GameIds << game;

    return createSubmissions(url.str());
}

gb::GbSubmissions* gb::GetSubmissionsFromCategory(int page, int category, int numItemsPerPage) {
    brls::Logger::debug("Gettings new submissions (page {} category {})...", page, category);
    std::stringstream url;
    url << gb::CategoryReq << std::to_string(category) << "&" << gb::RequestArgs::NumPerPage << std::to_string(numItemsPerPage) << "&" << gb::RequestArgs::PageNum << std::to_string(page);
    
    return createSubmissions(url.str());
}

gb::GbSubmissions* gb::GetSubmissionsFromSearch(int page, const std::string& search, int numItemsPerPage, const std::string& game) {
    brls::Logger::debug("Gettings submissions from search: (Search: {})...", search);
    std::stringstream url;
    // NOTE: Still need to filter this by game... api doesn't support that rn but tom is working on it      ----> --------------------------------------------------------------- here
    url << gb::SearchReq << search << "*&" << gb::RequestArgs::NumPerPage << std::to_string(numItemsPerPage) << "&" << gb::RequestArgs::PageNum << std::to_string(page) << /* doesnt do anything but hopefully will soon */ "&" << gb::RequestArgs::GameIds << game;
    
    return createSubmissions(url.str());
}


std::string gb::getItemIdFromProfileURL(const std::string& profileURL) {
    return profileURL.substr(profileURL.rfind("/")+1);
}

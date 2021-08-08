#include "gb.h"


std::string gb::Embeddables::ConstructEmbeddableImageUrl(const std::string& itemid, const std::string& variant, bool is_sound) {
    std::string url = gb::Embeddables::EmbeddableModBaseURL;
    if (is_sound) {
        gb::Embeddables::EmbeddableSoundBaseURL;
    }
    url += itemid + "_" + variant + ".jpg";
    return url;
}

void touchupJsonSubmission(json& entry) {
    // if the submission doesn't have an idRow field but does have a profileURL field, we can grab the id from there.
    if (entry.contains(gb::Fields::ProfileURL) && !entry.contains(gb::Fields::idRow)) {
        entry[gb::Fields::idRow] = gb::getItemIdFromProfileURL(entry[gb::Fields::ProfileURL].get<std::string>());
    }
    // construct our thumbnail url asap
    if (entry.contains(gb::Fields::PreviewMedia::PreviewMedia)) {
        std::string thumbnail_url = gb::Fields::PreviewMedia::BaseURL + entry[gb::Fields::PreviewMedia::PreviewMedia][0][gb::Fields::PreviewMedia::File220].get<std::string>();
        entry[gb::Fields::Custom::ThumbnailURL] = thumbnail_url;
    }
}

gb::GbSubmissions* createSubmissions(const std::string& url) {
    json submissions = curl::DownloadJson(url);
    gb::GbSubmissions* ret = new std::vector<gb::GbSubmission*>;
    if (!submissions.empty()) {
        json& iterate = submissions.contains(gb::Fields::Records) ? submissions[gb::Fields::Records] : submissions;
        for (auto& entry : iterate) {
            touchupJsonSubmission(entry);
            gb::GbSubmission* tmp = new gb::GbSubmission { .submission_data = entry };
            ret->push_back(tmp);
        }
    }
    return ret;
}

gb::GbSubmission* gb::GetSingleSubmission(std::string itemid) {
    json item_json = curl::DownloadJson("https://gamebanana.com/apiv3/Mod/" + itemid);
    touchupJsonSubmission(item_json);
    return new gb::GbSubmission { .submission_data = item_json };
}

gb::GbSubmissions* gb::GetNewSubmissions(int page, int numItemsPerPage, const std::string& game) {
    brls::Logger::debug("Gettings new submissions (page {})...", page);
    std::stringstream url;
    url << gb::NewSubsReq << gb::RequestArgs::NumPerPage << std::to_string(numItemsPerPage) << "&" << gb::RequestArgs::PageNum << std::to_string(page) << "&" << gb::RequestArgs::GameIds << game;

    return createSubmissions(url.str());
}

gb::GbSubmissions* gb::GetSubmissionsFromCategory(int page, int category, int numItemsPerPage) {
    brls::Logger::debug("Gettings submissions from category (page {} category {})...", page, category);
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

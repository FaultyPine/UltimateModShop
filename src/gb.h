#pragma once

#include "ums_utils.h"

namespace gb {
    static const char GB_API_URL[] = "https://api.gamebanana.com/";
    static const char GB_APIV3_URL[] = "https://gamebanana.com/apiv3/";
    static const char GB_SMASH_GAMEID[] = "6498";
    static const char GB_UNESCAPED_SLASHES_FLAG[] = "JSON_UNESCAPED_SLASHES";
    static const char GB_NO_THUMBNAIL_URL[] = "https://images.gamebanana.com//";

    namespace Endpoints {
        static const char CoreListNew[] = "Core/List/New?";
        static const char CoreItemData[] = "Core/Item/Data?";
    }

    const std::vector<const char*> AllowedItemTypes = {
        "Effect",
        "Gamefile",
        "Gui",
        "Map",
        "Model",
        "Project",
        "Skin",
        "Sound",
        "Texture",
        "Mod",
    };

    struct GbSubmission {
        std::string itemid;
        std::string itemtype;
        json submission_data;
    };

    typedef std::vector<GbSubmission*> GbSubmissions;
    typedef const char* GbField;

    namespace Fields {
        static GbField Files = "Files().aFiles()";
        static GbField Title = "name";
        static GbField Author = "Owner().name";
        static GbField Thumbnail = "Preview().sSubFeedImageUrl()";
        static GbField UploadDate = "date";
        static GbField NumUpdates = "Updates().nGetUpdatesCount()";
    }

    /// Returns a vector of GbSubmission pointers of the newest submissions of the specified gameid.
    /// Those GbSubmission's will only have itemid and itemtype populated. To get actual info about the submission, use gb::GetSubmissionData
    GbSubmissions GetNewSubmissions(int page, std::string game = GB_SMASH_GAMEID);

    /// partial_submission must have itemtype and itemid filled in
    /// appends data from 'fields' to partial_submission's 'submission_data' field
    void GetSubmissionData(gb::GbSubmission* partial_submission, std::vector<gb::GbField> fields = {});

    /// Uses multicalls to get submission data on a vector of itemid/itemtypes. Reduces api requests this way
    void GetSubmissionDataMulticall(gb::GbSubmissions* partial_submissions, const std::vector<gb::GbField>& fields);
    
    /// Takes list of GbField's and turns it into a single string to be passed into the gb api
    std::string ConcatFields(std::vector<GbField> fields);

}
#pragma once

#include "utils.h"

namespace gb {
    static const char GB_API_URL[] = "https://api.gamebanana.com/";
    static const char GB_SMASH_GAMEID[] = "6498";
    static const char GB_UNESCAPED_SLASHES_FLAG[] = "JSON_UNESCAPED_SLASHES";
    static const int NUM_SUBMISSIONS_PER_PAGE = 9;

    namespace Endpoints {
        static const char CoreListNew[] = "Core/List/New?";
        static const char CoreItemData[] = "Core/Item/Data?";
    }

    const std::vector<const char*> AllowedItemTypes = {
        "Effect",
        "Gamefile",
        "Gui",
        "GuiCategory",
        "Map",
        "Model",
        "Project",
        "Skin",
        "Sound",
        "Texture",
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
    }

    /// Returns a vector of GbSubmission pointers of the newest submissions of the specified gameid.
    /// Those GbSubmission's will only have itemid and itemtype populated. To get actual info about the submission, use gb::GetSubmissionData
    GbSubmissions GetNewSubmissions(int page, std::string game = GB_SMASH_GAMEID);

    /// partial_submission must have itemtype and itemid filled in
    /// appends data from 'fields' to partial_submission's 'submission_data' field
    void GetSubmissionData(gb::GbSubmission* partial_submission, std::vector<gb::GbField> fields);

    /// Takes list of GbField's and turns it into a single string to be passed into the gb api
    std::string ConcatFields(std::vector<GbField> fields);

}



void gb_test();
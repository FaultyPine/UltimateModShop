#pragma once

#include "utils.h"

namespace gb {
    static constexpr char* GB_API_URL = "https://api.gamebanana.com/";
    static constexpr char* GB_SMASH_GAMEID = "6498";
    static constexpr char* GB_UNESCAPED_SLASHES_FLAG = "JSON_UNESCAPED_SLASHES";
    static int NUM_SUBMISSIONS_PER_PAGE = 9;

    namespace Endpoints {
        static constexpr char* CoreListNew = "Core/List/New?";
        static constexpr char* CoreItemData = "Core/Item/Data?";
    }

    const std::vector<char*> AllowedItemTypes = {
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

    GbSubmissions GetNewSubmissions(int page, std::string game = GB_SMASH_GAMEID);
    void GetSubmissionData(gb::GbSubmission* partial_submission, std::vector<gb::GbField> fields);
    std::string ConcatFields(std::vector<GbField> fields);

}



void gb_test();
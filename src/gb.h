#pragma once

#include "ums_utils.h"


namespace gb {
    static const char GB_APIV3_URL[] = "https://gamebanana.com/apiv3/";
    static const char GB_SMASH_GAMEID[] = "6498";
    static const char NewSubsReq[] = 
        "https://gamebanana.com/apiv3/Mod/ByGame?_sRecordSchema=FileDaddy&_bReturnMetadata=true&";
    static const char CategoryReq[] = 
        "https://gamebanana.com/apiv3/Mod/ByCategory?_sRecordSchema=FileDaddy&_bReturnMetadata=true&_aCategoryRowIds[]=";
    static const char SearchReq[] = 
        "https://gamebanana.com/apiv6/Mod/ByName?_bReturnMetadata=true&_csvProperties=_idRow,_sName,_aFiles,_aPreviewMedia,_sText,_nViewCount,_nLikeCount,_nDownloadCount,_aSubmitter&_sName=*";

    namespace Endpoints {
        static const char ModByGame[] = "Mod/ByGame?";
        static const char Index[] = "Index?";
        static const char ByCategory[] = "ByCategory?";
    }
    namespace RequestArgs {
        static const char GameIds[] = "_aGameRowIds[]=";
        static const char GameID[] = "_idGameRow=";
        static const char RecordSchema[] = "_sRecordSchema=";
        static const char PageNum[] = "_nPage=";
        static const char NumPerPage[] = "_nPerpage=";
    }

    struct GbSubmission {
        json submission_data;
    };


    typedef const char* GbField;
    namespace Fields {
        static GbField Name = "_sName";
        static GbField DateAdded = "_tsDateAdded";
        static GbField DateUpdated = "_tsDateUpdated";
        static GbField idRow = "_idRow";
        static GbField ViewCount = "_nViewCount";
        static GbField DownloadCount = "_nDownloadCount";
        static GbField LikeCount = "_nLikeCount";
        static GbField IconURL = "_sIconUrl";
        static GbField Text = "_sText";
        static GbField Subtitle = "_sDescription";
        static GbField Records = "_aRecords";
        static GbField ProfileURL = "_sProfileUrl";
        static GbField Type = "_sType";

        namespace Metadata {
            static GbField Metadata = "_aMetadata";
            static GbField PageCount = "_nPageCount";
        }
        namespace PreviewMedia {
            static GbField PreviewMedia = "_aPreviewMedia";
            static GbField BaseURL = "https://images.gamebanana.com/img/ss/mods/";
            static GbField File220 = "_sFile220";
            static GbField File530 = "_sFile530";
            static GbField File100 = "_sFile100";
            static GbField Caption = "_sCaption";
        }
        namespace Files {
            static GbField Files = "_aFiles";
            static GbField FileName = "_sFile";
            static GbField FileSize = "_nFilesize"; // in bytes
            static GbField DownloadURL = "_sDownloadUrl";
        }
        namespace Submitter {
            static GbField Submitter = "_aSubmitter";
            static GbField AvatarURL = "_sAvatarUrl";
            // Name
        }
        namespace Category {
            static GbField SubCategory = "_aCategory";
            static GbField RootCategory = "_aRootCategory";
            static GbField ParentCategory = "_idParentCategoryRow";
            // Name
        }
        namespace AdditionalInfo {
            static GbField AdditionalInfo = "_aAdditionalInfo";
            static GbField Version = "_sVersion";
        }
        // not real fields from gb, for internal use
        namespace Custom {
            static GbField ThumbnailURL = "thumbnail_url";
            static GbField Paths = "paths";
            static GbField Enabled = "is_enabled";
        }
    }

    namespace Embeddables {
        static GbField EmbeddableModBaseURL = "https://images.gamebanana.com/img/embeddables/Mod_";
        static GbField EmbeddableSoundBaseURL = "https://images.gamebanana.com/img/embeddables/Sound_";
        // something like:
        //  https://images.gamebanana.com/img/embeddables/Mod_288826_medium_square.jpg    <- for Mod's
        std::string ConstructEmbeddableImageUrl(const std::string& itemid, const std::string& variant, bool is_sound);
    }

    std::string cleanGBDescriptionText(const std::string& str);

    // return itemid from the url of the mod page. Using this since idRow isn't included in the FileDaddy recordschema
    std::string getItemIdFromProfileURL(const std::string& profileURL);

    typedef std::vector<GbSubmission*> GbSubmissions;

    GbSubmission* GetSingleSubmission(std::string itemid);

    /// Returns a vector of GbSubmission pointers of the newest submissions of the specified gameid.
    GbSubmissions* GetNewSubmissions(int page, int numItemsPerPage, const std::string& game = GB_SMASH_GAMEID);

    GbSubmissions* GetSubmissionsFromCategory(int page, int category, int numItemsPerPage);

    GbSubmissions* GetSubmissionsFromSearch(int page, const std::string& search, int numItemsPerPage, const std::string& game = GB_SMASH_GAMEID);
}
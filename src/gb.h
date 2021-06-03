#pragma once

#include "ums_utils.h"


namespace gb {
    static const char GB_APIV3_URL[] = "https://gamebanana.com/apiv3/";
    static const char GB_SMASH_GAMEID[] = "6498";
    static const char NewSubsReq[] = "https://gamebanana.com/apiv3/Mod/ByGame?_aGameRowIds[]=6498&_sRecordSchema=FileDaddy&_bReturnMetadata=true&";

    namespace Endpoints {
        static const char ModByGame[] = "Mod/ByGame?";
        static const char Index[] = "Index?";
        static const char ByCategory[] = "ByCategory?";
    }
    namespace RequestArgs {
        static const char GameIds[] = "_aGameRowIds[]=";
        static const char RecordSchema[] = "_sRecordSchema=";
        static const char PageNum[] = "_nPage=";
        static const char NumPerPage[] = "_nPerpage=";
    }

    struct GbSubmission {
        //std::string itemid;
        //std::string itemtype;
        json submission_data;
    };


    typedef const char* GbField;
    namespace Fields {
        static GbField Name = "_sName";
        static GbField DateAdded = "_tsDateAdded";
        static GbField idRow = "_idRow";
        static GbField ViewCount = "_nViewCount";
        static GbField DownloadCount = "_nDownloadCount";
        static GbField LikeCount = "_nLikeCount";
        static GbField IconURL = "_sIconUrl";
        static GbField Text = "_sText";
        static GbField Description = "_sDescription";
        static GbField Records = "_aRecords";
        static GbField ProfileURL = "_sProfileUrl";

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
            static GbField Category = "_aCategory";
            static GbField RootCategory = "_aRootCategory";
            // Name
        }
        // not real fields from gb, for internal use
        namespace Custom {
            static GbField ThumbnailURL = "thumbnail_url";
            static GbField Paths = "paths";
        }
    }

    namespace Embeddables {
        static GbField EmbeddableModBaseURL = "https://images.gamebanana.com/img/embeddables/Mod_";
        static GbField EmbeddableSoundBaseURL = "https://images.gamebanana.com/img/embeddables/Sound_";
        // something like:
        //  https://images.gamebanana.com/img/embeddables/Mod_288826_medium_square.jpg    <- for Mod's
        std::string ConstructEmbeddableImageUrl(std::string itemid, std::string variant, bool is_sound);
    }

    // return itemid from the url of the mod page. Using this since idRow isn't included in the FileDaddy recordschema
    std::string getItemIdFromProfileURL(std::string profileURL);

    typedef std::vector<GbSubmission*> GbSubmissions;
    /// Returns a vector of GbSubmission pointers of the newest submissions of the specified gameid.
    /// Those GbSubmission's will only have itemid and itemtype populated. To get actual info about the submission, use gb::GetSubmissionData
    GbSubmissions* GetNewSubmissions(int page, int numItemsPerPage, std::string game = GB_SMASH_GAMEID);
}
#include "mod_page.h"

ModPage::ModPage(SubmissionNode* sub) {
    this->submission = sub;
    this->v = brls::View::createFromXMLResource("tabs/mod_page/mod_page.xml");
}

const std::string imgTemplate = R"xml(
    <brls:Image 
        scalingType="fit"
    />
)xml";

#define GetChildView(type, id) brls::type* id = (brls::type*)this->getView(#id);

void ModPage::setupModPage() {
    if (this->submission != nullptr) {

        const json& mod = this->submission->getSubmissionData()->submission_data[gb::Fields::Records][0];
        if (mod.empty())
            return;

        /* ---------------- Left Side Prep ---------------- */
        GetChildView(Label, title_label)
        GetChildView(Label, subtitle_label)
        GetChildView(Box, screenshots_box)
        GetChildView(Image, screenshots_default_image)
        GetChildView(Label, image_caption_label)
        GetChildView(Box, scroll_dot_box)
        GetChildView(Image, scroll_dot)
        GetChildView(Label, description_label)

        /* ---------------- Right Side Prep ---------------- */
        GetChildView(Label, author_label)
        GetChildView(Image, author_image)
        GetChildView(Label, version_label)
        GetChildView(Label, date_label)
        GetChildView(Label, dlsize_label)
        GetChildView(Label, download_count_label)
        GetChildView(Label, view_count_label)
        GetChildView(Label, like_count_label)
        GetChildView(Label, category_label)
        GetChildView(Image, category_image)
        GetChildView(Label, subcategory_label)
        GetChildView(Image, subcategory_image)

        /* ---------------- Left Side Setting ---------------- */

        /* Title */
        title_label->setText(mod[gb::Fields::Name].get<std::string>());

        /* Subtitle */
        std::string subtitle_txt = mod[gb::Fields::Subtitle].get<std::string>();
        if (!subtitle_txt.empty())
            subtitle_label->setText(subtitle_txt);
        else
            subtitle_label->setVisibility(brls::Visibility::INVISIBLE);

        /* Screenshots */
        brls::LayerView* screenshots_layers = new brls::LayerView();
        screenshots_box->addView(screenshots_layers);

        json PreviewMedia = mod[gb::Fields::PreviewMedia::PreviewMedia];
        if (PreviewMedia.size() > 0)
            screenshots_default_image->setVisibility(brls::Visibility::GONE);

        for (json img_json : PreviewMedia) {
            // doesn't support videos/gifs and stuff
            if (img_json[gb::Fields::Type].get<std::string>() == "image") {
                std::string img_url = gb::Fields::PreviewMedia::BaseURL + img_json[gb::Fields::PreviewMedia::File530].get<std::string>();
                MemoryStruct img = curl::DownloadToMem(img_url);
                if (img.memory != nullptr && img.size > 0) {
                    brls::Image* new_img = (brls::Image*)brls::View::createFromXMLString(imgTemplate);
                    new_img->setImageFromMem((unsigned char*)img.memory, img.size);
                    screenshots_layers->addLayer(new_img);
                }
            }
        }

        /* Image Captions */
        

        /* Scroll Dots */

        /* Description */
        std::string description_full_text = mod[gb::Fields::Text].get<std::string>();
        description_label->setText(cleanGBDescriptionText(description_full_text));

        /* ---------------- Right Side Setting ---------------- */

        /* Author */
        // gb usernames are 30 chars max. fontSize of 16 fits that - scale the size based on num of characters
        std::string author_name = mod[gb::Fields::Submitter::Submitter][gb::Fields::Name].get<std::string>();
        if (author_name.size() >= 20) // 22 chars or more makes the label start to go off the edge... (should probably lerp this tbh)
            author_label->setFontSize(16);
        author_label->setText(author_name);
        std::string author_image_url = mod[gb::Fields::Submitter::Submitter][gb::Fields::Submitter::AvatarURL].get<std::string>();
        MemoryStruct author_img_mem = curl::DownloadToMem(author_image_url);
        if (author_img_mem.memory != nullptr && author_img_mem.size > 0)
            author_image->setImageFromMem((unsigned char*)author_img_mem.memory, author_img_mem.size);

        /* Version */
        if (mod.contains(gb::Fields::AdditionalInfo::AdditionalInfo))
            version_label->setText(mod[gb::Fields::AdditionalInfo::Version].get<std::string>());

        /* Date */
        std::string full_date = EpochToHumanReadable(mod[gb::Fields::DateAdded].get<size_t>());
        int first_space_idx = full_date.find(" ");
        std::string date = full_date.substr(first_space_idx, first_space_idx+8); 
        date_label->setText("Date Uploaded: " + date);

        /* Download Size */
        dlsize_label->setText("Download Size: " + std::to_string(mod[gb::Fields::Files::Files][0][gb::Fields::Files::FileSize].get<float>()/1000/1000) + " MB");

        /* Download Count */
        download_count_label->setText(std::to_string(mod[gb::Fields::DownloadCount].get<size_t>()));

        /* View Count */
        view_count_label->setText(std::to_string(mod[gb::Fields::ViewCount].get<size_t>()));

        /* Like Count */
        like_count_label->setText(std::to_string(mod[gb::Fields::LikeCount].get<size_t>()));

        /* Category */
        category_label->setText(mod[gb::Fields::Category::RootCategory][gb::Fields::Name].get<std::string>());
        std::string category_img_url = mod[gb::Fields::Category::RootCategory][gb::Fields::IconURL].get<std::string>();
        MemoryStruct category_img = curl::DownloadToMem(category_img_url);
        if (category_img.memory != nullptr && category_img.size > 0)
            category_image->setImageFromMem((unsigned char*)category_img.memory, category_img.size);

        /* SubCategory */
        subcategory_label->setText(mod[gb::Fields::Category::SubCategory][gb::Fields::Name].get<std::string>());
        std::string subcategory_img_url = mod[gb::Fields::Category::SubCategory][gb::Fields::IconURL].get<std::string>();
        MemoryStruct subcategory_img = curl::DownloadToMem(subcategory_img_url);
        if (subcategory_img.memory != nullptr && subcategory_img.size > 0)
            subcategory_image->setImageFromMem((unsigned char*)subcategory_img.memory, subcategory_img.size);


    }
}

void ModPage::onContentAvailable() {
    this->registerAction(
        "Close", brls::ControllerButton::BUTTON_B, [](brls::View* view) {
            brls::Application::popActivity();
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_CHANGE);
    this->setupModPage();
}
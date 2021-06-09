#include "mod_page.h"
#include "../../installation/manager.h"


const std::string screenshotTemplate = R"xml(
    <brls:Image 
        scalingType="fit"
        imageAlign="center"
    />
)xml";
const std::string scrollDotXML = R"xml(
    <brls:Image
        id="scroll_dot"
        image="@res/icon/dot.png"
        scalingType="fit"
        marginRight="2"
        marginLeft="7"
        alignSelf="center"
        width="2%"
        height="55%"
        maxHeight="20"
        maxWidth="20"
        cornerRadius="25"
        borderColor="#FFFFFF"
    />
)xml";
const std::string scrollDotHorzHolder = R"xml(
    <brls:Box
        id="scroll_dot_horz_holder"
        axis="row"
        justifyContent="spaceEvenly"
        width="100%"
    />
)xml";

void setScrollDotSelected(brls::Image* dot) {
    dot->setBorderThickness(8);
}
void setScrollDotUnselected(brls::Image* dot) {
    dot->setBorderThickness(0);
}

ModPage::ModPage(SubmissionNode* sub) {
    this->screenshots_layers = new brls::LayerView();
    this->submission = sub;
    this->v = brls::View::createFromXMLResource("tabs/mod_page/mod_page.xml");
}
ModPage::~ModPage() {
    if (!this->medias.empty()) {
        for (PreviewMediaContainer* p : this->medias) {
            delete p;
        }
    }
}

#define GetChildView(type, id) brls::type* id = (brls::type*)this->getView(#id);

void ModPage::setupModPage() {
    if (this->submission != nullptr && this->submission->getSubmissionData() != nullptr) {
        
        const json& mod = this->submission->getSubmissionData()->submission_data;
        if (mod.empty())
            return;

        if (mod.contains(gb::Fields::idRow))
            brls::Logger::debug("Setting up ModPage... Itemid: {}", mod[gb::Fields::idRow].get<std::string>());
        else
            brls::Logger::debug("Setting up ModPage...");

        /* ---------------- Left Side Prep ---------------- */
        GetChildView(Label, title_label)
        GetChildView(Label, subtitle_label)
        GetChildView(Box, screenshots_box)
        GetChildView(Image, screenshots_default_image)
        GetChildView(Label, image_caption_label)
        GetChildView(Box, scroll_dot_box)
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
        if (mod.contains(gb::Fields::Subtitle)) {
            std::string subtitle_txt = mod[gb::Fields::Subtitle].get<std::string>();
            if (!subtitle_txt.empty()) {
                subtitle_label->setText(subtitle_txt);
                subtitle_label->setIsWrapping(true);
                subtitle_label->setVisibility(brls::Visibility::VISIBLE);
            }
            else
                subtitle_label->setVisibility(brls::Visibility::INVISIBLE);
        }


        /* Screenshots */
        screenshots_box->addView(this->screenshots_layers);

        json PreviewMedia = mod[gb::Fields::PreviewMedia::PreviewMedia];
        if (PreviewMedia.size() > 0)
            screenshots_default_image->setVisibility(brls::Visibility::GONE);


        for (json img_json : PreviewMedia) {
            // doesn't support videos/gifs and stuff
            if (img_json[gb::Fields::Type].get<std::string>() == "image") {
                std::string img_url = gb::Fields::PreviewMedia::BaseURL + img_json[gb::Fields::Files::FileName].get<std::string>();
                MemoryStruct img = curl::DownloadToMem(img_url);
                if (img.memory != nullptr && img.size > 0) {
                    brls::Image* new_img = (brls::Image*)brls::View::createFromXMLString(screenshotTemplate);
                    new_img->setImageFromMem((unsigned char*)img.memory, img.size);
                    this->screenshots_layers->addLayer(new_img);

                    std::string caption = img_json.contains(gb::Fields::PreviewMedia::Caption) ? img_json[gb::Fields::PreviewMedia::Caption].get<std::string>() : "";
                    this->medias.push_back(new PreviewMediaContainer({ .img = new_img, .caption = caption}));
                }
            }
        }

        /* Image Caption(s) */
        if (!this->medias.empty() && !this->medias.at(0)->caption.empty()) {
            image_caption_label->setText(this->medias.at(0)->caption);
        }
        else
            image_caption_label->setVisibility(brls::Visibility::GONE);

        /* Scroll Dots */
        int num_dots_iter = 0;
        int num_row_iterations = ceil( (float)this->medias.size() / (float)num_screenshot_dots_per_row );
        for (int i = 0; i < num_row_iterations; i++) {
            brls::Box* scroll_dot_horz_holder = (brls::Box*)brls::View::createFromXMLString(scrollDotHorzHolder);
            for (int j = 0; j < num_screenshot_dots_per_row; j++) {
                if (num_dots_iter >= this->medias.size()) {
                    i = num_row_iterations; 
                    break;
                }
                brls::Image* scroll_dot = (brls::Image*)brls::View::createFromXMLString(scrollDotXML);
                if (i == 0 && j == 0)
                    setScrollDotSelected(scroll_dot);
                scroll_dot_horz_holder->addView(scroll_dot);
                num_dots_iter += 1;
            }
            scroll_dot_box->addView(scroll_dot_horz_holder);
        }

        /* Description */
        if (mod.contains(gb::Fields::Text)) {
            std::string description_full_text = mod[gb::Fields::Text].get<std::string>();
            description_label->setIsWrapping(true);
            description_label->setGrow(1.0);
            description_label->setText(cleanGBDescriptionText(description_full_text));
            description_label->setVisibility(brls::Visibility::VISIBLE);
        }


        /* ---------------- Right Side Setting ---------------- */

        /* Author */
        std::string author_name = mod[gb::Fields::Submitter::Submitter][gb::Fields::Name].get<std::string>();
        float author_text_size = lerp(25.0, 16.0, author_name.size() / 30.0); // gb usernames are 30 chars max. fontSize of 16 fits max chars
        author_label->setFontSize(author_text_size);
        author_label->setText(author_name);
        std::string author_image_url = mod[gb::Fields::Submitter::Submitter][gb::Fields::Submitter::AvatarURL].get<std::string>();
        MemoryStruct author_img_mem = curl::DownloadToMem(author_image_url);
        if (author_img_mem.memory != nullptr && author_img_mem.size > 0)
            author_image->setImageFromMem((unsigned char*)author_img_mem.memory, author_img_mem.size);

        /* Version */
        if (mod.contains(gb::Fields::AdditionalInfo::AdditionalInfo)) {
            std::string version = mod[gb::Fields::AdditionalInfo::AdditionalInfo][gb::Fields::AdditionalInfo::Version].get<std::string>();
            if (version.empty())
                version = "Version: 0.0.0";
            version_label->setText(version);
        }

        /* Date */
        std::string full_date = EpochToHumanReadable(mod[gb::Fields::DateAdded].get<size_t>());
        int first_space_idx = full_date.find(" ");
        std::string date = full_date.substr(first_space_idx, first_space_idx+8); 
        date_label->setText("Date Uploaded: " + date);

        /* Download Size */
        std::string download_size = readable_fs(mod[gb::Fields::Files::Files][0][gb::Fields::Files::FileSize].get<float>());
        dlsize_label->setText("Download Size: " + download_size);

        /* Download Count */
        download_count_label->setText(std::to_string(mod[gb::Fields::DownloadCount].get<size_t>()));

        /* View Count */
        view_count_label->setText(std::to_string(mod[gb::Fields::ViewCount].get<size_t>()));

        /* Like Count */
        like_count_label->setText(std::to_string(mod[gb::Fields::LikeCount].get<size_t>()));

        /* Category */
        if (mod.contains(gb::Fields::Category::RootCategory)) {
            std::string category_text = mod[gb::Fields::Category::RootCategory][gb::Fields::Name].get<std::string>();
            float category_text_size = lerp(24.0, 18.0, category_text.size() / 12.0);
            category_label->setFontSize(category_text_size);
            category_label->setIsWrapping(true);
            category_label->setText(category_text);
            std::string category_img_url = mod[gb::Fields::Category::RootCategory][gb::Fields::IconURL].get<std::string>();
            if (!category_img_url.empty()) {
                MemoryStruct category_img = curl::DownloadToMem(category_img_url);
                if (category_img.memory != nullptr && category_img.size > 0)
                    category_image->setImageFromMem((unsigned char*)category_img.memory, category_img.size);
            }
            else
                category_image->setVisibility(brls::Visibility::GONE);
        }

        /* SubCategory */
        if (mod.contains(gb::Fields::Category::SubCategory)) {
            std::string subcategory_txt = mod[gb::Fields::Category::SubCategory][gb::Fields::Name].get<std::string>();
            float subcategory_text_size = lerp(24.0, 14.0, subcategory_txt.size() / 16.0); // max chars is 23 but i need it smaller to better suit the more common ones
            subcategory_label->setFontSize(subcategory_text_size);
            subcategory_label->setIsWrapping(true);
            subcategory_label->setText(subcategory_txt);
            std::string subcategory_img_url = mod[gb::Fields::Category::SubCategory][gb::Fields::IconURL].get<std::string>();
            if (!subcategory_img_url.empty()) {
                MemoryStruct subcategory_img = curl::DownloadToMem(subcategory_img_url);
                if (subcategory_img.memory != nullptr && subcategory_img.size > 0)
                    subcategory_image->setImageFromMem((unsigned char*)subcategory_img.memory, subcategory_img.size);
            }
            else
                subcategory_image->setVisibility(brls::Visibility::GONE);
        }

    }
}

void ModPage::screenshotsScroll(brls::FocusDirection dir) {
    GetChildView(Box, scroll_dot_box)

    {   // prev scroll dot
        int row = this->screenshot_idx / 10;
        brls::Box* scroll_dot_row_box = (brls::Box*)scroll_dot_box->getChildren().at(row);
        setScrollDotUnselected((brls::Image*)scroll_dot_row_box->getChildren().at(this->screenshot_idx % 10));
    }


    int num_preview_medias = this->medias.size();
    switch (dir) {
        case brls::FocusDirection::RIGHT:
            this->screenshot_idx = this->screenshot_idx+1 >= num_preview_medias ? 0 : this->screenshot_idx + 1;
            break;
        case brls::FocusDirection::LEFT:
            this->screenshot_idx = this->screenshot_idx-1 < 0 ? num_preview_medias-1 : this->screenshot_idx - 1;
            break;
        default:
            brls::Logger::error("Cannot scroll screenshots up/down!");
    }

    this->screenshots_layers->changeLayer(this->screenshot_idx);

    GetChildView(Label, image_caption_label)
    image_caption_label->setText(this->medias.at(this->screenshot_idx)->caption);

    if (!this->medias.empty() && !this->medias.at(this->screenshot_idx)->caption.empty()) {
        image_caption_label->setVisibility(brls::Visibility::VISIBLE);
        image_caption_label->setText(this->medias.at(this->screenshot_idx)->caption);
    }
    else
        image_caption_label->setVisibility(brls::Visibility::GONE);


    {
        int row = this->screenshot_idx / 10;
        brls::Box* scroll_dot_row_box = (brls::Box*)scroll_dot_box->getChildren().at(row);
        setScrollDotSelected((brls::Image*)scroll_dot_row_box->getChildren().at(this->screenshot_idx % 10));
    }
}

void ModPage::onContentAvailable() {
    this->setupModPage();

    this->registerAction(
        "Close", brls::ControllerButton::BUTTON_B, [](brls::View* view) {
            brls::Application::popActivity();
            brls::Application::giveFocus(main_box->getView("main_window_box"));
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_CHANGE);

    this->registerAction(
        "Install", brls::ControllerButton::BUTTON_X, [this](brls::View* view) {
            this->submission->downloadSubmission();
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_CHANGE);

    this->registerAction(
        "Scroll Right", brls::ControllerButton::BUTTON_RB, [this](brls::View* view) {
            this->screenshotsScroll(brls::FocusDirection::RIGHT);
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_CHANGE);

    this->registerAction(
        "Scroll Left", brls::ControllerButton::BUTTON_LB, [this](brls::View* view) {
            this->screenshotsScroll(brls::FocusDirection::LEFT);
            return true;
        },
        false, brls::Sound::SOUND_FOCUS_CHANGE);

    brls::Application::giveFocus(this->getView("install_box"));
}


bool ModPage::onInstallButtonClicked(brls::View* v) {
    brls::Logger::debug("install clicked");
    return true;
}

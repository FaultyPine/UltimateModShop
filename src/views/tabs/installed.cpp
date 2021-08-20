#include "installed.h"
#include "installation/manager.h"

bool toggleInstalledMod(brls::View* mod_view) {
    InstalledMod* clicked_mod = installed_mods->getInstalledMod(std::stoi(mod_view->getID()));
    Manager::ToggleMod(clicked_mod);
    float new_alpha = mod_view->getAlpha() > 0.5 ? 0.5 : 1.0;
    mod_view->setAlpha(new_alpha);

    bool new_state = clicked_mod->enabled;
    brls::Image* toggle_img = (brls::Image*)mod_view->getView("installed_item_toggle");
    if (new_state) // toggled on
        toggle_img->setImageFromRes("icon/toggle_on.png");
    else // toggled off
        toggle_img->setImageFromRes("icon/toggle_off.png");

    return true;
}

// This is definitely something that could be refactored. 
//      I.E. Set this up for each added installed item, and when uninstalling, just change the navigation ptrs for the two items around the one installed.

// iterate through the installed_box and set up/down navigation routes
void refreshNavigationInfo(brls::Box* installed_box) {
    //set up custom navigation because things totally break down with the default navigation logic for some reason
    std::vector<brls::View*>& installed_box_children = installed_box->getChildren();
    int installed_box_size = installed_box_children.size();
    if (installed_box_size > 1) {
        for (int i = 0; i < installed_box_size; i++) {
            // set this installed item's up navigation to previous item
            int up_idx = i-1 < 0 ? installed_box_size-1 : i-1;
            installed_box_children[i]->setCustomNavigationRoute(brls::FocusDirection::UP, installed_box_children[up_idx]);
            // set bottom installed item's down navigation to ourselves
            int down_idx = (i+1) % installed_box_size;
            installed_box_children[i]->setCustomNavigationRoute(brls::FocusDirection::DOWN, installed_box_children[down_idx]);
        }
    }
}

Installed::Installed() {
    this->inflateFromXMLRes("xml/tabs/installed.xml");

    // read from installed items json and populate based on that
    json& mem_json_installed = installed_mods->GetMemJson();
    if (mem_json_installed.contains("Installed") && mem_json_installed["Installed"].size() > 0) {
        CURL_builder curl = CURL_builder();
        for (json& entry : mem_json_installed["Installed"]) {
            if (!entry.is_null()) {

                std::vector<std::filesystem::path> paths = {};
                if (entry.contains(gb::Fields::Custom::Paths))
                    paths = entry[gb::Fields::Custom::Paths].get<std::vector<std::filesystem::path>>();

                InstalledMod* m = new InstalledMod({
                    entry[gb::Fields::Name].get<std::string>(), 
                    entry[gb::Fields::Submitter::Submitter][gb::Fields::Name].get<std::string>(), 
                    entry.contains(gb::Fields::Custom::Enabled) ? entry[gb::Fields::Custom::Enabled].get<bool>() : true,
                    entry[gb::Fields::idRow].get<std::string>(), 
                    entry[gb::Fields::Custom::ThumbnailURL].get<std::string>(), 
                    paths
                });

                this->addInstalledItem(m, &curl);

            }
        }
    }
    refreshNavigationInfo((brls::Box*)this->getView("installed_box"));
}

void Installed::addInstalledItem(InstalledMod* mod, CURL_builder* curl) {
    brls::Box* installed_item = (brls::Box*)brls::View::createFromXMLResource("views/installed_item.xml");
    brls::Box* installed_box = (brls::Box*)(this->getView("installed_box"));

    // no scrolling left/right
    installed_item->setCustomNavigationRoute(brls::FocusDirection::LEFT, nullptr);
    installed_item->setCustomNavigationRoute(brls::FocusDirection::RIGHT, nullptr);

    // text related elements

    brls::Label* installed_item_title = (brls::Label*)installed_item->getView("installed_item_title");
    installed_item_title->setText(mod->name);

    brls::Label* installed_item_author = (brls::Label*)installed_item->getView("installed_item_author");
    installed_item_author->setText("Author: " + mod->author);

    // thumbnail

    if (!NO_IMAGES) {
        brls::Image* installed_item_thumbnail = (brls::Image*)installed_item->getView("installed_item_thumbnail");
        if (!mod->thumbnail_url.empty()) {
            MemoryStruct img = curl::DownloadToMem(mod->thumbnail_url, curl);
            //brlsImageAsync(mod->thumbnail_url, installed_item_thumbnail);
            installed_item_thumbnail->setImageFromMem((unsigned char*)img.memory, img.size);
        }
    }

    // toggle

    brls::Image* installed_item_toggle = (brls::Image*)installed_item->getView("installed_item_toggle");
    if (!mod->enabled) {
        installed_item_toggle->setImageFromRes("icon/toggle_off.png");
        installed_item->setAlpha(0.5);
    }
    
    // add to installed_mods / register click action
    std::string id = std::to_string(installed_mods->getInstalledModsSize());
    installed_item->setId(id);
    installed_mods->addInstalledMod(mod);

    installed_item->registerClickAction(toggleInstalledMod);
    installed_item->registerAction(
        "Uninstall", brls::ControllerButton::BUTTON_X, [this] (brls::View* v) {
            this->onUninstallPrompt(v);
            return true;
        }, false, brls::Sound::SOUND_CLICK
    );

    installed_box->addView(installed_item);
}

void Installed::afterUninstallPrompt(brls::View* popup, brls::Box* installed_box) {
    if (popup && installed_box) {
        brls::Application::giveFocus(nullptr);
        this->removeView(popup);
        brls::Application::giveFocus(installed_box);
    }
    else 
        brls::Logger::warning("Popup/Installed box nullptr!");
}


void Installed::onUninstallPrompt(brls::View* installed_item) {
    InstalledMod* mod = installed_mods->getInstalledMod(std::stoi(installed_item->getID()));
    brls::Box* installed_box = (brls::Box*)(this->getView("installed_box"));

    brls::View* popup = brls::View::createFromXMLResource("views/popup.xml");
    this->addView(popup);
    brls::Label* popup_title_label = (brls::Label*)popup->getView("popup_title_label");
    popup_title_label->setText("Uninstall?");

    popup->getView("popup_confirm")->registerClickAction(
        [this, popup, mod, installed_item, installed_box] (brls::View* v) {
            brls::Application::giveFocus(nullptr);
            brls::Logger::debug("Uninstalling: {}", mod->name);
            Manager::UninstallMod(mod); // 'mod' gets freed
            installed_box->removeView(installed_item);
            refreshNavigationInfo(installed_box);
            this->afterUninstallPrompt(popup, installed_box);
            return true;
        }
    );
    popup->getView("popup_deny")->registerClickAction(
        [this, popup, installed_box] (brls::View* v) {
            this->afterUninstallPrompt(popup, installed_box);
            return true;
        }
    );
    brls::Application::giveFocus(popup);
}


void Installed::willAppear(bool resetState) {
    Box::willAppear(resetState);
    setTopText("A -> Toggle | X -> Uninstall");
    brls::Application::giveFocus(this->getView("installed_box"));
}

void Installed::willDisappear(bool resetState) {
    Box::willDisappear(resetState);
    setHintText();
    setTopText("UltimateModShop");
} 


brls::Box* Installed::create() {
    return new Installed();
}
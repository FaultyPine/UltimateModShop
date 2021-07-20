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

Installed::Installed() {
    this->inflateFromXMLRes("xml/tabs/installed.xml");
    // read from installed items json and populate based on that
    json mem_json_installed = installed_mods->GetMemJson();
    if (mem_json_installed.contains("Installed") && mem_json_installed["Installed"].size() > 0) {
        for (json entry : mem_json_installed["Installed"]) {
            if (!entry.is_null()) {

                std::vector<std::filesystem::path> paths = {};
                if (entry.contains(gb::Fields::Custom::Paths))
                    paths = entry[gb::Fields::Custom::Paths].get<std::vector<std::filesystem::path>>();

                std::string version;
                if (entry.contains(gb::Fields::AdditionalInfo::AdditionalInfo))
                    version = entry[gb::Fields::AdditionalInfo::AdditionalInfo][gb::Fields::AdditionalInfo::Version].get<std::string>();

                InstalledMod* m = new InstalledMod({
                    entry[gb::Fields::Name].get<std::string>(), 
                    entry[gb::Fields::Submitter::Submitter][gb::Fields::Name].get<std::string>(), 
                    version.empty() ? "0.0.0" : version,
                    entry.contains(gb::Fields::Custom::Enabled) ? entry[gb::Fields::Custom::Enabled].get<bool>() : true,
                    entry[gb::Fields::idRow].get<std::string>(), 
                    entry[gb::Fields::Custom::ThumbnailURL].get<std::string>(), 
                    paths
                });

                this->addInstalledItem(m);

            }
        }
    }

}

void Installed::addInstalledItem(InstalledMod* mod) {
    brls::Box* installed_item = (brls::Box*)brls::View::createFromXMLResource("views/installed_item.xml");

    // text related elements

    brls::Label* installed_item_title = (brls::Label*)installed_item->getView("installed_item_title");
    installed_item_title->setText(mod->name);

    brls::Label* installed_item_author = (brls::Label*)installed_item->getView("installed_item_author");
    installed_item_author->setText("Author: " + mod->author);

    brls::Label* installed_item_ver = (brls::Label*)installed_item->getView("installed_item_ver");
    installed_item_ver->setText("Ver. " + mod->ver);

    // thumbnail

    if (!NO_IMAGES) {
        brls::Image* installed_item_thumbnail = (brls::Image*)installed_item->getView("installed_item_thumbnail");
        if (!mod->thumbnail_url.empty()) {
            MemoryStruct img = curl::DownloadToMem(mod->thumbnail_url);
            //setBrlsImageAsync(mod->thumbnail_url, installed_item_thumbnail);
            installed_item_thumbnail->setImageFromMem((unsigned char*)img.memory, img.size);
        }
    }

    // toggle

    brls::Image* installed_item_toggle = (brls::Image*)installed_item->getView("installed_item_toggle");
    if (mod->enabled)
        installed_item_toggle->setImageFromRes("icon/toggle_on.png");
    
    // add to installed_mods / register click action
    std::string id = std::to_string(installed_mods->getInstalledModsSize());
    installed_item->setId(id);
    installed_mods->addInstalledMod(mod);

    brls::Box* installed_box = (brls::Box*)(this->getView("installed_box"));

    installed_item->registerClickAction(toggleInstalledMod);
    installed_item->registerAction(
        "Uninstall", brls::ControllerButton::BUTTON_X, [this, mod, installed_mods, installed_item, installed_box] (brls::View* v) {
            if (installed_item && mod) {
                std::string name = mod->name;
                Manager::UninstallMod(mod); // 'mod' gets freed
                installed_box->removeView(installed_item);
                brls::Application::giveFocus(installed_box);
                brls::Logger::debug("Uninstalled: {}", name);
            }
            return false;
        }, false, brls::Sound::SOUND_CLICK
    );

    installed_box->addView(installed_item);
}


void Installed::willAppear(bool resetState) {
    Box::willAppear(resetState);
    setTopText("A -> Toggle | X -> Uninstall");
}

void Installed::willDisappear(bool resetState) {
    setHintText();
    Box::willDisappear(resetState);
    setTopText("UltimateModShop");
} 


brls::Box* Installed::create() {
    return new Installed();
}
#include "installed.h"

bool toggleInstalledMod(brls::View* mod_view) {
    brls::Logger::debug("Clicked installed mod: {}", mod_view->describe());
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

                InstalledMod* m = new InstalledMod({
                    entry[gb::Fields::Name].get<std::string>(), 
                    entry[gb::Fields::Submitter::Submitter][gb::Fields::Name].get<std::string>(), 
                    "0.0.0", 
                    true, 
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

    // thumbnail    ---- in the future when Tasks are a thing in brls, spin a thread here to load the thumbnail in the background.

    if (!REDUCED_NET_REQUESTS) {
        brls::Image* installed_item_thumbnail = (brls::Image*)installed_item->getView("installed_item_thumbnail");
        if (!mod->thumbnail_url.empty()) {
            MemoryStruct img = curl::DownloadToMem(mod->thumbnail_url);
            installed_item_thumbnail->setImageFromMem((unsigned char*)img.memory, img.size);
        }
    }

    // toggle

    brls::Image* installed_item_toggle = (brls::Image*)installed_item->getView("installed_item_toggle");
    if (mod->enabled)
        installed_item_toggle->setImageFromRes("icon/toggle_on.png");
    
    // add to installed_mods / register click action
    std::string id = "installed_item" + std::to_string(installed_mods->getInstalledModsSize());
    installed_item->setId(id);
    installed_mods->addInstalledMod(mod);

    installed_item->registerClickAction(toggleInstalledMod);

    ((brls::Box*)(this->getView("installed_box")))->addView(installed_item);
}


void Installed::willAppear(bool resetState) {
    setHintText("A -> Toggle | X -> Uninstall");
    Box::willAppear(resetState);
}

void Installed::willDisappear(bool resetState) {
    setHintText();
    Box::willDisappear(resetState);
} 


brls::Box* Installed::create() {
    return new Installed();
}
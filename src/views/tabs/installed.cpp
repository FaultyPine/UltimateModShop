#include "installed.h"

const std::string installed_item_init = R"xml(
    <brls:Box
        id="installed_item"
        width="90%"
        height="10%"
        borderThickness="3"
        borderColor="#FFFFFF"
        focusable="true"
    >

    <brls:Label
        id="installed_item_label"
    />


    </brls:Box>
)xml";

/**
 *  Need to reformat this so that extra installed item views are only added when something is downloaded, and things would get init-ed from the ctor probably
 */
void Installed::LoadInstalledFromMem() {
    for (brls::View* v : this->getChildren())
        this->removeView(v);
    brls::Logger::debug("Reading installed mods...");
    json mem_json_installed = installed_mods->GetMemJson();
    if (mem_json_installed.contains("Installed") && mem_json_installed["Installed"].size() > 0) {
        for (json entry : mem_json_installed["Installed"]) {
            if (!entry.is_null()) {
                brls::Box* installed_item = (brls::Box*)brls::View::createFromXMLString(installed_item_init);
                ((brls::Label*)installed_item->getView("installed_item_label"))->setText(entry[gb::Fields::Title].get<std::string>());
                this->addView(installed_item);
            }
        }
    } else {
        brls::Logger::debug("didn't contain installed");
    }


}

Installed::Installed() {
    this->inflateFromXMLRes("xml/tabs/installed.xml");
    // read from installed items json and populate based on that
}

void Installed::show(std::function<void(void)> cb) {
    brls::View::show(cb);
    brls::Logger::debug("Showing Installed");
    LoadInstalledFromMem();
}


brls::Box* Installed::create() {
    return new Installed();
}
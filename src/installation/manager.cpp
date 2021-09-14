#include "manager.h"

const std::vector<std::string> ArcFolders = {
    "append",
    "assist",
    "boss",
    "camera",
    "campaign",
    "common",
    "effect",
    "enemy",
    "fighter",
    "finalsmash",
    "item",
    "miihat",
    "param",
    "pokemon",
    "prebuilt:",
    "render",
    "snapshot",
    "sound",
    "spirits",
    "stage",
    "standard",
    "stream:",
    "ui",
};

void move_folder(const fs::path& src, const fs::path& dst, bool createRoot = true) {
    if (createRoot) {
        fs::create_directories(dst);
    }

    for(fs::path p: fs::directory_iterator(src)){
        fs::path destFile = dst/p.filename();

        if (fs::is_directory(p)) {
            fs::create_directory(destFile);
            move_folder(p.string(), destFile.string(), false);
        } else {
            fs::rename(p, destFile);
        }
    }
}
void move_file(const fs::path& src, const fs::path& dest) {
    if (!fs::exists(src)) {
        brls::Logger::error("Tried to move file that doesn't exist!");
        return;
    }
    if (!fs::exists(dest)) {
        fs::create_directories(dest);
    }

    if (fs::is_directory(dest)) {
        fs::rename(src, dest/fs::path(src).filename());
    }
    else {
        brls::Logger::error("Cannot move file into another file!");
    }
}

void moveFSItem(const fs::path& src, const fs::path& dest) {
    if (fs::is_directory(src)) {
        move_folder(src, dest/src.filename());
    }
    else {
        move_file(src, dest);
    }
}

template<class T>
void appendVector(std::vector<T>& a, const std::vector<T>& b) {
    a.insert(a.end(), b.begin(), b.end());
}

/**
 *  Philosophy of parsing relevant mod files:
 * 
 *     Basically, only care about arc mod folders, and skyline plugins. Anything else that is relevant to a mod should be specified in the info.toml.
 *      
 *     For arc mods, basically we just find an arc root folder, then go one dir level out to get the mod folder.
 *      To get the workspace, we'd just go one more out from that ^ and make sure it isn't `tmp_dir` or the `mods` folder
 * 
 *     For skyline plugins, just look for .nro files. If its a dependency (harcoded detection atm... icky) plop it in the deps folder, if not,
 *      put all the plugins for the mod in a corresponding folder for that mod.
 * 
 */


std::vector<fs::path> handleArcFiles(const std::string& tmp_extraction_dir, const std::vector<fs::path>& tmp_dir_paths, const std::string& zip_filename) {
    std::vector<fs::path> paths = {};

    for (fs::path p : tmp_dir_paths) {
        // this ignores workspaces. If a mod has a workspace, I'm fine with assuming that it is formatted for the sd root.
        if (std::find(ArcFolders.begin(), ArcFolders.end(), p.filename()) != ArcFolders.end()) {
            fs::path parent = p.parent_path();

            // if parent is the tmp extraction dir (which means the mod itself doesn't have a proper ultimate/mods/MyMod folder), insert the zip filename as the "name" of the mod
            if (parent.filename().string() == TEMP_EXTRACTION_DIRNAME) {
                fs::path new_path = MODS_WORKSPACE_PATH + zip_filename + "/" + p.filename().string();
                move_folder(p.string(), new_path.string());
                paths.push_back(new_path.parent_path());

                continue;
            }

            // if we haven't already pushed this mod folder... accounts for mods with multiple arc folder roots... I.E. having a `ui` AND a `fighter` folder
            if ( std::none_of(paths.begin(), paths.end(), [parent](fs::path i){ 
                    return str_contains(i.string(), parent.filename().string()) || str_contains(parent.string(), i.filename().string()) ;
                }) ) {
                fs::path new_path = MODS_WORKSPACE_PATH + parent.filename().string();
                
                paths.push_back(new_path);
                move_folder(parent.string(), new_path);
            }

        }
    }
 
    return paths;
}

std::vector<std::string> pluginDeps = {
    "libacmd_hook",
    "libnro_hook",
    "liblua_replace",
    "libsmashline_hook",
    "libstatus-hook",
    "libarcropolis",
    "libparam_hook"
};
// put all plugin deps in a "dependencies" folder in the skyline/plugins path.
std::vector<fs::path> handleSkylinePlugins(const std::string& tmp_extraction_dir, const std::vector<fs::path>& tmp_dir_paths, const std::string& zip_filename) {
    std::vector<fs::path> paths = {};

    for (fs::path p : tmp_dir_paths) {
        if (p.extension().string() == ".nro") {
            // if the file is a plugin dependency
            if (std::find(pluginDeps.begin(), pluginDeps.end(), p.stem().string()) != pluginDeps.end()) {
                std::string new_path = SKYLINE_PLUGIN_DEP_PATH + p.filename().string();
                fs::rename(p, new_path);
                // We don't push plugin deps to paths since they won't be toggled/uninstalled with the normal mod
            }
            // file is a normal plugin
            else {
                fs::path new_path = SKYLINE_PLUGINS_PATH + zip_filename;
                fs::create_directory(new_path); // make sure path exists before moving it
                new_path.append(p.filename().string());
                fs::rename(p, new_path);
                paths.push_back(new_path);
            }
        }
    }

    return paths;
}

/**
 * info.toml files looks something like
 * 
[
    ["atmosphere", ""],
    ["MyModFolder", "ultimate/mods/"],
]
 * Empty string -> place at sd card root.
 * pre-pend sd card root (or platform-specific equivalent) to all paths
 */
std::vector<fs::path> handleTomlFiles(std::optional<toml::table> data) {
    std::vector<fs::path> ret = {};
    /* Read in paths from toml file */

    /* If there is an info.toml in this mod, get all those paths and pass them into the subsequent handler funcs */
    std::vector<std::pair<fs::path, fs::path>> toml_paths = {};
    if (data) {
        toml::table tbl = *data;

        if (tbl.contains("paths")) {    
            toml::array paths = *tbl["paths"].as_array();
            for (int i = 0; i < paths.size(); i++) {
                toml::array* element = paths.get(i)->as_array();
                std::string idx1 = element->get(0)->as_string()->get();
                std::string idx2 = element->get(1)->as_string()->get();
                toml_paths.push_back(std::make_pair(idx1, idx2));
            }
        }
    }
    else {
        brls::Logger::debug("No toml found in mod");
    }

    /* 
        Handling the toml paths..

        we first move all relevant files/folders from the tmp extraction dir into their destination folders.
        Then we delete those files/folders from the tmp extraction dir
    */

    for (std::pair<fs::path, fs::path> pathPair : toml_paths) {

        fs::path src = TEMP_EXTRACTION_DIR + pathPair.first.string();
        fs::path dest = SD_ROOT + pathPair.second.string();

        brls::Logger::debug("Toml Path:  {} -> {}", src.string(), dest.string());

        moveFSItem(src, dest);
        ret.push_back(dest);
        fs::remove_all(src);
    }
    return ret;
}

// returns vector of paths that contain this mods files.
std::vector<fs::path> handleModFiles(
    const std::string& tmp_extraction_dir, 
    const std::vector<fs::path>& tmp_dir_paths, 
    const std::string& zip_filename
) {
    std::vector<fs::path> ret_paths = {};

    for (fs::path p : handleArcFiles(tmp_extraction_dir, tmp_dir_paths, zip_filename)) {
        brls::Logger::debug("ArcFile: {}", p.string());
        ret_paths.push_back(p);
    }

    for (fs::path p : handleSkylinePlugins(tmp_extraction_dir, tmp_dir_paths, zip_filename)) {
        brls::Logger::debug("SkylinePlugin: {}", p.string());
        ret_paths.push_back(p);
    }

    return ret_paths;
}

std::vector<fs::path> Manager::InstallModFileAuto(std::string url, std::string filename, CURL_builder* curl) {
    std::vector<fs::path> paths = {};
    std::string tmp_extraction_dir = TEMP_EXTRACTION_DIR;
    std::string root_file = SD_ROOT + filename;

    // create temp directory to allow us to easily read the folder structure
    fs::create_directory(tmp_extraction_dir);

    curl::DownloadFile(url, root_file, curl);
    
    if (UnZip::ArchiveExtract(root_file, tmp_extraction_dir)) {
        
        // mod creator explicit pathing with info.toml
        std::string infoTomlPath = tmp_extraction_dir + "info.toml";
        if (fs::exists(infoTomlPath)) { // assumes that info.toml would be at the root of the (extracted) archive
            try {
                std::optional<toml::table> data = toml::parse_file(infoTomlPath);
                appendVector(paths, handleTomlFiles(data));
            }
            catch (const toml::parse_error& err) {
                brls::Logger::error("Toml parsing failed: {}", err.description());
            }
        }
        else {
            brls::Logger::debug("No toml file found!");
        }


        std::vector<fs::path> tmp_dir_paths = {};
        for (const auto& dirEntry : fs::recursive_directory_iterator(tmp_extraction_dir)) {
            tmp_dir_paths.push_back(dirEntry.path());
        }

        std::vector<fs::path> mod_files = handleModFiles(tmp_extraction_dir, tmp_dir_paths, filename.substr(0, filename.find_last_of(".")));
        appendVector(paths, mod_files);
    }
    else {
        brls::Logger::warning("Failed to extract!");
    }

    // cleanup
    fs::remove(root_file);
    fs::remove_all(tmp_extraction_dir);

    return paths;
}

// takes in gb::Fields::Files::Files
std::vector<std::filesystem::path> Manager::InstallModFilesAuto(const json &files, const std::vector<bool>& dl_idxs, CURL_builder* curl) {
    assert(files.size() == dl_idxs.size());
    std::vector<fs::path> paths = {};

    // iterate through each uploaded file in the submission, 
    // and if the corresponding idx is true, install that file.
    for (int i = 0; i < files.size(); i++) { 
        if (dl_idxs[i]) { // for each submission file that the user selected
            const json& file = files[i];
            brls::Logger::debug("Downloading file {}. Size = {} bytes", file[gb::Fields::Files::FileName].get<std::string>(), file[gb::Fields::Files::FileSize].get<unsigned long>());
            std::string url = file[gb::Fields::Files::DownloadURL].get<std::string>();
            std::string filename = file[gb::Fields::Files::FileName].get<std::string>();
            std::vector<fs::path> modfile = Manager::InstallModFileAuto(url, filename, curl);
            // append contents of modfile vec to paths vec
            paths.insert(paths.end(), modfile.begin(), modfile.end());
        }
    }

    return paths;
}



void Manager::ToggleMod(InstalledMod* mod) {
    json* installed_mods_json = &installed_mods->GetMemJsonPtr()->at("Installed");
    for (int i = 0; i < mod->paths.size(); i++) {
        fs::path path = mod->paths.at(i);
        if (fs::exists(path)) {
            // if is a plugin, and not a plugin dependency, and exists, place it in "disabled_plugins" folder
            if (path.extension().string() == ".nro" && !str_contains(path.string(), SKYLINE_PLUGIN_DEP_PATH)) {
                std::string old_path = path.string();
                std::string new_path = mod->enabled ?
                                        replaceAll(path, "plugins", "disabled_plugins") : replaceAll(path, "disabled_plugins", "plugins") ;
                fs::rename(old_path, new_path);

                mod->paths.at(i) = fs::path(new_path);
                installed_mods_json->at(mod->itemid)[gb::Fields::Custom::Paths][i] = new_path;
            }
            // else, is an arc mod. So rename it with a period in front to disable
            else if (str_contains(path.string(), ULTIMATE_ARC_PATH) && (mod->enabled ? path.filename().string().at(0) != '.' : path.filename().string().at(0) == '.')) {
                std::string old_path = path.string();
                std::string new_path = mod->enabled ? 
                                       path.replace_filename("." + path.filename().string()) : path.replace_filename(path.filename().string().substr(1));
                fs::rename(old_path, new_path);

                mod->paths.at(i) = fs::path(new_path);
                installed_mods_json->at(mod->itemid)[gb::Fields::Custom::Paths][i] = new_path;
            }
        }
    }
    installed_mods_json->at(mod->itemid)[gb::Fields::Custom::Enabled] = !mod->enabled;
    installed_mods->OverwriteFileFromMem();
    mod->enabled = !mod->enabled;
}

void Manager::UninstallMod(InstalledMod* mod) {
    for (fs::path p : mod->paths) {
        if (fs::exists(p)) {
            fs::remove_all(p);
        }
    }
    installed_mods->removeInstalledMod(mod);
}

// determines based off archive file structure if an automatic installation is possible
bool Manager::canUseAutomaticInstallation(const std::string& itemid, CURL_builder* curl) {
    //const std::string api_req = "https://api.gamebanana.com/Core/Item/Data?itemtype=File&itemid=" + itemid + "&fields=Metadata().aArchiveFilesList()";
    //json archiveFiles = curl::DownloadJson(api_req, curl);

    return true;
}
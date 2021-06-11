#include "manager.h"

std::vector<std::string> ArcFolders = {
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

void move_folder(std::string sorc, std::string dst, bool createRoot = true) {
    if (createRoot)
        fs::create_directories(dst);

    for(fs::path p: fs::directory_iterator(sorc)){
        fs::path destFile = dst/p.filename();

        if (fs::is_directory(p)) {
            fs::create_directory(destFile);
            move_folder(p.string(), destFile.string(), false);
        } else {
            fs::rename(p, destFile);
        }
    }
}

/**
 *  Philosophy of parsing relevant mod files:
 * 
 *      Basically, only care about arc mod folders, and skyline plugins. Anything else that is relevant to a mod should be specified in some sort of info.ini type file.
 *      
 *      
 *     For arc mods, basically we just find an arc root folder, then go one dir level out to get the mod folder.
 *      To get the workspace, we'd just go one more out from that ^ and make sure it isn't `tmp_dir` or the `mods` folder
 * 
 *     For skyline plugins, just look for .nro files. If its a dependency (harcoded detection atm... icky) plop it in the deps folder, if not,
 *      put all the plugins for the mod in a corresponding folder for that mod.
 * 
 *  
 *     ^ that's the automatic part of the system. If there are any mod files that should be dealt with differently, mod creators will need to specify
 *      that in some sort of modfiles.ini or something for me to parse.
 *      Might also just give users a prompt to put files in proper locations themselves... would honestly be less of a headache
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

// returns vector of paths that contain this mods files.
std::vector<fs::path> handleModFiles(const std::string& tmp_extraction_dir, const std::vector<fs::path>& tmp_dir_paths, const std::string& zip_filename) {
    std::vector<fs::path> paths = {};

    for (fs::path p : handleArcFiles(tmp_extraction_dir, tmp_dir_paths, zip_filename)) {
        brls::Logger::debug("ArcFile: {}", p.string());
        paths.push_back(p);
    }

    for (fs::path p : handleSkylinePlugins(tmp_extraction_dir, tmp_dir_paths, zip_filename)) {
        brls::Logger::debug("SkylinePlugin: {}", p.string());
        paths.push_back(p);
    }

    return paths;
}

std::vector<fs::path> Manager::InstallModFile(std::string url, std::string filename) {
    std::vector<fs::path> paths = {};
    std::string tmp_extraction_dir = TEMP_EXTRACTION_DIR;
    if (strHasEnding(filename, ".7z")) {
        brls::Logger::error("Installing .7z archives is currently unsupported.");
        return paths;
    }
    std::string root_file = stdstr(SD_ROOT) + filename;

    // create temp directory to allow us to easily read the folder structure
    fs::create_directory(tmp_extraction_dir);

    curl::DownloadFile(url, root_file);
    UnZip::ArchiveExtract(root_file, tmp_extraction_dir);
    fs::remove(root_file);

    std::vector<fs::path> tmp_dir_paths = {};

    for (const auto& dirEntry : fs::recursive_directory_iterator(tmp_extraction_dir)) {
        tmp_dir_paths.push_back(dirEntry);
    }

    paths = handleModFiles(tmp_extraction_dir, tmp_dir_paths, filename.substr(0, filename.find_last_of(".")));

    // remove temp directory and all it's contents
    fs::remove_all(tmp_extraction_dir);

    return paths;
}

// takes in gb::Fields::Files::Files
std::vector<std::filesystem::path> Manager::InstallModFiles(const json &files) {
    std::vector<fs::path> paths = {};

    json files_to_install;
    if (files.size() > 1) {
        // TODO: let users choose which files they want to install
    }

    for (json file : files) { // iterate through each uploaded file in the submission... will probably end up prompting the user somehow or having them select which files in the submission they want
        brls::Logger::debug("Downloading file ({} files total)). Size = {} bytes", files.size(), file[gb::Fields::Files::FileSize].get<unsigned long>());
        std::string url = file[gb::Fields::Files::DownloadURL].get<std::string>();
        std::string filename = file[gb::Fields::Files::FileName].get<std::string>();
        std::vector<fs::path> modfile = Manager::InstallModFile(url, filename);
        // append contents of modfile vec to paths vec
        paths.insert(paths.end(), modfile.begin(), modfile.end());
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
#pragma once

#include "ums_utils.h"
using json = nlohmann::json;


class InstalledMods {

    public:
    /// overwrites file and puts all data from installed_json in 
    void OverwriteFileFromMem();

    /// appends json data to the file
    void AppendToFile(json j);

    /// Overwrites in-memory json with data from file
    void OverwriteMemFromFile();

    /// appends json data to in-memory json data
    void AppendToMem(json j);

    /// returns json obj of data from file
    json GetFileJson();

    /// returns this json obj
    json GetMemJson();

    /// returns a pointer to this json obj for direct manipulation
    json* GetMemJsonPtr();

    void resetFile();
    /// Init json for installed mods
    InstalledMods(json default_json = json(  { { "Installed", {} } }   ));


    private:
    json installed_json;

};
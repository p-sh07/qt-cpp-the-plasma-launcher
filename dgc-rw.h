//
//  Doom-game-config reader/writer
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#pragma once

#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_set>

using std::ofstream;
using std::ifstream;

namespace fs = std::filesystem;

namespace dgc {

const fs::path DFLT_DCG_PATH = "mod_settings.dgc";
const fs::path DFLT_GZDOOM = "gzdoom";
const fs::path DFLT_CONFIG = "gzdoom_portable.ini";
const fs::path DFLT_DGC_FILE = "games_config.dgc";

const fs::path DFLT_IWADS_PATH = "iwads";
const fs::path DFLT_MODWADS_PATH = "mods";

struct ModSet
{
    ModSet() = default;
    ModSet(std::string label, std::string iwad = "", std::vector<std::string> mod_filenames = {});

    //Required:
    std::string label = "Default";
    std::string iwad = "DOOM.WAD";
    std::vector<std::string> selected_mods;

    //Optional, if needed to specify:
    fs::path gzdoom_dir;
    fs::path config;
    fs::path iwad_dir;
    fs::path mod_dir;
};

class DgcParser {
public:
    //open default file if empty path
    DgcParser(const fs::path settings_file = {});

    void InitPathsForMac();

    bool open(const fs::path& settings_file);
    std::vector<ModSet> ParseFromFile();
private:
    std::fstream file_;
};

}

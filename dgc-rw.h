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
    ModSet(std::string label, fs::path iwad, std::unordered_set<std::string> mod_filenames);
    std::string label = "None";
    fs::path gzdoom_dir = DFLT_GZDOOM;
    fs::path config = DFLT_CONFIG;
    fs::path iwad_dir = "iwads";
    fs::path iwad = "iwads/DOOM.WAD";
    fs::path mod_dir = "mods";
    std::unordered_set<std::string> mod_filenames;
    
    //zips dir? multiple dirs?
};

class DgcParser {
public:
    //open default file if empty path
    DgcParser(const fs::path settings_file = {});

    bool open(const fs::path& settings_file);
    std::vector<ModSet> ParseFromFile();
private:
    std::fstream file_;
};

}

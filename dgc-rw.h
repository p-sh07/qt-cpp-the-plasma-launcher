//
//  Doom-game-config reader/writer
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

using std::ofstream;
using std::ifstream;

namespace fs = std::filesystem;
//==========================================================//

namespace dgc {
//==========================================================//
//-------------- ModSet ----------------//
struct ModSet
{
    ModSet() = default;
    ModSet(std::string label, std::string iwad = "DOOM.WAD", std::vector<std::string> mod_filenames = {});

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
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//


//==========================================================//
//-------------- Launch Settings ----------------//
struct LaunchSettings {
    inline bool Empty() const {
        return working_folder.empty();
    }

    fs::path working_folder;
    fs::path doom_game_config = "plasma_settings.dgc";

    fs::path gzdoom_folder;
    fs::path gzdoom_ini_path;
    fs::path gzdoom_saves;

    fs::path iwad_folder;
    fs::path mod_folder;

    size_t chosen_iwad = 0;
    size_t chosen_mod_set = 0;
    std::vector<size_t> chosen_mods;

    std::vector<std::string> iwad_filenames;
    std::vector<std::string> mod_filenames;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//


//==========================================================//
//-------------- ParsingError ----------------//
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//


//==========================================================//
//-------------- DgContents ----------------//
struct DgContents {
    LaunchSettings settings;
    std::vector<ModSet> games;
};
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//



//==========================================================//
//-------------- Parser ----------------//
class Parser {
public:
    //open default file if empty path
    Parser();

    LaunchSettings GetDefaults(fs::path working_folder = {});
    DgContents ParseFile(const fs::path& dgc_path);
    void Write(const fs::path to_file, const LaunchSettings& lcfg, const std::vector<ModSet>& presets = {});

    bool AddWadDirToGzdoomIni(const fs::path& iwad_full_path);

private:
//---- Var ----//
    const LaunchSettings _default_cfg;

//---- Methods ----//
    LaunchSettings ReadLaunchPaths(std::fstream& fs);
    std::vector<ModSet> ParseModPresets(std::fstream& fs);

};


}//namespace dgc
//==========================================================//

//
//  Doom-game-config reader/writer
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#pragma once


#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using std::ofstream;
using std::ifstream;

namespace fs = std::filesystem;
//==========================================================//

namespace dgc {

const fs::path DFLT_DCG_PATH = "D:/User/Documents/My Games/PlasmaLauncher/mod_settings.dgc";

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
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
//-------------- Launch Settings ----------------//
struct LaunchSettings {
    fs::path mod_config_path;

    fs::path working_folder;
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

    fs::path gzdoom_exe;
    fs::path gzdoom_app;

    std::vector<dgc::ModSet> games;
};

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};
//==========================================================//


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
//-------------- ModSet ----------------//
class DgcParser {
public:
    //open default file if empty path
    DgcParser(const fs::path settings_file = DFLT_DCG_PATH);

    void Write(const ModSet& mset);
    void Write(const LaunchSettings& lcfg);

    LaunchSettings MakeDefaults();
    LaunchSettings MakeDefaultsWin();
    LaunchSettings MakeDefaultsMac();

    bool Open(const fs::path& settings_file);

    LaunchSettings ParseLaunchSettings();
    void ReadLaunchPaths(std::fstream& fs, LaunchSettings& settings);
    std::vector<ModSet> ParseModPresets(std::fstream& in);

    bool AddWadDirToGzdoomIni(const fs::path& iwad_full_path);
private:
    fs::path dgc_file_;
    const LaunchSettings _default_cfg;

};


}//namespace dgc
//==========================================================//

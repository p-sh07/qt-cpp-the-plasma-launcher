//{
//  dgc-rw.cpp
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#include "dgc-rw.h"


//================================================================//
//================ String Processing Functions ================//
namespace {

bool EraseSymbol(const char& c) {
    return c == ' ' || c == '[' || c == '{'
           || c == ']' || c == '}' || c == ';' || c == '\t';
}

std::string& TrimWs (std::string& str) {
    //remove leading and lagging whitespace, and braces
    while(!str.empty() && str[0] == ' ') {
        str.erase(0);
    }
    while(!str.empty() && str.back() == ' ') {
        str.erase(str.size()-1);
    }
    return str;
}

std::string& Trim (std::string& str) {
    //remove leading and lagging whitespace, and braces
    while(!str.empty() && EraseSymbol(str[0])) {
        str.erase(0, 1);
    }
    while(!str.empty() && EraseSymbol(str.back())) {
        str.erase(str.size()-1);
    }
    return str;
}

std::pair<std::string, std::string> Split (std::string& str) {
    //remove leading and lagging whitespace, and braces
    Trim(str);

    std::cerr << "Trimmed str: " << str;

    auto pos = str.find(':');

    if(pos == str.npos) {
        throw std::runtime_error("Separator : not found");
    }

    std::cerr << "Split str: " << str.substr(0, pos) << ' ' << str.substr(pos+1);

    return {str.substr(0, pos), str.substr(pos+1)};
}

} //namespace

namespace dgc {

using namespace std::literals;

const std::string LAST_STNGS_LABEL = "Prev Settings"s;
const std::string MODSETS_LABEL = "Mod Presets"s;

const std::string WDIR_LABEL = "WORKING_DIR"s;
const std::string IWADS_LABEL = "IWADS"s;
const std::string MODS_LABEL = "MODS"s;

const std::string GZDOOM_LABEL = "GZDOOM"s;
const std::string GZDOOM_INI_LABEL = "GZDOOM_INI"s;
const std::string GZDOOM_SAVES_LABEL = "GZDOOM_SAVES"s;

ModSet::ModSet(std::string label, std::string iwad, std::vector<std::string> mod_filenames)
    : label(std::move(label))
    , iwad(std::move(iwad))
    , selected_mods(mod_filenames) {
}

//open default file if empty path
DgcParser::DgcParser(const fs::path settings_file)
    : dgc_file_(settings_file)
    , _default_cfg(MakeDefaults()) {

}

bool DgcParser::Open(const fs::path& settings_file) {
    dgc_file_ = settings_file;

    std::fstream fs(dgc_file_, std::ios_base::in);
    return fs.is_open();
}

void DgcParser::ReadLaunchPaths(std::fstream& fs, LaunchSettings& settings) {
    std::string line;
    auto pos = fs.tellg();
    while(getline(fs, line)) {
        if(!TrimWs(line).empty() && line[0] == '[') {
            //go back to the start of line
            fs.seekg(pos);
            break;
        }

        if(line.empty() || line[0] == '#') {
            continue;
        }

        //LABELS:
        const auto& [label, val] = Split(line);
        if(label == WDIR_LABEL) {
            settings.working_folder = val;
        } else if(label == WDIR_LABEL) {
            settings.working_folder = val;
        } else if(label == IWADS_LABEL) {
            settings.iwad_folder = val;
        } else if(label == MODS_LABEL) {
            settings.mod_folder = val;
        } else if(label == GZDOOM_LABEL) {
            settings.gzdoom_folder = val;
        } else if(label == GZDOOM_INI_LABEL) {
            settings.gzdoom_ini_path = val;
        } else if(label == GZDOOM_SAVES_LABEL) {
            settings.gzdoom_saves = val;
        }
    }
    pos = fs.tellg();
}

std::vector<ModSet> DgcParser::ParseModPresets(std::fstream& fs) {
    std::vector<ModSet> games;

    std::string line;
    //auto pos = fs.tellg();

    while(getline(fs, line)) {
        if(!TrimWs(line).empty() && line[0] == '}') {
            break;
        }
        if(TrimWs(line).empty() || line[0] == '#') {
            continue;
        }
        const auto& [label, val] = Split(line);

        games.push_back({});
        if(label == "Label"s) {
            games.back().label = val;
        } else if(label == "iWad"s) {
            games.back().iwad = val;
        } else if(label == "Mods"s) {
            //Link to dst:
            auto& mods = games.back().selected_mods;
            std::string mod_name = val;
            //process other lines until ';'
            while(!TrimWs(mod_name).empty() && mod_name.back() != ';') {
                mods.push_back(std::move(mod_name));
                getline(fs, mod_name);
            } //process last line
                if(!Trim(mod_name).empty()) {
                    mods.push_back(std::move(mod_name));
            }
        }
    }


    return games;
}


LaunchSettings DgcParser::ParseLaunchSettings() {
    LaunchSettings stngs;

    std::fstream fs(dgc_file_, std::ios_base::in);

    LaunchSettings settings;
    std::string line;
    while(std::getline(fs, line)) {
        //Get rid of whitespaces before & after text
        TrimWs(line);

        //Skip empty lines and #comments
        if(line.empty() || line[0] == '#') {
            continue;
        }

        //Settings Section Label
        if(line[0] == '[') {
            //Get rid of braces and more whitespace
            Trim(line);
            if(line == LAST_STNGS_LABEL) {
                ReadLaunchPaths(fs, stngs);
            }
            else if(line == MODSETS_LABEL) {
                settings.games = ParseModPresets(fs);
            }
        }

    }

    return stngs;
}

LaunchSettings DgcParser::MakeDefaults() {
    #ifdef WIN_BUILD
    return MakeDefaultsWin();
    #elif defined(MAC_BUILD)
    return MakeDefaultsMac();
    #endif
}

LaunchSettings DgcParser::MakeDefaultsWin() {
    LaunchSettings parsed_settings;

    return parsed_settings;
}

LaunchSettings DgcParser::MakeDefaultsMac() {
    LaunchSettings parsed_settings;

    return parsed_settings;
}


}//namespace dgc

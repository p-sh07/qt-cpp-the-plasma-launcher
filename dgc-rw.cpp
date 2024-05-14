//{
//  dgc-rw.cpp
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#include "dgc-rw.h"

using std::cerr;
using std::endl;
using std::string;
using std::string_view;

//================================================================//
//================ String Processing Functions ================//
namespace {

bool eraseSymbol(const char& c) {
    return c == ' ' || c == '[' || c == '{'
           || c == ']' || c == '}' || c == ';' || c == ':';
}

std::string_view trimWs (std::string_view str) {
    //remove leading and lagging whitespace, and braces
    while(!str.empty() && str[0] == ' ') {
        str.remove_prefix(1);
    }
    while(!str.empty() && str.back() == ' ') {
        str.remove_suffix(1);
    }
    return str;
}

std::string_view trim (std::string_view str) {
    //remove leading and lagging whitespace, and braces
    while(!str.empty() && eraseSymbol(str[0])) {
        str.remove_prefix(1);
    }
    while(!str.empty() && eraseSymbol(str.back())) {
        str.remove_suffix(1);
    }
    return str;
}

std::pair<std::string_view, std::string_view> split (std::string_view str) {
    //remove leading and lagging whitespace, and braces
    trim(str);
    
    auto pos = str.find(':');
    
    if(pos == str.npos) {
        //throw std::runtime_error("Separator : not found");
    }
    string_view label = trim(str.substr(0, pos));
    string_view val = trim(str.substr(pos+1));
    
    return {label, val};
}

bool readSequence(std::fstream& fs, std::string& line, bool& skip_line, bool& is_last_line, char end_symbol = ';') {
    if(!getline(fs, line)) {
        return false;
    }
    auto no_ws_line = trimWs(line);
    
    if(no_ws_line.empty() || no_ws_line[0] == '#') {
        skip_line = true;
        is_last_line = false;
        return true;
    } else if(no_ws_line[0] == end_symbol) {
        skip_line = true;
        is_last_line = true;
        return false;
    } else if(size_t end_pos = line.find(end_symbol); end_pos!= line.npos) {
        //discard text after end pos symbol
        line = line.substr(0, end_pos);
        
        skip_line = false;
        is_last_line = true;
        return true;
    }
    
    is_last_line = false;
    skip_line = false;
    return true;
}

std::string indent(int n = 2) {
    return std::string(n, ' ');
}

} //namespace

namespace dgc {

using namespace std::literals;

const std::string_view LAST_STNGS_LABEL = "Prev Settings"sv;
const std::string_view MODSETS_LABEL = "Mod Presets"sv;

const std::string_view WDIR_LABEL = "WORKING_DIR"sv;
const std::string_view IWADS_LABEL = "IWADS"sv;
const std::string_view MODS_LABEL = "MODS"sv;

const std::string_view GZDOOM_LABEL = "GZDOOM"sv;
const std::string_view GZDOOM_INI_LABEL = "GZDOOM_INI"sv;
const std::string_view GZDOOM_SAVES_LABEL = "GZDOOM_SAVES"sv;

const std::string_view MODSET_LABEL = "Label"sv;
const std::string_view MS_IWAD_LABEL = "iWad"sv;
const std::string_view MS_MOD_LABEL = "Mods"sv;

ModSet::ModSet(std::string label, std::string iwad, std::vector<std::string> mod_filenames)
    : label(std::move(label))
    , iwad(std::move(iwad))
    , selected_mods(mod_filenames) {
}

//open default file if empty path
Parser::Parser(const fs::path settings_file)
    : dgc_file_(settings_file)
    , _default_cfg(MakeDefaults()) {
    //NB: Creating file if doesn't exist!
    if(!fs::exists(dgc_file_)) {
        Write(_default_cfg);
    }
}

bool Parser::Open(const fs::path& settings_file) {
    dgc_file_ = settings_file;
    
    std::fstream fs(dgc_file_);
    return fs.is_open();
}

LaunchSettings Parser::ReadLaunchPaths(std::fstream& fs) {
    
    LaunchSettings settings;
    std::string line;
    bool skip_line = false, section_end = false;
    
    while(!section_end && readSequence(fs, line, skip_line, section_end)) {
        if(skip_line) {
            continue;
        }
        string_view ln_view = trimWs(line);
        
        //LABELS:
        auto [label, val] = split(ln_view);
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
    return settings;
}

std::vector<ModSet> Parser::ParseModPresets(std::fstream& fs) {
    std::vector<ModSet> games;
    
    std::string line;
    bool skip_line = false, presets_section_end = false;
    //auto pos = fs.tellg();
    
    while(!presets_section_end && readSequence(fs, line, skip_line, presets_section_end)) {
        if(skip_line) {
            continue;
        }
        
        try{
            auto [label, value] = split(line);
            
            if(label == MODSET_LABEL) {
                games.push_back(std::string(value));
            } else if(label == MS_IWAD_LABEL) {
                games.back().iwad = value;
            } else if(label == MS_MOD_LABEL) {
                //in case there was a first line after 'Mods:'
                if(!value.empty()) {
                    games.back().selected_mods.push_back(std::string(value));
                }
                
                string mod_line;
                bool skip_mod_line =false, mod_section_end = false;
                
                while(!mod_section_end && readSequence(fs, mod_line, skip_mod_line, mod_section_end, '}')) {
                    
                    if(skip_mod_line) {
                        continue;
                    }
                    
                    string_view no_ws_line_view = trimWs(mod_line);
                    if(!no_ws_line_view.empty() && no_ws_line_view.back() == ';') {
                        presets_section_end = true;
                    }
                    
                    string_view trimmed_line_view = trim(mod_line);
                    if(!trimmed_line_view.empty()) {
                        games.back().selected_mods.push_back(std::string(trimmed_line_view));
                    }
                }
            }
        } catch (std::exception& ex) {
            cerr << ex.what() << endl;
        }
    }
    return games;
}

DgContents Parser::ParseFile(const fs::path& dgc_path) {
    DgContents result;
    try {
        //NB: if file does not exist, write default file:
        if(!fs::exists(dgc_path)) {
            Write(_default_cfg);
            return {_default_cfg, {}};
        }

        //if path not provided, use default or given during construction
        std::fstream fs(dgc_path.empty() ? dgc_file_ : dgc_path, std::ios_base::in);
        std::string line;

        while(std::getline(fs, line)) {
            string_view no_whitespace_line_view = trimWs(line);
            //Skip empty lines and #comments
            if(no_whitespace_line_view.empty() || no_whitespace_line_view[0] == '#') {
                continue;
            }
            //Settings Section Label
            if(no_whitespace_line_view[0] == '[') {
                //Get rid of braces and extra whitespaces
                string_view trimmed_line_view = trim(no_whitespace_line_view);

                if(trimmed_line_view == LAST_STNGS_LABEL) {
                    result.settings = ReadLaunchPaths(fs);
                }
                else if(trimmed_line_view == MODSETS_LABEL) {
                    result.games = ParseModPresets(fs);
                }
            }

        }
    } catch (std::exception& ex) {
        cerr << ex.what() << endl;
    }
    return result;
}

void Parser::Write(const LaunchSettings& lcfg, const std::vector<ModSet>& presets) {
    try {

        fs::path folder = dgc_file_.parent_path();
        //create directories if they dont exist
        if(!fs::exists(folder)) {
            fs::create_directories(folder);
        }

        //overwrite existing file
        std::ofstream fs(dgc_file_, std::ios_base::trunc);
        if(!fs.is_open()) {
            throw std::runtime_error("Could not open dgc file for write!"s);
        }

        //Write config:
        fs << "[ "s << LAST_STNGS_LABEL << " ]\n"s;

        fs << indent() << WDIR_LABEL << ": " << lcfg.working_folder << '\n';
        fs << '\n';

        fs << indent() << IWADS_LABEL << ": " << lcfg.iwad_folder << '\n';
        fs << indent() << MODS_LABEL << ": " << lcfg.mod_folder << '\n';
        fs << indent() << GZDOOM_LABEL << ": " << lcfg.gzdoom_folder << '\n';
        fs << '\n';

        fs << indent() << GZDOOM_INI_LABEL << ": " << lcfg.gzdoom_ini_path << '\n';
        fs << indent() << GZDOOM_SAVES_LABEL << ": " << lcfg.gzdoom_saves << ';' << '\n';
        fs << '\n';

        if(presets.empty()) {
            return;
        }

        //Write presets:
        fs << "[ "s << MODSETS_LABEL << " ]\n"s;
        int counter = 0;
        for(const ModSet& game : presets) {
            fs << indent() << '#' << counter << '\n';
            fs << indent() << MODSET_LABEL << ": " << game.label << '\n';

            if(!game.iwad.empty()) {
                fs << indent() << MS_IWAD_LABEL << ": " << game.iwad << '\n';
            }

            if(!game.selected_mods.empty()) {
                fs << indent() << MS_MOD_LABEL << ": {\n";
                for(const string& mod : game.selected_mods) {
                    fs << indent(4) << mod << '\n';
                }
                fs << indent() << '}';
            }
        }
        fs << ";\n\n";
    }  catch (std::exception& ex) {
        cerr << ex.what() << endl;
    }
}


LaunchSettings Parser::MakeDefaults() {
    //    #ifdef WIN_BUILD
    //    return MakeDefaultsWin();
    //    #elif defined(MAC_BUILD)
    return MakeDefaultsMac();
    //===    #endif
}

LaunchSettings Parser::MakeDefaultsWin() {
    LaunchSettings parsed_settings;
    
    return parsed_settings;
}

LaunchSettings Parser::MakeDefaultsMac() {
    fs::path working_folder = "/Users/ps/Documents/My Games/ThePlasmaLauncher";
    LaunchSettings default_settings;
    
    default_settings.working_folder = working_folder;
    
    default_settings.mod_config_path = "mod_settings.dgc"s;

    default_settings.gzdoom_folder = "gzdoom";
    default_settings.gzdoom_ini_path = "gzdoom/gzdoom_portable_mac.ini";
    default_settings.gzdoom_saves = "Saves";

    default_settings.iwad_folder = "iwads";
    default_settings.mod_folder = "mods";

    return default_settings;
}


}//namespace dgc

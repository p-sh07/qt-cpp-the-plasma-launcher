//{
//  dgc-rw.cpp
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#include "dgc-rw.h"

namespace dgc {

using namespace std::literals;

ModSet::ModSet(std::string label, std::string iwad, std::vector<std::string> mod_filenames)
    : label(std::move(label))
    , iwad(std::move(iwad))
    , selected_mods(mod_filenames) {
}

//open default file if empty path
DgcParser::DgcParser(const fs::path settings_file)
    : file_(std::fstream(settings_file, std::ios_base::in))
    , _default_cfg(MakeDefaults()) {

    std::string line;
    std::getline(file_,line);
    if(line.empty()) {

    }
}

bool DgcParser::open(const fs::path& settings_file) {
    file_.open(settings_file);
    return file_.is_open();
}
LaunchSettings DgcParser::ParseLaunchSettings(const fs::path& gdc_path) {
    LaunchSettings s;



    return s;
}


std::vector<ModSet> DgcParser::ParsePresetsFromFile(const fs::path& gdc_path) {
        std::vector<ModSet> games;

    return games;
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

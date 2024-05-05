//
//  dgc-rw.cpp
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#include "dgc-rw.h"

namespace dgc {

ModSet::ModSet(std::string label, fs::path iwad = {}, std::vector<fs::path> mod_paths = {})
    : label(label), iwad(iwad), mod_paths(mod_paths) {}

//open default file if empty path
DgcParser::DgcParser(const fs::path settings_file)
    : file_(std::fstream(settings_file.empty() ? DFLT_DGC_FILE : settings_file)) {
}

bool DgcParser::open(const fs::path& settings_file) {
    file_.open(settings_file);
    return file_.is_open();
}

std::vector<ModSet> DgcParser::ParseFromFile() {
    using namespace std::literals;

    std::vector<ModSet> games;

    // if(!file_.is_open()) {
    //     throw std::runtime_error("filestream not open");
    // }

    //set mod configs for testing
    games.push_back({"None"});

    //0
    games.push_back(ModSet(
        //Label
        "Brutal DooM Sigil I&II"s,
        //iWad
        "DOOM.WAD"s,
        {//Mods
            "brutalv22test3.pk3",
            "SIGIL_v1_21.wad",
            "SIGIL_SHREDS.wad",
            "SIGIL_II_V1_0.WAD",
            "sigil2_ost_thorr.WAD",
            "D64TEX.PK3",
            "DoomMetalVol5.wad"
        }
        ));
    //1
    games.push_back(ModSet(
        //Label
        "Brutal DooM2"s,
        //iWad
        "DOOM2.WAD"s,
        {//Mods
            "brutalv22test3.pk3",
            "D64TEX.PK3",
            "DoomMetalVol5.wad"
        }
        ));
    //2
    games.push_back(ModSet(
        //Label
        "Brutal DooM BTSX I"s,
        //iWad
        "DOOM2.WAD"s,
        {//Mods
            "btsx_e1a.wad",
            "btsx_e1b.wad",
            "brutalv22test3.pk3"
        }
        ));
    //3
    games.push_back(ModSet(
        //Label
        "Brutal DooM BTSX II"s,
        //iWad
        "DOOM2.WAD"s,
        {//Mods
            "btsx_e2a.wad",
            "btsx_e2b.wad",
            "brutalv22test3.pk3"
        }
        ));
    //4
    games.push_back(ModSet(
        //Label
        "LostInDarkness"s,
        //iWad
        "DOOM.WAD"s,
        {//Mods
            "d2-in-d1-v3.pk3",
            "Lost_in_DarknessV2.0.23.pk3",
            "LiveReverb.pk3",
            "Rain_and_Snow.pk3",
            "D64TEX.PK3"
        }
        ));
    return games;
}

}//namespace dgc

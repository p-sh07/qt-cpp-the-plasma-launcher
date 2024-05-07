//
//  dgc-rw.cpp
//  gzdml
//
//  Created by ps on 02.05.2024.
//
#include "dgc-rw.h"

namespace dgc {

ModSet::ModSet(std::string label, std::string iwad, std::vector<std::string> mod_filenames)
    : label(std::move(label))
    , iwad(std::move(iwad))
    , selected_mods(mod_filenames) {
}

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
    games.push_back({"Default"});

    //0
    games.push_back(ModSet(
        //Label
        "Brutal DooM Sigil I&II"s,
        //iWad
        "DOOM.WAD"s,
        {//Mods
            "BrutalDoom.pk3",
            "Sigil_I.wad",
            "Sigil_I_ost_bckh.wad",
            "Sigil_II.wad",
            "Sigil2_ost_thorr.WAD",
            "DoomMetalVol5.wad"
        }
        ));
    //1
    games.push_back(ModSet(
        //Label
        "Brutal DooM 2"s,
        //iWad
        "DOOM2.WAD"s,
        {//Mods
            "BrutalDoom.pk3",
            "D64Textures.pk3",
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
            "BTSX_1a.wad",
            "BTSX_1b.wad",
            "BrutalDoom.pk3"
        }
        ));
    //3
    games.push_back(ModSet(
        //Label
        "Brutal DooM BTSX II"s,
        //iWad
        "DOOM2.WAD"s,
        {//Mods
            "BTSX_2a.wad",
            "BTSX_2b.wad",
            "BrutalDoom.pk3"
        }
        ));
    //4
    games.push_back(ModSet(
        //Label
        "LostInDarkness"s,
        //iWad
        "DOOM.WAD"s,
        {//Mods
            "LiD_for_dm1.pk3",
            "LostinDarknes.pk3",
            "LiveReverb.pk3",
            "Rain_and_Snow.pk3",
            "D64Textures.pk3"
        }
        ));
    return games;
}

}//namespace dgc

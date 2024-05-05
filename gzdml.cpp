#include "gzdml.h"

//================ Abstract Class - Interface ================//
GzdoomLauncher::GzdoomLauncher()
    : parser_(dgc::DgcParser())
    , games_(parser_.ParseFromFile()) {

    //init cfg paths
    MakeIwadLabels();
    MakeModLabels();

    //Create main directories if they dont exist;
    CreateDirIfDoesntExist(dgc::DFLT_GZDOOM);
    CreateDirIfDoesntExist(dgc::DFLT_IWADS_PATH);
    CreateDirIfDoesntExist(dgc::DFLT_MODWADS_PATH);
}

//There is always a default launch config present
void GzdoomLauncher::LaunchGame() {

    try {
        PerformLaunch();
    }
    catch (std::exception& ex) {
        cout << "Error during launch: " << ex.what() << endl;
    }
}

void GzdoomLauncher::SetModSet(size_t index) {
    cfg_.chosen_mod_set = index;

    //TODO:
    //Update all params in cfg:
    auto& new_mset = games_[index];
    //TODO: apply modset to cfg_
    //cfg_.

    MakeIwadLabels();

}
void GzdoomLauncher::SetIwad(size_t index) {
    cfg_.chosen_iwad = index;
    MakeIwadLabels();
}
void GzdoomLauncher::SetIwadDir(std::string dir) {
    cfg_.iwad_dir = dir;
    MakeIwadLabels();
}

QStringList GzdoomLauncher::GetModsetLabels()  {
    QStringList labels;

    for(const auto& modset : games_) {
        labels.append(QString::fromStdString(modset.label));
    }
    return labels;
}
QStringList GzdoomLauncher::MakeIwadLabels() {
    cfg_.iwad_paths.clear();

    for(const auto& filepath : fs::directory_iterator{cfg_.iwad_dir}) {
        if(fs::is_regular_file(filepath.status())) {
            cfg_.iwad_paths.push_back(filepath.path().filename());
        }
    }

    std::sort(cfg_.iwad_paths.begin(), cfg_.iwad_paths.end(), [] (const auto& lhs, const auto& rhs) {
        return lhs.string().compare(rhs.string()) < 0;
    });

    QStringList labels;
    for(const auto& label : cfg_.iwad_paths) {
        labels.append(QString::fromStdString(label.string()));
    }

    return labels;
}
QStringList GzdoomLauncher::MakeModLabels() {
    cfg_.mod_paths.clear();

    for(const auto& filepath : fs::directory_iterator{cfg_.mod_dir}) {
        if(fs::is_regular_file(filepath.status())) {
            cfg_.mod_paths.push_back(filepath.path());
        }
    }
    std::sort(cfg_.mod_paths.begin(), cfg_.mod_paths.end(), [] (const auto& lhs, const auto& rhs) {
        return lhs.string().compare(rhs.string()) < 0;
    });

    QStringList labels;
    for(const auto& label : cfg_.mod_paths) {
        labels.append(QString::fromStdString(label.filename().string()));
    }
    return labels;
}

void GzdoomLauncher::EnableMod(size_t mod_folder_index, bool enable) {
    auto& mfv = cfg_.chosen_mods;
    auto it = std::find(mfv.begin(), mfv.end(), mod_folder_index);

    if(enable && it == GetCfg().chosen_mods.end()) {
        mfv.push_back(mod_folder_index);
    }
    else if(!enable && it != GetCfg().chosen_mods.end()) {
        mfv.erase(it);
    }

}

std::vector<size_t> GzdoomLauncher::GetEnabledMods() {
    return cfg_.chosen_mods;
}

void GzdoomLauncher::SetEnabledMods(const std::vector<size_t>& enabled_mods) {
    cfg_.chosen_mods = enabled_mods;
}

std::string GzdoomLauncher::GetDisplayCmd() {
    std::string cmd;

    auto& mset = GetModset(GetCfg().chosen_mod_set);

    if(!mset.config.empty()) {
        cmd.append("CONFIG:  ").append(mset.config.filename().string());
    }
    cmd.append(  "\nIWAD:    ");
    if (GetCfg().chosen_mod_set == 0 || mset.iwad.empty()) {
        std::string iwad = GetIwad();
        cmd.append(iwad.empty() ? "***NOT FOUND***" : iwad);
    } else {
        cmd.append(mset.iwad);
    }

    if(!mset.mod_paths.empty()) {
        cmd.append(  "\nMODS:    ");
        for(const auto& mod : mset.mod_paths) {
            cmd.append(" [").append(mod).append("]");
        }
    } else if (!GetCfg().chosen_mods.empty()) {
        cmd.append(  "\nMODS:    ");
        for(const size_t mod_num : GetCfg().chosen_mods) {
            cmd.append(" [").append(GetCfg().mod_paths[mod_num].filename().string()).append("]");
        }
    }
    return cmd;
}

std::string GzdoomLauncher::GetLaunchCmd() {
    std::string cmd;

    auto& mset = GetModset(GetCfg().chosen_mod_set);

    if(!mset.config.empty()) {
        cmd.append(" -config ").append((fs::current_path() / mset.config).string());
    }
    cmd.append(" -iwad ");
    if (GetCfg().chosen_mod_set == 0 || mset.iwad.empty()) {
        cmd.append(GetIwad());
    } else {
        cmd.append(mset.iwad);
    }

    if(!mset.mod_paths.empty()) {
        cmd.append(" -file");
        for(const auto& mod : mset.mod_paths) {
            //cerr << "-mod: " << << endl;
            cmd.append(" ").append( (fs::current_path() / mod).string());
        }
    } else if (!GetCfg().chosen_mods.empty()) {
        cmd.append(" -file");
        for(const size_t mod_num : GetCfg().chosen_mods) {
            cmd.append(" ").append( (fs::current_path() / GetCfg().mod_paths[mod_num]).string() );
        }
    }
    return cmd;
}


void GzdoomLauncher::CreateDirIfDoesntExist(const fs::path& dir) const  {
    if(!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

//=================== MacOS Implementation ===================//
gzdml_mac_qt::gzdml_mac_qt()
    : GzdoomLauncher() {
}

//makes a launch command string from a DooM game config struct
std::string gzdml_mac_qt::MakeLaunchCommand() {

    std::string cmd = "open -g -a "
                      + (fs::current_path() / GetCfg().gzdoompath.string() / "gzdoom.app").string();

    cmd.append(" --args ");

    cmd.append(GetLaunchCmd());

    return cmd;
}

void gzdml_mac_qt::PerformLaunch() {
    std::string cmd = MakeLaunchCommand();
    cerr << "\n ->Performing launch with cmd [" << cmd << "]\n";

    std::system(cmd.data());
}







//=================== Windows Implementation ===================//
/*
gzdml_win_qt::gzdml_win_qt()
    : GzdoomLauncher() {
}

//makes a launch command string from a DooM game config struct
std::string gzdml_win_qt::MakeLaunchCommand(size_t index) {
    std::string launch_command;// = "-config ";

    if(index == 0) {
        return MakeDefaultCmd();
    }

    //launch_command.append(mset.config.string());
    auto& mset = GetModset(index);
    if (!mset.iwad.empty()) {
        launch_command.append(" -iwad ");
        launch_command.append( (mset.iwad_dir / mset.iwad).string()) ;
    }

    launch_command.append(" -file");

    for (const auto& mod : mset.mod_paths) {
        launch_command.append(" " + mset.mod_dir.string() + "/" + mod);
    }
    return launch_command;
}

void gzdml_win_qt::PerformLaunch(size_t index) {

}

    // std::string lcmd = MakeLaunchCommand(selected_mod_id_);
    // ShellExecuteW(NULL, NULL, gzdoomexe.c_str(), std::wstring(lcmd.begin(), lcmd.end()).c_str(), gzdoompath.c_str(), SW_SHOWDEFAULT);

    // STARTUPINFO si;
    // PROCESS_INFORMATION pi;

    // set the size of the structures
    // ZeroMemory(&si, sizeof(si));
    // si.cb = sizeof(si);
    // ZeroMemory(&pi, sizeof(pi));

    // std::string command = "-config user_config.txt -file mods\\brutalv22test3.pk3 mods\\SIGIL_v1_21.wad mods\\SIGIL_SHREDS.wad mods\\D64TEX.PK3 mods\\DoomMetalVol5.wad";

    // CreateProcess(
    //    gzdoomexe.c_str(),   // the path
    //    NULL,            // Command line
    //    NULL,           // Process handle not inheritable
    //    NULL,           // Thread handle not inheritable
    //    FALSE,          // Set handle inheritance to FALSE
    //    0,              // No creation flags
    //    NULL,           // Use parent's environment block
    //    NULL,           // Use parent's starting directory
    //    &si,            // Pointer to STARTUPINFO structure
    //    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    // );
    // // Close process and thread handles.
    // CloseHandle(pi.hProcess);
    // CloseHandle(pi.hThread);
}



//Tested - working - windows
//void PerformLaunch() {
    // std::string lcmd = MakeLaunchCommand(selected_mod_id_);
    // ShellExecuteW(NULL, NULL, gzdoomexe.c_str(), std::wstring(lcmd.begin(), lcmd.end()).c_str(), gzdoompath.c_str(), SW_SHOWDEFAULT);

    //STARTUPINFO si;
    //PROCESS_INFORMATION pi;

    // set the size of the structures
    //ZeroMemory(&si, sizeof(si));
    //si.cb = sizeof(si);
    //ZeroMemory(&pi, sizeof(pi));

    //std::string command = "-config user_config.txt -file mods\\brutalv22test3.pk3 mods\\SIGIL_v1_21.wad mods\\SIGIL_SHREDS.wad mods\\D64TEX.PK3 mods\\DoomMetalVol5.wad";

    //CreateProcess(
    //    gzdoomexe.c_str(),   // the path
    //    NULL,            // Command line
    //    NULL,           // Process handle not inheritable
    //    NULL,           // Thread handle not inheritable
    //    FALSE,          // Set handle inheritance to FALSE
    //    0,              // No creation flags
    //    NULL,           // Use parent's environment block
    //    NULL,           // Use parent's starting directory
    //    &si,            // Pointer to STARTUPINFO structure
    //    &pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
    //);
    //// Close process and thread handles.
    //CloseHandle(pi.hProcess);
    //CloseHandle(pi.hThread);

    //ALT, working:
    //ShellExecute(NULL, L"runas", gzdoomexe.c_str(), NULL, L"gzdoom", SW_SHOWDEFAULT);
    //ShellExecute(NULL, NULL, gzdoomexe.c_str(), NULL, L"gzdoom", SW_SHOWDEFAULT);

*/















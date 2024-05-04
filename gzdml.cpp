#include "gzdml.h"

//================ Abstract Class - Interface ================//
GzdoomLauncher::GzdoomLauncher()
    : parser_(dgc::DgcParser())
    , games_(parser_.ParseFromFile()) {

    //Create main directories if they dont exist;
    CreateDirIfDoesntExist(dgc::DFLT_GZDOOM);
    CreateDirIfDoesntExist(dgc::DFLT_IWADS_PATH);
    CreateDirIfDoesntExist(dgc::DFLT_MODWADS_PATH);
}

void GzdoomLauncher::AddNewModset(const fs::path& iwad, const std::vector<fs::path>& mods, const fs::path& config) {
    
}

void GzdoomLauncher::AddModToExistingSet(size_t mset_id, const fs::path& wad_or_pk3) {
    
}

//There is always a default launch config present
void GzdoomLauncher::LaunchGame(size_t index) {
    MakeLabels(mod_dir_);
    MakeLabels(iwad_dir_);


    try {
        PerformLaunch(index);
    }
    catch (std::exception& ex) {
        cout << "Error during launch: " << ex.what() << endl;
    }
}

void GzdoomLauncher::SetModSet(size_t index) {
    chosen_mod_set_ = index;
}
void GzdoomLauncher::SetIwad(size_t index) {
    chosen_iwad_ = index;
}

//METHODS
//looks for the gzdoom exe and sets the path variables
void GzdoomLauncher::InitGZDoomPaths() const {
    
}

//creates all the neccessary directories
void GzdoomLauncher::SetupFolders() const {
    
}


std::string GzdoomLauncher::MakeDefaultCmd() {
    std::string cmd = "open -g -a "+ (fs::current_path() / gzdoompath_.string() / "gzdoom.app").string() + " -iwad " + iwad_paths_[chosen_iwad_].string()
                      + " -file";
    for(const auto& mod_index : chosen_mods_) {
        cmd.append(" ").append(mod_paths_[mod_index]);
    }

    return cmd;
}


std::vector<std::string> GzdoomLauncher::MakeLabels(const fs::path& dir) {
    std::vector<std::string> result;
    for(const auto& filepath : fs::directory_iterator{dir}) {
        if(fs::is_regular_file(filepath.status())) {
            result.push_back(std::string(filepath.path().filename()));
            if(dir == mod_dir_) {
                mod_paths_.push_back(filepath.path());
            } else if (dir == iwad_dir_) {
                iwad_paths_.push_back(filepath.path());
            }
        }
    }
    return result;
}


std::vector<std::string> GzdoomLauncher::MakeModsetLabels() const {
    std::vector<std::string> labels;

    for(const auto& mset : games_) {
        labels.push_back(mset.label);
    }

    return labels;
}
        //using ranges:
    /*
        std::ranges::for_each(
            std::filesystem::directory_iterator{mod_dir_},
            [&](const auto& dir_entry) {
                if(fs::is_regular_file(dir_entry.status()) ) {
                    result.push_back(std::string(dir_entry.path()));
                }
        });*/ //- weird

void GzdoomLauncher::CreateDirIfDoesntExist(const fs::path& dir) const  {
    if(!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}


QStringList GzdoomLauncher::GetModsetLabels()  {
    QStringList labels;
    for(const auto& lbl : GzdoomLauncher::MakeModsetLabels()) {
        labels.append(QString::fromStdString(std::string(lbl)));
    }
    return labels;
}

QStringList GzdoomLauncher::GetIwadLabels() {
    QStringList labels;
    for(const auto& lbl : GzdoomLauncher::MakeLabels(iwad_dir_)) {
        labels.append(QString::fromStdString(lbl));
    }
    return labels;
}

QStringList GzdoomLauncher::GetModLabels() {
    QStringList labels;
    for(const auto& lbl : GzdoomLauncher::MakeLabels(mod_dir_)) {
        labels.append(QString::fromStdString(lbl));
    }
    return labels;
}



//=================== Windows Implementation ===================//
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

    for (const auto& mod : mset.mod_filenames) {
        launch_command.append(" " + mset.mod_dir.string() + "/" + mod);
    }
    return launch_command;
}

void gzdml_win_qt::PerformLaunch(size_t index) {

}

/*
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

//=================== Windows Implementation ===================//
gzdml_mac_qt::gzdml_mac_qt()
    : GzdoomLauncher() {
}

//makes a launch command string from a DooM game config struct
std::string gzdml_mac_qt::MakeLaunchCommand(size_t index) {
    std::string launch_command;

    // if(index == 0) {
    //     return MakeDefaultCmd();
    // }

    auto& mset = GetModset(index);

    if(!mset.config.empty()) {
        launch_command.append(" -config ").append(mset.config.string());
    }

    if (mset.iwad.empty()) {
         launch_command.append(" -iwad ").append(iwad_paths_[chosen_iwad_].string());
    } else {
       launch_command.append(" -iwad ").append(mset.iwad_dir / mset.iwad);
    }

    launch_command.append(" -file");

    for (const auto& mod : mset.mod_filenames) {
        launch_command.append(" " + mset.mod_dir.string() + "/" + mod);
    }
    return launch_command;
}

void gzdml_mac_qt::PerformLaunch(size_t index) {

    const dgc::ModSet& ms = GetModset(index);

    const std::string cmd = "open -g -a "+ (fs::current_path() / gzdoompath_.string() / "gzdoom.app").string() + MakeLaunchCommand(index);
    cerr << "/n ->Cmd send: " << cmd << endl;
    std::system(cmd.data());
}





















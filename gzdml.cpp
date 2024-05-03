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
void GzdoomLauncher::LaunchGame() {

    try {
        PerformLaunch(games_.at(chosen_mod_set_));
    }
    catch (std::exception& ex) {
        cout << "Error during launch: " << ex.what() << endl;
    }
}

//METHODS
//looks for the gzdoom exe and sets the path variables
void GzdoomLauncher::InitGZDoomPaths() {
    
}

//creates all the neccessary directories
void GzdoomLauncher::SetupFolders() {
    
}

std::string GzdoomLauncher::MakeLaunchCommand(const dgc::ModSet& mset) const {
    std::string launch_command = "-config ";
    launch_command.append(mset.config.string());

    if (!mset.iwad.empty()) {
        launch_command.append(" -iwad ");
        launch_command.append(mset.iwad.string());
    }

    launch_command.append(" -file");

    for (const auto& mod : mset.mod_filenames) {
        launch_command.append(" " + mset.mod_dir.string() + "\\" + mod);
    }
    return launch_command;
}


std::vector<std::string> GzdoomLauncher::MakeLabels(const fs::path& dir) {
    std::vector<std::string> result;
    for(const auto& filepath : fs::directory_iterator{dir}) {
        if(fs::is_regular_file(filepath.status())) {
            result.push_back(std::string(filepath.path().filename()));
        }
    }
    return result;
}


std::vector<std::string_view> GzdoomLauncher::MakeModsetLabels() {
    std::vector<std::string_view> labels;

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

void GzdoomLauncher::CreateDirIfDoesntExist(const fs::path& dir) {
    if(!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}


//=================== Windows Implementation ===================//
gzdml_win_qt::gzdml_win_qt()
    : GzdoomLauncher() {
}

//makes a launch command string from a DooM game config struct
std::string gzdml_win_qt::MakeLaunchCommand(const dgc::ModSet& mset) const {
    return GzdoomLauncher::MakeLaunchCommand(mset);
}

void gzdml_win_qt::PerformLaunch(const dgc::ModSet& ms) const {
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


QStringList gzdml_win_qt::GetModsetLabels() {
    QStringList labels;
    for(const auto& lbl : GzdoomLauncher::MakeModsetLabels()) {
        labels.append(QString::fromStdString(std::string(lbl)));
    }
    return labels;
}

QStringList gzdml_win_qt::GetIwadLabels() {
    QStringList labels;
    for(const auto& lbl : GzdoomLauncher::MakeLabels(iwad_dir_)) {
        labels.append(QString::fromStdString(lbl));
    }
    return labels;
}

QStringList gzdml_win_qt::GetModLabels() {
    QStringList labels;
    for(const auto& lbl : GzdoomLauncher::MakeLabels(mod_dir_)) {
        labels.append(QString::fromStdString(lbl));
    }
    return labels;
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

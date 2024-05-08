#include "gzdml.h"

#include <cctype>
//=============== Sort Strings or Paths ===============//
namespace {
bool icharEquals(char a, char b)
{
    return std::tolower(static_cast<unsigned char>(a)) ==
           std::tolower(static_cast<unsigned char>(b));
}
bool icharLess(char a, char b)
{
    return std::tolower(static_cast<unsigned char>(a)) <
           std::tolower(static_cast<unsigned char>(b));
}
bool strIsLess(std::string_view lhs, std::string_view rhs)
{
    for(size_t i = 0; i < std::min(lhs.size(), rhs.size()); ++i) {
        if(!icharEquals(lhs[i], rhs[i])) {
             return icharLess(lhs[i], rhs[i]);
        }
    } //if all symbols equal, return shorter str first
    return lhs.size() < rhs.size();
}

bool pathFnameIsLess(const fs::path& lhs, const fs::path& rhs)
{
    return strIsLess(lhs.filename().string(), rhs.filename().string());
}

}

//================================================================//
//================ GzdoomLauncher - Interface ================//
GzdoomLauncher::GzdoomLauncher(const log::FileLogger& logger)
    : __log(logger)
    , parser_(dgc::DgcParser())
    , games_(parser_.ParseFromFile()) {


    __log << "\n\n***** New Launch *****\n";
    __log << "GZDML: construction Started";

    try {
    __log << "GZDML: Starting initLaunchscfg()";

    //init cfg paths
    InitLaunchConfig();
    } catch(std::exception& ex) {
    __log << "*GZDML ERROR*: " + std::string(ex.what());
    }
    __log << "GZDML: construction Finished\n";
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

void GzdoomLauncher::InitLaunchConfig (const fs::path& mod_folder, const fs::path& gzdoom_folder, const fs::path& iwad_folder) {

    //TODO: Adding iwad dir to gzdoom_config.ini!

    const fs::path working_dir = "/Users/ps/Documents/GZDoom";

    __log << "GZDML Starting initLaunchscfg()";

    //TODO: Create/Read settings file? -> constructor?

    lcfg_.mod_folder = working_dir / mod_folder;
    lcfg_.iwad_folder = working_dir / iwad_folder;
    lcfg_.gzdoom_folder = "/Applications/";

    __log << "Init paths as: \n Mods = " + lcfg_.mod_folder.string()
                 + "\n Iwad = " + lcfg_.iwad_folder.string()
                 + "\n Gzdm = " + lcfg_.gzdoom_folder.string();

    CreateDirIfDoesntExist(lcfg_.mod_folder);
    CreateDirIfDoesntExist(lcfg_.iwad_folder);
    CreateDirIfDoesntExist(lcfg_.gzdoom_folder);

    //default gzdoom config:
    lcfg_.gzdoom_ini_path = working_dir / "gzdoom_mac.ini";

    __log << "Using ini = " + lcfg_.gzdoom_ini_path.string();

    lcfg_.mod_filenames = GetSortedFilenames(lcfg_.mod_folder);
    lcfg_.iwad_filenames = GetSortedFilenames(lcfg_.iwad_folder);

    __log << "Got sorted filenames successfully";
}

void GzdoomLauncher::InitFromFile(const fs::path dgc_file) {

}

bool GzdoomLauncher::IsFirstLaunch() {
    //if the settings file exists, then it's not the first launch
    return !fs::exists(dgc::DFLT_DCG_PATH);
}

//------------------ Get & Set Methods ------------------//
// ---- Labels ----
QStringList GzdoomLauncher::GetModSetLabels()  {
    QStringList labels;

    for(const auto& modset : games_) {
        labels.append(QString::fromStdString(modset.label));
    }

    if(labels.empty()) {
        labels.append(QString::fromStdString("*EMPTY*"));
    }

    return labels;
}

QStringList GzdoomLauncher::MakeModListLabels() {
    QStringList labels;
    for(const std::string& fname : lcfg_.mod_filenames) {
        labels.append(QString::fromStdString(fname));
    }

    if(labels.empty()) {
        labels.append(QString::fromStdString("*EMPTY*: Please Add Mods to folder:\n" + lcfg_.mod_folder.string()));
    }

    return labels;
}

QStringList GzdoomLauncher::MakeIwadLabels() {
    QStringList labels;
    for(const std::string& fname : lcfg_.iwad_filenames) {
        labels.append(QString::fromStdString(fname));
    }

    if(labels.empty()) {
        labels.append(QString::fromStdString("*EMPTY*: Please Add IWADS to folder:\n " + lcfg_.iwad_folder.string()));
    }

    return labels;
}

//---- Iwad ----
std::string GzdoomLauncher::GetIwad() {
    if(lcfg_.iwad_filenames.empty() || lcfg_.iwad_filenames.size() <= lcfg_.chosen_iwad) {
        return "!!! IWAD NOT FOUND !!!";
    }

    return lcfg_.iwad_filenames[lcfg_.chosen_iwad];
}

size_t GzdoomLauncher::GetIwadIndex() {
    return lcfg_.chosen_iwad;
}

void GzdoomLauncher::SetIwad(size_t index) {
    //TODO: upd to cfg/modset
    lcfg_.chosen_iwad = index;
}

void GzdoomLauncher::SetIwadDir(std::string dir) {
    lcfg_.iwad_folder = dir;
    lcfg_.iwad_filenames = GetSortedFilenames(lcfg_.iwad_folder);
}

//---- Mods ----
void GzdoomLauncher::SetMod(size_t mod_folder_index, bool enable) {
    //Check if the mod is currently enabled
    auto it = std::ranges::find(lcfg_.chosen_mods, mod_folder_index);
    //Disable or enable the mod
    if(enable && it == lcfg_.chosen_mods.end()) {
        lcfg_.chosen_mods.push_back(mod_folder_index);
    }
    else if(!enable && it != lcfg_.chosen_mods.end()) {
        lcfg_.chosen_mods.erase(it);
    }
}

const std::vector<size_t>& GzdoomLauncher::GetEnabledMods() {
    return lcfg_.chosen_mods;
}

void GzdoomLauncher::SetEnabledMods(const std::vector<size_t>& enabled_mods) {
    lcfg_.chosen_mods = enabled_mods;
}

//---- ModSet ------

void GzdoomLauncher::ApplyChanges() {
    if(lcfg_.chosen_mod_set == 0) {
        return;
    }
    auto& iwad = games_[lcfg_.chosen_mod_set].iwad;
    iwad = lcfg_.iwad_filenames[lcfg_.chosen_iwad];

    auto& selected_mods_vec = games_[lcfg_.chosen_mod_set].selected_mods;
    selected_mods_vec.clear();

    for(const size_t mod_num : lcfg_.chosen_mods) {
        selected_mods_vec.push_back(lcfg_.mod_filenames[mod_num]);
    }
}

void GzdoomLauncher::AddNewModSet() {
    //TODO
}

void GzdoomLauncher::SetModSet(size_t index) {
    if(index >= games_.size()) {
        //TODO: throw
        cerr << "Index is bigger than ModSet size()\n";
        return;
    }
    //ModSet
    lcfg_.chosen_mod_set = index;
    auto& modset = games_[index];

    const auto cp = fs::current_path();
    //IWAD
    if(!modset.iwad_dir.empty() && (cp / modset.iwad_dir) != lcfg_.iwad_folder) {
        lcfg_.iwad_folder = modset.iwad_dir;
        lcfg_.iwad_filenames = GetSortedFilenames(lcfg_.iwad_folder);
    }
    if(modset.iwad.empty()) {
        cerr << "passing empty str modset.iwad in SetMOdSet()!\n";
    }
    lcfg_.chosen_iwad = GetIndexInVec(modset.iwad, lcfg_.iwad_filenames);

    //Mods
    if(!modset.mod_dir.empty() && (cp / modset.mod_dir) != lcfg_.mod_folder) {
        lcfg_.mod_folder = modset.mod_dir;
        lcfg_.mod_filenames = GetSortedFilenames(lcfg_.mod_folder);
    }

    for(const auto& mod : modset.selected_mods) {
        if(mod.empty()) {
            cerr << "passing empty str _mod_ in SetMOdSet()!\n";
        }
        lcfg_.chosen_mods.push_back(GetIndexInVec(mod, lcfg_.mod_filenames));
    }

    //TODO: Config, etc.
}

const dgc::ModSet& GzdoomLauncher::GetModSet() {
    return games_[lcfg_.chosen_mod_set];
}

//---- Launch Command ------
std::string GzdoomLauncher::GetDisplayCmd() {
    std::string cmd;
    //Add config, if specified:
    if(!lcfg_.gzdoom_ini_path.empty()) {
        cmd.append("CONFIG: ").append(lcfg_.gzdoom_ini_path.filename().string());
    }
    //IWAD:
    cmd.append(  "\nIWAD:  " + GetIwad());
    //MODS:
    if (!lcfg_.chosen_mods.empty()) {
        cmd.append(  "\nMODS:  ");
        for(const size_t mod_num : lcfg_.chosen_mods) {
            cmd.append(" [" + lcfg_.mod_filenames[mod_num] + "]");
        }
    }
    return cmd;
}

std::string GzdoomLauncher::GetCurrentConfigStr() {
    std::string cmd;

    if(!lcfg_.gzdoom_ini_path.empty()) {
        cmd.append(" -config " + lcfg_.gzdoom_ini_path.string());
    }
    cmd.append(" -iwad " + GetIwad());

    if (!lcfg_.chosen_mods.empty()) {
        cmd.append(" -file");
        for(const size_t mod_num : lcfg_.chosen_mods) {
            //TODO: insert current path here?
            cmd.append(" " + (lcfg_.mod_folder / lcfg_.mod_filenames[mod_num]).string() );
        }
    }
    return cmd;
}


//------------------ Private Helper Methods ------------------//
std::vector<std::string> GzdoomLauncher::GetSortedFilenames(const fs::path dir) {
    std::vector<std::string> filenames;

    for(const auto& filepath : fs::directory_iterator{dir}) {
        if(fs::is_regular_file(filepath.status())) {
            filenames.push_back(filepath.path().filename());
        }
    }
    std::sort(filenames.begin(), filenames.end(), strIsLess);
    return filenames;
}

void GzdoomLauncher::CreateDirIfDoesntExist(const fs::path& dir) const  {
    if(!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

size_t GzdoomLauncher::GetIndexInVec(std::string file, const std::vector<std::string>& file_names) {
    //TODO: Handle this error

    auto it = std::find(file_names.begin(), file_names.end(), file);
    if(file.empty()) {
        cerr << "***passing empty file\n";
    }
    if(it == file_names.end()) {
        //TODO throw
        cerr << "**Index search error !! for file: " << file << '\n';
    }
    return it - file_names.begin();
}

//+++++++++++++++++++ Eof GzdoomLauncher Class +++++++++++++++++++//
//================================================================//



//=================== MacOS Implementation ===================//

//makes a launch command string from a DooM game config struct
std::string MacGzdml::MakeLaunchCommand() {
    auto& lcfg = GetLaunchConfig();
    std::string cmd = "open -g -a "
    + (lcfg.gzdoom_folder / lcfg.gzdoom_app).string()
    + " --args " + GetCurrentConfigStr();

    return cmd;
}

void MacGzdml::PerformLaunch() {
    std::string cmd = MakeLaunchCommand();
    cerr << "\n ->Performing launch with cmd [" << cmd << "]\n";

    std::system(cmd.data());
}

//+++++++++++++++++++ Eof gzdml_mac_qt Class +++++++++++++++++++//
//==============================================================//


//=================== Windows Implementation ===================//
void WinGzdml::PerformLaunch() {
//First method:

    // std::string lcmd = GetCurrentConfigStr();
    // ShellExecuteW(NULL, NULL, gzdoomexe.c_str(), std::wstring(lcmd.begin(), lcmd.end()).c_str(), gzdoompath.c_str(), SW_SHOWDEFAULT);

//2nd method:
    // STARTUPINFO si;
    // PROCESS_INFORMATION pi;

    // // set the size of the structures
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

//ALT, working:
//ShellExecute(NULL, L"runas", gzdoomexe.c_str(), NULL, L"gzdoom", SW_SHOWDEFAULT);
//ShellExecute(NULL, NULL, gzdoomexe.c_str(), NULL, L"gzdoom", SW_SHOWDEFAULT);
}

//+++++++++++++++++++ Eof gzdml_win_qt Class +++++++++++++++++++//
//==============================================================//











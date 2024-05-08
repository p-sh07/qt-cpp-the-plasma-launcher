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
GzdoomLauncher::GzdoomLauncher()
    : parser_(dgc::DgcParser())
    , games_(parser_.ParseFromFile()) {

    //Create main directories if they dont exist;
    CreateDirIfDoesntExist(dgc::DFLT_GZDOOM);
    CreateDirIfDoesntExist(dgc::DFLT_IWADS_PATH);
    CreateDirIfDoesntExist(dgc::DFLT_MODWADS_PATH);

    //init cfg paths
    InitLaunchConfig();
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
    const fs::path cp = fs::current_path();

    //TODO: Create/Read settings file? -> constructor?

    lcfg_.mod_folder = cp / mod_folder;
    lcfg_.iwad_folder = cp / iwad_folder;
    lcfg_.gzdoom_folder = cp / gzdoom_folder;

    //default gzdoom config:
    lcfg_.gzdoom_ini_path = cp / "gzdoom_portable.ini";

    lcfg_.mod_filenames = GetSortedFilenames(lcfg_.mod_folder);
    lcfg_.iwad_filenames = GetSortedFilenames(lcfg_.iwad_folder);
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
    return labels;
}

QStringList GzdoomLauncher::MakeModListLabels() {
    QStringList labels;
    for(const std::string& fname : lcfg_.mod_filenames) {
        labels.append(QString::fromStdString(fname));
    }
    return labels;
}

QStringList GzdoomLauncher::MakeIwadLabels() {
    QStringList labels;
    for(const std::string& fname : lcfg_.iwad_filenames) {
        labels.append(QString::fromStdString(fname));
    }
    return labels;
}

//---- Iwad ----
std::string GzdoomLauncher::GetIwad() {
    if(lcfg_.iwad_filenames.empty()) {
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
    lcfg_.chosen_iwad = GetIndexInVec(modset.iwad, lcfg_.iwad_filenames);

    //Mods
    if(!modset.mod_dir.empty() && (cp / modset.mod_dir) != lcfg_.mod_folder) {
        lcfg_.mod_folder = modset.mod_dir;
        lcfg_.mod_filenames = GetSortedFilenames(lcfg_.mod_folder);
    }

    for(const auto& mod : modset.selected_mods) {
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
    cmd.append(  "\nIWAD:  " + (lcfg_.iwad_filenames.empty() ? "Folder Empty" : lcfg_.iwad_filenames[lcfg_.chosen_iwad]));
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
    cmd.append(" -iwad " + lcfg_.iwad_filenames[lcfg_.chosen_iwad]);

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
            filenames.push_back(filepath.path().filename().string());
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
    auto it = std::find(file_names.begin(), file_names.end(), file);
    if(it == file_names.end()) {
        //TODO throw
        cerr << "**Index search error !! for file: " << file << '\n';
    }
    return it - file_names.begin();
}

//+++++++++++++++++++ Eof GzdoomLauncher Class +++++++++++++++++++//
//================================================================//



//=================== MacOS Implementation ===================//
MacGzdml::MacGzdml()
    : GzdoomLauncher() {
}

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
WinGzdml::WinGzdml()
    : GzdoomLauncher() {
}

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











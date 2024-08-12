
#include "gzdml.h"

#include <algorithm>

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
    //TODO: for some reason std::min broke
    size_t min_size = lhs.size() < rhs.size() ? lhs.size() :rhs.size();
    for(size_t i = 0; i < min_size; ++i) {
        if(!icharEquals(lhs[i], rhs[i])) {
             return icharLess(lhs[i], rhs[i]);
        }
    } //if all symbols equal, return shorter str first
    return lhs.size() < rhs.size();
}
}

//================================================================//
//================ GzdoomLauncher - Interface ================//
GzdoomLauncher::GzdoomLauncher(const fs::path dgc_file)
    : games_(1, {"Default"}) //make 1 "default" game setting
{

    using namespace std::literals;

    try {
        if(fs::exists(dgc_file)) {
            InitFromFile(dgc_file);
        }
        else {
            lcfg_ = parser_.GetDefaults();
            //Create settings file:
            if(!fs::exists(lcfg_.working_folder)) {
                fs::create_directories(lcfg_.working_folder);
            }
            CreateFolders();

            parser_.Write(lcfg_.doom_game_config, lcfg_);
        }
        //init cfg paths
        InitPaths();
    } catch(std::exception& ex) {

    }
}

//There is always a default launch config present
void GzdoomLauncher::LaunchGame() {

    try {
        PerformLaunch();
    }
    catch (std::exception& ex) {
        //cout << "Error during launch: " << ex.what() << endl;
    }
}

//TODO:
void GzdoomLauncher::InitPaths () {
    lcfg_.mod_filenames = GetSortedFilenames(lcfg_.mod_folder);
    lcfg_.iwad_filenames = GetSortedFilenames(lcfg_.iwad_folder);
}

void GzdoomLauncher::InitFromFile(fs::path dgc_file) {
    auto file_data = parser_.ParseFile(dgc_file);

    lcfg_ = std::move(file_data.settings);

    if(!file_data.games.empty()) {
        games_ = std::move(file_data.games);
    }

    InitPaths();
}

bool GzdoomLauncher::IsFirstLaunch() {
    //if the settings file exists, then it's not the first launch
    return is_first_launch_;
}

void GzdoomLauncher::CreateFolders() {
    try{
        CreateDirIfDoesntExist(lcfg_.mod_folder);
        CreateDirIfDoesntExist(lcfg_.iwad_folder);
        CreateDirIfDoesntExist(lcfg_.gzdoom_folder);
    }
    catch (std::exception& ex) {

    }
}

void GzdoomLauncher::SetWorkingDir(fs::path wdir) {
    lcfg_.working_folder = wdir;

    CreateFolders();
    InitPaths();
}

//------------------ Get & Set Methods ------------------//


fs::path GzdoomLauncher::GetDefaultWorklingDir() {
    return fs::current_path();
}

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
        labels.append(QString::fromStdString("*EMPTY*: " + lcfg_.mod_folder.string()));
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
}

const dgc::ModSet& GzdoomLauncher::GetModSet() {
    return games_[lcfg_.chosen_mod_set];
}

//---- Launch Command ------
std::string GzdoomLauncher::GetDisplayCmd() {
    std::string cmd;

    //Add config, if specified:
    if(!lcfg_.gzdoom_ini_path.empty()) {
        cmd.append("INI: ").append(lcfg_.gzdoom_ini_path.filename().string());
    }

    //IWAD:
    cmd.append(  "\nIWAD: " + GetIwad());

    //MODS:
    if (!lcfg_.chosen_mods.empty() && !lcfg_.mod_filenames.empty()) {
        cmd.append(  "\nMODS: ");
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
std::vector<std::string> GzdoomLauncher::GetSortedFilenames(fs::path dir) {
    std::vector<std::string> filenames;

    for(const auto& filepath : fs::directory_iterator{dir}) {
        if(fs::is_regular_file(filepath.status())) {
            filenames.push_back(filepath.path().filename().string());
        }
    }
    std::sort(filenames.begin(), filenames.end(), strIsLess);
    return filenames;
}

void GzdoomLauncher::CreateDirIfDoesntExist(fs::path dir) const  {
    if(!fs::exists(dir)) {
        fs::create_directory(dir);
    }
}

size_t GzdoomLauncher::GetIndexInVec(std::string file, const std::vector<std::string>& file_names) {
    auto it = std::find(file_names.begin(), file_names.end(), file);

    if(it == file_names.end()) {
        throw std::out_of_range("mod file not found");
    }
    return it - file_names.begin();
}

//+++++++++++++++++++ Eof GzdoomLauncher Class +++++++++++++++++++//
//================================================================//



//=================== MacOS Implementation ===================//
void MacGzdml::PerformLaunch() {
    std::string lcmd = MakeLaunchCommand();
    std::system(lcmd.data());
}

//makes a launch command string from a DooM game config struct
std::string MacGzdml::MakeLaunchCommand() {
    auto& lcfg = GetLaunchConfig();
    std::string cmd = "open -g -a "
                      + (lcfg.gzdoom_folder / "gzdoom_app").string()
                      + " --args " + GetCurrentConfigStr();

    return cmd;
}

//+++++++++++++++++++ Eof gzdml_mac_qt Class +++++++++++++++++++//
//==============================================================//


//=================== Windows Implementation ===================//
void WinGzdml::PerformLaunch() {
#ifdef WIN_BUILD
    std::string lcmd = GetCurrentConfigStr();

ShellExecuteW(NULL, NULL, LPCWSTR("gzdoom.exe"), std::wstring(lcmd.begin(), lcmd.end()).c_str(), GetLaunchConfig().gzdoom_folder.c_str(), SW_SHOWDEFAULT);
#endif
}

//+++++++++++++++++++ Eof gzdml_win_qt Class +++++++++++++++++++//
//==============================================================//











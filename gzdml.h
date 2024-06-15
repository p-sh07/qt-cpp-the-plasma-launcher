#pragma once

#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

#include <QString>
#include <QStringList>

#ifdef WIN_BUILD
#include <windows.h>
#include <shlobj.h>
//#include <stringapiset.h>
//#include <Atlconv.h>
#endif

#include "parser.h"
#include "logger.h"

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;

namespace fs = std::filesystem;

//================================================================//
//================ GzdoomLauncher - Interface/Abstract Class =====//
class GzdoomLauncher
{
public:
    GzdoomLauncher(std::shared_ptr<_log::FileLogger>   logger, fs::path dgc_file = {});
    virtual ~GzdoomLauncher() = default;

//------------------ Public Methods ------------------//
    void LaunchGame();
    void InitFromFile(fs::path dgc_file);
    void SetWorkingDir(fs::path wdir);
    void InitPaths();
    bool IsFirstLaunch();
    void CreateFolders();

//------------------ Get & Set Methods ------------------//
    fs::path GetDefaultWorklingDir();

// ---- Labels ----
    QStringList GetModSetLabels();
    QStringList MakeModListLabels();
    QStringList MakeIwadLabels();

//---- Iwad ----
    std::string GetIwad();
    size_t GetIwadIndex();
    void SetIwad(size_t index);
    void SetIwadDir(std::string dir);

//---- Mods ----
    void SetMod(size_t mod_index, bool enable);
    const std::vector<size_t>& GetEnabledMods();
    void SetEnabledMods(const std::vector<size_t>& enabled_mods);

//---- ModSet ------
    void ApplyChanges();
    void AddNewModSet();
    void SetModSet(size_t index);
    const dgc::ModSet& GetModSet();

//---- Launch Command ------
    std::string GetDisplayCmd();
    std::string GetCurrentConfigStr();

//---- Launch Configuration Settings ------
    inline const dgc::LaunchSettings& GetLaunchConfig() { return lcfg_; }

protected:
    std::shared_ptr<_log::FileLogger> __log = nullptr;
//---- Get config ref for modifying - only for child classes ------
    dgc::LaunchSettings& ModLcfg() { return lcfg_; }
private:
//------------------ Private Vars ------------------//
    bool is_first_launch_ = true;
    std::vector<dgc::ModSet> games_;
    dgc::Parser parser_;
    dgc::LaunchSettings lcfg_;

//------------------ Private Methods ------------------//
    std::vector<std::string> GetSortedFilenames(const fs::path dir);
    void CreateDirIfDoesntExist(fs::path dir) const;
    size_t GetIndexInVec(std::string file, const std::vector<std::string>& file_names);


//---- Virtual Methods for OS-specific Implementation ------
    virtual void PerformLaunch() = 0;
    //virtual fs::path GetUserDocumentsPath() = 0;
};
//+++++++++++++++++++ Eof gzdml_mac_qt Class +++++++++++++++++++//
//==============================================================//


//================================================================//
//================ MacOS implementation ================//
class MacGzdml final : public GzdoomLauncher {
public:
    using GzdoomLauncher::GzdoomLauncher;
    ~MacGzdml() override = default;

private:
    void PerformLaunch() override;
    //fs::path GetUserDocumentsPath() override;
    std::string MakeLaunchCommand();
};
//+++++++++++++++++++ Eof gzdml_mac_qt Class +++++++++++++++++++//
//==============================================================//


//================================================================//
//================ Windows Implementation ================//
class WinGzdml final : public GzdoomLauncher {
public:
    WinGzdml(const _log::FileLogger& logger, fs::path working_folder);
    ~WinGzdml() override = default;


private:
    void PerformLaunch() override;
    //fs::path GetUserDocumentsPath() override;
};
//+++++++++++++++++++ Eof gzdml_win_qt Class +++++++++++++++++++//
//==============================================================//


#pragma once

#include <windows.h>
//#include <Atlconv.h>


#include <cstdlib>
#include <fstream>
#include <iostream>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>

#include <unordered_set>
#include <unordered_map>

#include <vector>

#include <QString>
#include <QStringList>

#include "dgc-rw.h"
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
    GzdoomLauncher(const _log::FileLogger& logger);
    virtual ~GzdoomLauncher() = default;

//------------------ Public Methods ------------------//
    void LaunchGame();
    void InitLaunchConfig(const fs::path& mod_folder = dgc::DFLT_MODWADS_PATH, const fs::path& gzdoom_folder = dgc::DFLT_GZDOOM, const fs::path& iwad_folder = dgc::DFLT_IWADS_PATH);
    void InitFromFile(const fs::path dgc_file);
    bool IsFirstLaunch();

//------------------ Get & Set Methods ------------------//
    GzdoomLauncher(const fs::path& gzdoom_dir);

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
    const _log::FileLogger& __log;
//---- Get config ref for modifying - only for child classes ------
    dgc::LaunchSettings& ModLcfg() { return lcfg_; }
private:
//------------------ Private Vars ------------------//
    dgc::LaunchSettings lcfg_;
    dgc::DgcParser parser_;
    std::vector<dgc::ModSet> games_{1};

//------------------ Private Methods ------------------//
    std::vector<std::string> GetSortedFilenames(const fs::path dir);
    void CreateDirIfDoesntExist(const fs::path& dir) const;
    size_t GetIndexInVec(std::string file, const std::vector<std::string>& file_names);


//---- Virtual Methods for OS-specific Implementation ------
    virtual void PerformLaunch() = 0;
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
    std::string MakeLaunchCommand();
};
//+++++++++++++++++++ Eof gzdml_mac_qt Class +++++++++++++++++++//
//==============================================================//


//================================================================//
//================ Windows Implementation ================//
class WinGzdml final : public GzdoomLauncher {
public:
    using GzdoomLauncher::GzdoomLauncher;
    ~WinGzdml() override = default;

private:
    void PerformLaunch() override;
};
//+++++++++++++++++++ Eof gzdml_win_qt Class +++++++++++++++++++//
//==============================================================//


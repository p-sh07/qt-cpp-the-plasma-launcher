#pragma once

//#include <windows.h>
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

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;

namespace fs = std::filesystem;
namespace dgc {
struct Cfg {
    inline const static fs::path gzdoomexe = "gzdoom.exe";

    fs::path gzdoompath     = dgc::DFLT_GZDOOM;
    fs::path gzdoomexe_full = gzdoompath / gzdoomexe;
    fs::path gzdoom_config  = dgc::DFLT_CONFIG;
    fs::path mod_config     = dgc::DFLT_DCG_PATH;

    fs::path iwad_dir       = dgc::DFLT_IWADS_PATH;
    fs::path mod_dir        = dgc::DFLT_MODWADS_PATH;

    size_t chosen_iwad = 0;
    size_t chosen_mod_set = 0;
    std::vector<size_t> chosen_mods;

    std::vector<fs::path> iwad_paths;
    std::vector<fs::path> mod_paths;
};
}

class GzdoomLauncher
{
public:
    GzdoomLauncher();
    virtual ~GzdoomLauncher() = default;

    GzdoomLauncher(const fs::path& gzdoom_dir);

    void ApplyChanges() {
        if(GetCfg().chosen_mod_set == 0) {
            return;
        }
        auto& mods_vec = games_[GetCfg().chosen_mod_set].mod_paths;
        mods_vec.clear();

        for(const size_t mod_num : GetCfg().chosen_mods) {
            mods_vec.push_back(GetCfg().mod_paths[mod_num]);
        }
    }
    void AddNewModset() {

    }
    void AddModToCurrentSet() {

    }

    //There is always a default launch config present
    void LaunchGame();

    void SetModSet(size_t index);
    void SetIwad(size_t index);
    void SetIwadDir(std::string dir);

    QStringList GetModsetLabels() ;
    QStringList MakeIwadLabels() ;
    QStringList MakeModLabels() ;

    void EnableMod(size_t mod_folder_index, bool enable = true);

    std::vector<size_t> GetEnabledMods();
    void SetEnabledMods(const std::vector<size_t>& enabled_mods);

    inline std::vector<fs::path>& GetModPaths() {
        return cfg_.mod_paths;
    }

    std::string GetIwad() {
        if(!cfg_.iwad_paths.empty() && cfg_.iwad_paths.size() >= cfg_.chosen_iwad && !cfg_.iwad_paths[cfg_.chosen_iwad].empty()) {
            return cfg_.iwad_paths[cfg_.chosen_iwad].filename().string();
        }
        return "";
    }

    dgc::ModSet& GetModset(size_t index) {
        return games_[index];
    }

    const dgc::Cfg& GetCfg() {
        return cfg_;
    }

    std::string GetLaunchCmd();
    std::string GetDisplayCmd();

    virtual std::string MakeLaunchCommand() = 0;

private:
    dgc::Cfg cfg_;
    dgc::DgcParser parser_;
    std::vector<dgc::ModSet> games_{1};

    void CreateDirIfDoesntExist(const fs::path& dir) const;
    virtual void PerformLaunch() = 0;
};


// class gzdml_win_qt final : public GzdoomLauncher {
// public:
//     gzdml_win_qt();
//     ~gzdml_win_qt() = default;
    
// private:
//     std::string MakeLaunchCommand(size_t index) override;
//     void PerformLaunch(size_t index) override;
// };



class gzdml_mac_qt final : public GzdoomLauncher {
public:
    gzdml_mac_qt();
    ~gzdml_mac_qt() = default;

    std::string MakeLaunchCommand() override;

private:
    //creates all the neccessary directories, if not already present
    void PerformLaunch() override;
};


//TODO: IFdef windows, etc...

//TODO: "fname is: " gzdoomexe_full.filename().string()
// " and path is: " gzdoomexe_full.remove_filename() - not working atm



//Launch a program:
/*
//SHELL
HINSTANCE ShellExecuteA(
  [in, optional] HWND   hwnd,
  [in, optional] LPCSTR lpOperation,
  [in]           LPCSTR lpFile,
  [in, optional] LPCSTR lpParameters,
  [in, optional] LPCSTR lpDirectory,
  [in]           INT    nShowCmd
);

//PROCESS
#include <windows.h>

VOID startup(LPCTSTR lpApplicationName)
{
   // additional information
   STARTUPINFO si;
   PROCESS_INFORMATION pi;

   // set the size of the structures
   ZeroMemory( &si, sizeof(si) );
   si.cb = sizeof(si);
   ZeroMemory( &pi, sizeof(pi) );

  // start the program up
  CreateProcess( lpApplicationName,   // the path
	argv[1],        // Command line
	NULL,           // Process handle not inheritable
	NULL,           // Thread handle not inheritable
	FALSE,          // Set handle inheritance to FALSE
	0,              // No creation flags
	NULL,           // Use parent's environment block
	NULL,           // Use parent's starting directory
	&si,            // Pointer to STARTUPINFO structure
	&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	);
	// Close process and thread handles.
	CloseHandle( pi.hProcess );
	CloseHandle( pi.hThread );
}
*/

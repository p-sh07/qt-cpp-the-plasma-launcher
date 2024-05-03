#pragma once

//#include <windows.h>
//#include <Atlconv.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
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

class GzdoomLauncher
{
public:
    GzdoomLauncher();
    ~GzdoomLauncher() = default;

    GzdoomLauncher(const fs::path& gzdoom_dir);
    
    void AddNewModset(const fs::path& iwad, const std::vector<fs::path>& mods, const fs::path& config = dgc::DFLT_CONFIG);
    void AddModToExistingSet(size_t mset_id, const fs::path& wad_or_pk3);

    //There is always a default launch config present
    void LaunchGame();

    void SetModSet(size_t index);
    void SetIwad(size_t index);

    std::unordered_set<std::string_view> GetEnabledMods();
    void SetEnabledMods(const std::unordered_set<std::string_view>& enabled_mods);

    void SaveChanges();

protected:
    //PARAMS
    inline const static fs::path gzdoomexe_ = "gzdoom.exe";

    fs::path gzdoompath_     = dgc::DFLT_GZDOOM;
    fs::path gzdoomexe_full_ = gzdoompath_ / gzdoomexe_;
    fs::path gzdoom_config_  = dgc::DFLT_CONFIG;
    fs::path mod_config_     = dgc::DFLT_DCG_PATH;

    fs::path iwad_dir_       = dgc::DFLT_IWADS_PATH;
    fs::path mod_dir_        = dgc::DFLT_MODWADS_PATH;

    //METHODS
    std::vector<std::string_view> MakeModsetLabels();
    std::vector<std::string> MakeLabels(const fs::path& dir);

    //makes a launch command string from a ModSet - define for each OS separately in case of specifics ?
    virtual std::string MakeLaunchCommand(const dgc::ModSet& mset) const;
    void CreateDirIfDoesntExist(const fs::path& dir);
private:
    size_t chosen_mod_set_ = 0;
    size_t chosen_iwad_ = 0;
    //looks for the gzdoom exe and sets the path variables
    void InitGZDoomPaths();
    // //creates all the neccessary directories, if not already present
    void SetupFolders();
    dgc::DgcParser parser_;
    //games_[0] is default & temp?
    std::vector<dgc::ModSet> games_{1};
    
    virtual void PerformLaunch(const dgc::ModSet& mset) const = 0;
};

class gzdml_win_qt final : public GzdoomLauncher {
public:
    gzdml_win_qt();
    ~gzdml_win_qt() = default;

    QStringList GetModsetLabels();
    QStringList GetIwadLabels();
    QStringList GetModLabels();
    
private:
    std::string MakeLaunchCommand(const dgc::ModSet& ms) const override;
    void PerformLaunch(const dgc::ModSet& ms) const override;
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

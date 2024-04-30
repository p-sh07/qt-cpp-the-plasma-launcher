#pragma once

#include <windows.h>
#include <Atlconv.h>


#include <fstream>
#include <iostream>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::ofstream;
using std::ifstream;
using std::filesystem::path;

namespace fs = std::filesystem;

const path DFLT_DCG_PATH = "mod_options.dgc";
const path DFLT_GZDOOM = "gzdoom";
const path DFLT_CONFIG = "gzdoom_portable.ini";
const std::string DEFAULT_COMMAND = "-config gzdoom_portable.ini -file mods\\SIGIL_v1_21.wad mods\\SIGIL_SHREDS.wad mods\\SIGIL_II_V1_0.WAD mods\\sigil2_ost_thorr.WAD mods\\D64TEX.PK3 mods\\DoomMetalVol5.wad";

struct DooM
{
	std::string label = "Default";

	path gzdoom_dir = DFLT_GZDOOM;

	//path iwad_dir = "iwads";
	path iwad;

	path mod_dir = "mods";
	std::vector<std::string> mod_filenames;

	path config = DFLT_CONFIG;

	//zips dir? multiple dirs?
};

class GZDML
{
public:
	GZDML() {
		if (fs::current_path().string() == "C:\\Projects\\Qt\\gzdml") {
			cerr << "!changed curr path" << endl;
			fs::current_path(path{ "C:\\Projects\\Qt\\gzdml\\x64\\Release" });

			//bool is_first_launch = CheckIfFirstLaunch();
			file_writer_.open(DFLT_DCG_PATH.string());


		}

		//set mod configs for testing
		launch_dgcs_[0].label = "Classic DOOM";

		//"SIGIL_II_V1_0.WAD mods\\sigil2_ost_thorr.WAD mods\\D64TEX.PK3 mods\\DoomMetalVol5.wad";

		launch_dgcs_[1].label = "Brutal DooM Sigil I&II";
		launch_dgcs_[1].iwad = "DOOM.WAD";
		launch_dgcs_[1].mod_filenames = { "brutalv22test3.pk3", "SIGIL_v1_21.wad", "SIGIL_SHREDS.wad", "SIGIL_II_V1_0.WAD", "sigil2_ost_thorr.WAD", "D64TEX.PK3", "DoomMetalVol5.wad"};

		launch_dgcs_[2].label = "Brutal DooM BTSX I";
		launch_dgcs_[2].iwad = "DOOM2.WAD";
		launch_dgcs_[2].mod_filenames = { "btsx_e1a.wad", "btsx_e1b.wad", "brutalv22test3.pk3" };

		launch_dgcs_[3].label = "Brutal DooM BTXS II";
		launch_dgcs_[3].iwad = "DOOM2.WAD";
		launch_dgcs_[3].mod_filenames = { "btsx_e2a.wad", "btsx_e2b.wad", "brutalv22test3.pk3" };

		launch_dgcs_[4].label = "Lost in Darkness";
		launch_dgcs_[4].iwad = "DOOM.WAD";
		launch_dgcs_[4].mod_filenames = { "d2-in-d1-v3.pk3", "Lost_in_DarknessV2.0.23.pk3", "LiveReverb.pk3", "Rain_and_Snow.pk3", "D64TEX.PK3"};

		launch_dgcs_[5].label = "Brutal Doom Lost in Darkness?";
		launch_dgcs_[5].iwad = "DOOM.WAD";
		launch_dgcs_[5].mod_filenames = { "d2-in-d1-v3.pk3", "Lost_in_DarknessV2.0.23.pk3", "LiveReverb.pk3", "Rain_and_Snow.pk3", "brutalv22test3.pk3", "D64TEX.PK3"};

	}
	GZDML(const path& gzdoom_dir);

	DooM ParseStringInput(std::string_view in);

	void LoadModLaunchConfig(path config_file);
	void SaveModLaunchConfig(path config_file);

	// TODO: use latest id? add new? use back()?
	void AddMod(const path& wad_pk3);

	void AddGamefromParams(const path& iwad, const path& mod, const path& config = DFLT_CONFIG);
	void AddGamefromParams(const path& iwad, const std::vector<path>& mods, const path& config = DFLT_CONFIG);

	void AddADoom(DooM lp);

	void SetGZDoomDir();

	//returns the mod label
	std::string_view SetPrevMod() {
		//loop to end
		if (selected_mod_id_ == 0) {
			selected_mod_id_ = launch_dgcs_.size()-1;
		}
		else {
			--selected_mod_id_;
		}
		return launch_dgcs_[selected_mod_id_].label;
	}
	//returns the mod label
	std::string_view SetNextMod() {
		//loop to beginning
		if (selected_mod_id_ >= launch_dgcs_.size() - 1) {
			selected_mod_id_ = 0;
		}
		else {
			++selected_mod_id_;
		}
		return launch_dgcs_[selected_mod_id_].label;
	}


	//There is always a default launch config present
	void LaunchGame(size_t id = 0) {

		try {
			PerformLaunch();
		}
		catch (std::exception& ex) {
			cout << "Error during launch: " << ex.what() << endl;
		}
	}

private:
//PARAMS
	size_t selected_mod_id_ = 0;

	path gzdoomexe_full = "gzdoom/gzdoom.exe";
	path gzdoomexe = "gzdoom.exe";
	path gzdoompath = "gzdoom/";

	path mod_config = DFLT_DCG_PATH;
	//TODO: "fname is: " gzdoomexe_full.filename().string() 
	// " and path is: " gzdoomexe_full.remove_filename() - not working atm

	std::ofstream file_writer_;

	std::vector<DooM> launch_dgcs_{ 6 };

//METHODS
	bool CheckIfFirstLaunch() {
		//auto dir_it = fs::directory_iterator(fs::current_path());
		//cerr << "looking for cfg:";
		//for (const auto& entry : dir_it) {
		//	if (entry.is_regular_file() && entry.path().filename() == DFLT_DCG_PATH) {

		//	}
		//}
		return !fs::exists(DFLT_DCG_PATH);
	}
	//looks for the gzdoom exe and sets the path variables
	void InitGZDoomPaths();

	//creates all the neccessary directories
	void SetupFolders(); 

	//makes a launch command string from a DooM-config
	std::string MakeLaunchCommand(size_t mod_id) {

		auto& dgc = launch_dgcs_[mod_id];

		std::string launch_command = "-config ";
		launch_command.append(dgc.config.string());

		if (!dgc.iwad.empty()) {
			launch_command.append(" -iwad ");
			launch_command.append(dgc.iwad.string());
		}

		launch_command.append(" -file");

		for (const auto& mod : dgc.mod_filenames) {
			launch_command.append(" " + dgc.mod_dir.string() + "\\" + mod);
		}
		//cerr << "\n*** launching w/command: " << launch_command << endl;
		return launch_command;
	}

	bool ParseDgcFile(path file) {
		////////parse.....
	}

	//Tested - working
	void PerformLaunch() {
		std::string lcmd = MakeLaunchCommand(selected_mod_id_);
		ShellExecuteW(NULL, NULL, gzdoomexe.c_str(), std::wstring(lcmd.begin(), lcmd.end()).c_str(), gzdoompath.c_str(), SW_SHOWDEFAULT);

		//STARTUPINFO si;
		//PROCESS_INFORMATION pi;

		// set the size of the structures
		//ZeroMemory(&si, sizeof(si));
		//si.cb = sizeof(si);
		//ZeroMemory(&pi, sizeof(pi));

		//std::string command = "-config user_config.txt -file mods\\brutalv22test3.pk3 mods\\SIGIL_v1_21.wad mods\\SIGIL_SHREDS.wad mods\\D64TEX.PK3 mods\\DoomMetalVol5.wad";

		//CreateProcess(
		//	gzdoomexe.c_str(),   // the path
		//	NULL,			// Command line
		//	NULL,           // Process handle not inheritable
		//	NULL,           // Thread handle not inheritable
		//	FALSE,          // Set handle inheritance to FALSE
		//	0,              // No creation flags
		//	NULL,           // Use parent's environment block
		//	NULL,           // Use parent's starting directory
		//	&si,            // Pointer to STARTUPINFO structure
		//	&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		//);
		//// Close process and thread handles.
		//CloseHandle(pi.hProcess);
		//CloseHandle(pi.hThread);

		//ALT, working:
		//ShellExecute(NULL, L"runas", gzdoomexe.c_str(), NULL, L"gzdoom", SW_SHOWDEFAULT);
		//ShellExecute(NULL, NULL, gzdoomexe.c_str(), NULL, L"gzdoom", SW_SHOWDEFAULT);
	}
};


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
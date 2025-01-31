#pragma once
#include <string>
#include <vector>

struct FilesContainer
{
	std::string sPath;                    // Full path to this directory
	std::string sDirectory;               // Name of this directory
	std::vector<std::string> vsFiles;     // Files in this directory
	std::vector<FilesContainer> vSubDirs; // Subdirectories
};

extern std::vector<FilesContainer> g_vFilesContainer;

extern void GetAllFiles( const std::string& sBasePath );
extern void Render();
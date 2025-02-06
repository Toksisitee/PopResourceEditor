#pragma once
#include <string>
#include <vector>

#include "Assets.h"

struct FileAsset
{
	std::string sFile;
	std::string sFileType;
	Assets::FileType eFileType;
};

struct FilesContainer
{
	std::string sPath;						 // Full path to this directory
	std::string sDirectory;					 // Name of this directory
	std::vector<FileAsset> vsFiles;			// Files in this directory
	std::vector<FilesContainer> vSubDirs;	// Subdirectories
};

extern std::vector<FilesContainer> g_vFilesContainer;

extern void GetAllFiles( const std::string& sBasePath );
extern void Render();
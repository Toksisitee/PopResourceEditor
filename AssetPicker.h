/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <string>
#include <vector>

#include "AssetTypes.h"

namespace AssetPicker
{
	struct FileAsset
	{
		std::string sFile;
		std::string sFileType;
		Assets::FileType eFileType;
	};

	struct FilesContainer
	{
		std::string sPath;						// Full path to this directory
		std::string sDirectory;					// Name of this directory
		std::vector<FileAsset> vsFiles;			// Files in this directory
		std::vector<FilesContainer> vSubDirs;	// Subdirectories
	};

	extern std::vector<FilesContainer> g_vFilesContainer;

	extern void GetAllFiles( std::string& sBasePath );
	extern void Render();
}

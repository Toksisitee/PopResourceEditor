#pragma once
#include <string>
#include "Editor.h"

namespace Util
{
	namespace FileSystem
	{
		extern std::string GetApplicationDirectory();
		extern std::string GetAssetsDirectory();
		extern std::string GetCurrentDir();
		extern std::string FormatPath( const char* pFileName, const char* pFilePath = nullptr );
		[[nodiscard]] extern bool FileExists( const std::string& file );
	}
}
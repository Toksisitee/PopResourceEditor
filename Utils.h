#pragma once
#include <string>
#include <vector>
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
		[[nodiscard]] extern std::string GetFolderName( const std::string& sPath );
		[[nodiscard]] extern std::vector<uint8_t> ReadFileMagic( const std::string& sFilePath, size_t uMagicSize );
		[[nodiscard]] extern std::string GetFileExtension( const std::string& sFilePath );
		[[nodiscard]] extern std::string GetFileName( const std::string& sFilePath );
		[[nodiscard]] extern size_t GetFileSize( const std::string& sFilePath );
		[[nodiscard]] extern std::string GetLastCharacterInFilePath( const std::string& sFilePath );
		[[nodiscard]] extern std::string RemoveFileExtension( const std::string& sFilePath );
		[[nodiscard]] extern std::string GetParentDirectory( const std::string& sFilePath );
		[[nodiscard]] extern bool PathExists( const std::string& sPath );
	}
}
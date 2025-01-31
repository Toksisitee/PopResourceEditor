#include <windows.h>
#include <fstream>

#include "Utils.h"

namespace Util
{
	namespace FileSystem
	{
		std::string GetApplicationDirectory()
		{
			static std::string sFinalPath;
			if ( sFinalPath.empty() ) {
				wchar_t wszPath[MAX_PATH] = { 0 };
				GetModuleFileNameW( NULL, wszPath, MAX_PATH );

				wchar_t* pwszLastSlash = wcsrchr( wszPath, L'\\' );
				if ( pwszLastSlash != nullptr ) {
					*pwszLastSlash = L'\0'; // truncate
				}

				std::wstring wStr( wszPath );
				sFinalPath = std::string( wStr.begin(), wStr.end() );
			}
			return sFinalPath;
		}

		std::string GetAssetsDirectory()
		{
			static std::string sDataPath;
			if ( sDataPath.empty() ) {
				sDataPath = GetApplicationDirectory();
				sDataPath += "\\Assets\\";
			}
			return sDataPath;
		}

		std::string GetCurrentDir()
		{
			char szBuffer[MAX_PATH];
			if ( GetModuleFileNameA( NULL, szBuffer, MAX_PATH ) == 0 ) {
				return "";
			}

			std::string fullPath( szBuffer );
			size_t uSlash = fullPath.find_last_of( "\\/" );
			if ( uSlash != std::string::npos ) {
				return fullPath.substr( 0, uSlash );
			}
			return fullPath;
		}

		std::string FormatPath( const char* pFileName, const char* pFilePath )
		{
			char szDirBuffer[MAX_PATH];
			char szPathBuffer[MAX_PATH];

			if ( !pFilePath )
				sprintf_s( szDirBuffer, MAX_PATH, GetCurrentDir().c_str() );
			else
				sprintf_s( szDirBuffer, MAX_PATH, pFilePath );

			sprintf_s( szPathBuffer, MAX_PATH, "%s\\%s", szDirBuffer, pFileName );
			return std::string( szPathBuffer );
		}

		[[nodiscard]] bool FileExists( const std::string& sFilePath )
		{
			struct stat buffer;
			return (stat( sFilePath.c_str(), &buffer ) == 0);
		}

		[[nodiscard]] std::string GetFolderName( const std::string& sPath )
		{
			size_t uSlash = sPath.find_last_of( "/\\" );
			if ( uSlash == std::string::npos ) {
				return sPath;
			}
			return sPath.substr( uSlash + 1 );
		}

		[[nodiscard]] std::vector<uint8_t> ReadFileMagic( const std::string& sFilePath, size_t uMagicSize )
		{
			std::ifstream file( sFilePath, std::ios::binary );
			if ( !file.is_open() ) {
				return {};
			}

			std::vector<uint8_t> vecMagic( uMagicSize );
			file.read( reinterpret_cast<char*>(vecMagic.data()), uMagicSize );
			file.close();

			return vecMagic;
		}

		[[nodiscard]] std::string GetFileExtension( const std::string& sFilePath )
		{
			size_t uDot = sFilePath.find_last_of( '.' );
			if ( uDot == std::string::npos || uDot == sFilePath.length() - 1 ) {
				return "";
			}
			return sFilePath.substr( uDot );
		}

		[[nodiscard]] std::string GetFileName( const std::string& sFilePath )
		{
			size_t uSlash = sFilePath.find_last_of( "/\\" );
			if ( uSlash == std::string::npos ) {
				return sFilePath;
			}
			return sFilePath.substr( uSlash + 1 );
		}

		[[nodiscard]] size_t GetFileSize( const std::string& sFilePath )
		{
			std::ifstream file( sFilePath, std::ios::binary | std::ios::ate );
			if ( !file.is_open() ) {
				return 0;
			}
			return file.tellg();
		}
	}
}
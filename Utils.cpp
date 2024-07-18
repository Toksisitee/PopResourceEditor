#include <windows.h>
#include <string>

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
			GetCurrentDirectoryA( MAX_PATH, szBuffer );
			return (std::string( szBuffer ));
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

		bool FileExists( const std::string& sFilePath )
		{
			struct stat buffer;
			return (stat( sFilePath.c_str(), &buffer ) == 0);
		}
	}
}
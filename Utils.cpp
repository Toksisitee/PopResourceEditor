#include <windows.h>
#include <fstream>
#include <algorithm>

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
					*pwszLastSlash = L'\0';
				}

				int nSize = WideCharToMultiByte( CP_UTF8, 0, wszPath, -1, NULL, 0, NULL, NULL );
				if ( nSize > 0 ) {
					std::string str( nSize - 1, 0 );
					WideCharToMultiByte( CP_UTF8, 0, wszPath, -1, &str[0], nSize, NULL, NULL );
					sFinalPath = str;
				}
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

			std::streamoff fileSize = file.tellg();
			if ( fileSize < 0 ) {
				return 0;
			}

			return static_cast<size_t>(fileSize);
		}

		[[nodiscard]] std::string GetLastCharacterInFilePath( const std::string& sFilePath )
		{
			size_t uDotPos = sFilePath.find_last_of( '.' );
			if ( uDotPos == std::string::npos || uDotPos == 0 ) return "";
			size_t uCharPos = uDotPos - 1;
			if ( uCharPos < sFilePath.size() ) {
				return std::string( 1, sFilePath[uCharPos] );
			}
			return "";
		}

		[[nodiscard]] std::string RemoveFileExtension( const std::string& sFilePath )
		{
			size_t uDot = sFilePath.find_last_of( '.' );
			size_t uSlash = sFilePath.find_last_of( "/\\" );

			if ( uDot != std::string::npos && (uSlash == std::string::npos || uDot > uSlash) ) {
				return sFilePath.substr( 0, uDot );
			}

			return sFilePath;
		}

		[[nodiscard]] std::string GetParentDirectory( const std::string& sFilePath )
		{
			size_t uLastSlash = sFilePath.find_last_of( "/\\" );
			if ( uLastSlash == std::string::npos ) {
				return "";
			}

			size_t uSecondLastSlash = sFilePath.find_last_of( "/\\", uLastSlash - 1 );
			if ( uSecondLastSlash == std::string::npos ) {
				return "";
			}

			return sFilePath.substr( 0, uSecondLastSlash + 1 );
		}

		[[nodiscard]] bool PathExists( const std::string& sPath )
		{
			DWORD dwAttr = GetFileAttributesA( sPath.c_str() );
			return (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
		}

		void RemoveFileSpec( char* pszPath )
		{
			if ( !pszPath || !*pszPath )
				return;

			size_t uLen = strlen( pszPath );

			for ( size_t i = uLen; i > 0; --i ) {
				if ( pszPath[i] == '\\' || pszPath[i] == '/' ) {
					pszPath[i] = '\0';
					break;
				}
			}
		}
	}

	std::string StringToLowerCopy( const std::string& sStr )
	{
		std::string sLower = sStr;
		std::transform( sLower.begin(), sLower.end(), sLower.begin(), []( unsigned char c ) { return std::tolower( c ); } );
		return sLower;
	}
}
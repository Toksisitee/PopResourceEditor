#include <unordered_map>
#include <filesystem>
#include <fstream>

#include "spdlog\spdlog.h"
#include "imgui.h"

#include "Utils.h"
#include "Assets.h"
#include "BigFade.h"
#include "Fade.h"
#include "Palette.h"
#include "AssetPicker.h"

std::vector<FilesContainer> g_vFilesContainer;
using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

struct VectorHash {
	std::size_t operator()( const std::vector<uint8_t>& vec ) const
	{
		std::size_t uHash = 0;
		for ( const auto& byte : vec ) {
			uHash ^= std::hash<uint8_t>()(byte) + 0x9e3779b9 + (uHash << 6) + (uHash >> 2);
		}
		return uHash;
	}
};

struct CaseInsensitiveHash {
	std::size_t operator()( const std::string& sStr ) const
	{
		std::string sLower = sStr;
		std::transform( sLower.begin(), sLower.end(), sLower.begin(), ::tolower );
		return std::hash<std::string>()(sLower);
	}
};

struct CaseInsensitiveEqual {
	bool operator()( const std::string& lhs, const std::string& rhs ) const
	{
		if ( lhs.size() != rhs.size() ) return false;
		return std::equal( lhs.begin(), lhs.end(), rhs.begin(),
			[]( char a, char b ) { return std::tolower( a ) == std::tolower( b ); } );
	}
};

struct FileTypeSize
{
	Assets::FileType eFileType;
	size_t uSize;
};

std::unordered_map<std::string, FileTypeSize, CaseInsensitiveHash, CaseInsensitiveEqual> mapName = {
	{"bigf", { Assets::FileType::BigFade, Assets::BigFade::k_uSize }},
	{"fade", { Assets::FileType::Fade, Assets::Fade::k_uSize }},
	{"pal", { Assets::FileType::Palette, Assets::Palette::k_uSize}},
};

std::unordered_map<std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual> mapExtension = {
	{".txt", "Text"},
	{".png", "Image"},
	{".jpg", "Image"},
	{".jpeg", "Image"},
	{".gif", "Image"},
	{".bmp", "Image"},
	{".SF2", "Sound Bank"},
	{".SDT", "Sound Bank"},
};

std::unordered_map<std::vector<uint8_t>, std::string, VectorHash> mapMagic = {
	{{0x50, 0x53, 0x46, 0x42}, "Sprite Bank"},
};

bool FileTypeCorrectSize( Assets::FileType eFileType, size_t uSize )
{
	switch ( eFileType ) {
		case Assets::FileType::BigFade:
			return true;
			break;
	}
}

std::string GetFileType( const std::string& sFilePath, bool& bKnownType )
{
	bKnownType = false;
	std::string sFileName = Util::FileSystem::GetFileName( sFilePath );

	for ( const auto& [name, entry] : mapName ) {
		if ( sFileName.find( name ) != std::string::npos ) {
			if ( Util::FileSystem::GetFileSize( sFilePath ) == entry.uSize ) {
				bKnownType = true;
				return std::string( Assets::GetFileTypeSz( entry.eFileType ) );
			}
		}
	}

	std::string sExt = Util::FileSystem::GetFileExtension( sFilePath );

	if ( !sExt.empty() ) {
		auto it = mapExtension.find( sExt );
		if ( it != mapExtension.end() ) {
			bKnownType = true;
			return it->second;
		}
	}

	constexpr size_t uMaxHeaderSize = 8;
	std::vector<uint8_t> vecHeader = Util::FileSystem::ReadFileMagic( sFilePath, uMaxHeaderSize );

	for ( const auto& [magicNumber, fileType] : mapMagic ) {
		if ( vecHeader.size() >= magicNumber.size() &&
			std::equal( magicNumber.begin(), magicNumber.end(), vecHeader.begin() ) ) {
			bKnownType = true;
			return fileType;
		}
	}

	return "Unknown";
}

void GetFilesRecursively( const std::string& sPath, FilesContainer& container )
{
	if ( !std::filesystem::exists( sPath ) || !std::filesystem::is_directory( sPath ) ) {
		spdlog::warn( "Invalid path: {}", sPath );
		return;
	}

	for ( const auto& entry : std::filesystem::directory_iterator( sPath ) ) {
		if ( entry.is_directory() ) {
			FilesContainer subContainer;
			subContainer.sPath = entry.path().string();
			subContainer.sDirectory = Util::FileSystem::GetFolderName( subContainer.sPath );
			GetFilesRecursively( subContainer.sPath, subContainer );
			container.vSubDirs.push_back( subContainer );
		}
		else if ( entry.is_regular_file() ) {
			std::string sFilePath = entry.path().string();
			bool bKnownType = false;
			std::string sFileType = GetFileType( sFilePath, bKnownType );

			if ( bKnownType ) {
				spdlog::info( "File: {}, Type: {}", sFilePath, sFileType );
				container.vsFiles.push_back( sFilePath + " (" + sFileType + ")" );
			}
		}
	}
}

void GetAllFiles( const std::string& sBasePath )
{
	g_vFilesContainer.clear();

	FilesContainer baseContainer;
	baseContainer.sPath = sBasePath;
	baseContainer.sDirectory = Util::FileSystem::GetFolderName( baseContainer.sPath );

	GetFilesRecursively( sBasePath, baseContainer );
	g_vFilesContainer.push_back( baseContainer );
}

void RenderDirectory( const FilesContainer& container, std::string& selectedAsset )
{
	ImGui::SetNextItemOpen( true );
	if ( ImGui::TreeNode( container.sDirectory.c_str() ) ) {
		for ( const auto& filePath : container.vsFiles ) {
			std::string fileName = std::filesystem::path( filePath ).filename().string();

			if ( ImGui::Selectable( fileName.c_str(), selectedAsset == filePath ) ) {
				selectedAsset = filePath;
				spdlog::info( "Selected file: {}", selectedAsset );
			}
		}

		for ( const auto& subContainer : container.vSubDirs ) {
			if ( subContainer.vsFiles.size() == 0 ) continue;
			ImGui::SetNextItemOpen( true );
			RenderDirectory( subContainer, selectedAsset );
		}

		ImGui::TreePop();
	}
}

void Render()
{
	ImGui::Begin( "Asset Picker" );

	static std::string sSelectedAsset = "";

	for ( const auto& container : g_vFilesContainer ) {
		RenderDirectory( container, sSelectedAsset );
	}

	ImGui::Separator();
	ImGui::Text( "Selected Asset:" );
	if ( !sSelectedAsset.empty() ) {
		ImGui::TextWrapped( "%s", sSelectedAsset.c_str() );
	}
	else {
		ImGui::Text( "None" );
	}

	ImGui::End();
}

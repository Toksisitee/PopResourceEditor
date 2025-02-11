#include <unordered_map>
#include <filesystem>
#include <fstream>

#include "spdlog\spdlog.h"
#include "imgui.h"

#include "Utils.h"
#include "Assets.h"
#include "BigFade.h"
#include "Fade.h"
#include "Alpha.h"
#include "Ghost.h"
#include "Sky.h"
#include "Blocks.h"
#include "Cliff.h"
#include "Disp.h"
#include "Palette.h"
#include "AssetPicker.h"
#include "ImEditor.h"

std::vector<FilesContainer> g_vFilesContainer;
using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;
enum class ViewMode { List, Grid };
ViewMode g_ViewMode = ViewMode::Grid;

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
	{"al0", { Assets::FileType::Alpha, Assets::Alpha::k_uSize}},
	{"bigf0", { Assets::FileType::BigFade, Assets::BigFade::k_uSize }},
	{"bl320", { Assets::FileType::Blocks, Assets::Blocks::k_uSize}},
	{"cliff0", { Assets::FileType::Cliff, Assets::Cliff::k_uSize}},
	{"disp0", { Assets::FileType::Disp, Assets::Disp::k_uSize}},
	{"fade0", { Assets::FileType::Fade, Assets::Fade::k_uSize }},
	{"ghost0", { Assets::FileType::Ghost, Assets::Ghost::k_uSize }},
	{"pal0", { Assets::FileType::Palette, Assets::Palette::k_uSize}},
	{"sky0", { Assets::FileType::Sky, Assets::Sky::k_uSize}},
};

std::unordered_map<std::string, std::string, CaseInsensitiveHash, CaseInsensitiveEqual> mapExtension = {
	{".SF2", "Sound Bank"},
	{".SDT", "Sound Bank"},
};

std::unordered_map<std::vector<uint8_t>, FileAsset, VectorHash> mapMagic = {
	{{0x50, 0x53, 0x46, 0x42}, {"Sprite Bank", "Sprite", Assets::FileType::Sprite}},
};

bool FileTypeCorrectSize( Assets::FileType eFileType, size_t uSize )
{
	switch ( eFileType ) {
		case Assets::FileType::BigFade:
			return true;
			break;
	}
}

// TODO: Add support for search type. Full (Slow) and Fast (Name+Ext only)
std::string GetFileType( const std::string& sFilePath, Assets::FileType& eType )
{
	eType = Assets::FileType::Unknown;
	std::string sFileName = Util::FileSystem::GetFileName( sFilePath );

	for ( const auto& [name, asset] : mapName ) {
		if ( sFileName.find( name ) != std::string::npos ) {
			if ( Util::FileSystem::GetFileSize( sFilePath ) == asset.uSize ) {
				eType = asset.eFileType;
				return std::string( Assets::GetFileTypeSz( asset.eFileType ) );
			}
		}
	}

	std::string sExt = Util::FileSystem::GetFileExtension( sFilePath );

	if ( !sExt.empty() ) {
		auto it = mapExtension.find( sExt );
		if ( it != mapExtension.end() ) {
			eType = Assets::FileType::Unknown; // TODO: handle ext based assets
			return it->second;
		}
	}

	constexpr size_t uMaxHeaderSize = 8;
	std::vector<uint8_t> vecHeader = Util::FileSystem::ReadFileMagic( sFilePath, uMaxHeaderSize );

	for ( const auto& [magicNumber, asset] : mapMagic ) {
		if ( vecHeader.size() >= magicNumber.size() &&
			std::equal( magicNumber.begin(), magicNumber.end(), vecHeader.begin() ) ) {
			eType = asset.eFileType;
			return asset.sFile;
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
			Assets::FileType eType = Assets::FileType::Unknown;
			std::string sFileType = GetFileType( sFilePath, eType );

			if ( eType != Assets::FileType::Unknown ) {
				spdlog::info( "File: {}, Type: {}", sFilePath, sFileType );
				container.vsFiles.push_back( { sFilePath, sFileType, eType } );
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

void RenderDirectory( const FilesContainer& container, std::string& sSelectedAsset )
{
	if ( ImGui::TreeNodeEx( container.sDirectory.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) ) {
		for ( const auto& entry : container.vsFiles ) {
			std::string fileName = std::filesystem::path( entry.sFile ).filename().string() + " (" + entry.sFileType + ")";

			if ( ImGui::Selectable( fileName.c_str(), sSelectedAsset == entry.sFile ) ) {
				Assets::OpenWnd( entry.sFile, entry.eFileType );
				sSelectedAsset = entry.sFile;
				spdlog::info( "Selected file: {}", sSelectedAsset );
			}
		}

		for ( const auto& subContainer : container.vSubDirs ) {
			if ( subContainer.vsFiles.size() == 0 ) continue;
			RenderDirectory( subContainer, sSelectedAsset );
		}

		ImGui::TreePop();
	}
}

void RenderDirectoryGrid( const FilesContainer& container, std::string& sSelectedAsset, const std::string& sParentFolder = "" )
{
	const float fIconSize = 64.0f;
	const int nColumns = 7;

	std::string currentFolder = sParentFolder.empty() ? container.sDirectory : sParentFolder + "/" + container.sDirectory;
	if ( !container.vsFiles.empty() || !container.vSubDirs.empty() ) {
		if ( ImGui::TreeNodeEx( currentFolder.c_str(), ImGuiTreeNodeFlags_DefaultOpen ) ) {
			ImGui::Columns( nColumns, nullptr, false );
			for ( const auto& entry : container.vsFiles ) {
				
				ImVec2 v2IconPos = ImGui::GetCursorScreenPos(); 

				auto pTex2D = Assets::LoadTexture( entry.sFile, entry.eFileType );
				if ( pTex2D ) {
					ImEditor::RenderTexture( pTex2D, ImVec2( fIconSize, fIconSize ) );
				}
				else {
					ImGui::Button( "?", ImVec2( fIconSize, fIconSize ) );
				}

				if ( ImGui::IsItemHovered() ) {
					ImDrawList* drawList = ImGui::GetWindowDrawList();
					drawList->AddRect( v2IconPos, ImVec2( v2IconPos.x + fIconSize, v2IconPos.y + fIconSize ), IM_COL32( 255, 255, 255, 255 ), 0.0f, 0, 2.0f );
				}

				if ( ImGui::IsItemClicked() ) {
					sSelectedAsset = entry.sFile;
					spdlog::info( "Selected file: {}", sSelectedAsset );
				}

				ImGui::TextWrapped( "%s", std::filesystem::path( entry.sFile ).filename().string().c_str() );
				ImGui::NextColumn();
			}
			ImGui::Columns( 1 );

			for ( const auto& subContainer : container.vSubDirs ) {
				if ( subContainer.vsFiles.size() == 0 ) continue;
				RenderDirectoryGrid( subContainer, sSelectedAsset, currentFolder );
			}

			ImGui::TreePop();
		}
	}
}


void Render()
{
	ImGui::Begin( "Asset Picker" );
	static std::string sSelectedAsset = "";

	if ( ImGui::Button( "Toggle View" ) ) {
		g_ViewMode = (g_ViewMode == ViewMode::List) ? ViewMode::Grid : ViewMode::List;
	}

	for ( const auto& container : g_vFilesContainer ) {
		if ( g_ViewMode == ViewMode::List ) {
			RenderDirectory( container, sSelectedAsset );
		}
		else {
			RenderDirectoryGrid( container, sSelectedAsset );
		}
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

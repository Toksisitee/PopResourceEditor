#include <assert.h>

#include "Assets.h"
#include "Editor.h"
#include "App.h"
#include "AssetsErrHandler.h"

#include "WindowManager.h"
#include "SkyWnd.h"
#include "GhostWnd.h"
#include "FadeWnd.h"
#include "BigFadeWnd.h"
#include "CliffWnd.h"
#include "AlphaWnd.h"
#include "DispWnd.h"
#include "BlocksWnd.h"
#include "LevelWnd.h"

namespace Assets
{
	const char* GetFileTypeSz( FileType eFileType )
	{
		switch ( eFileType ) {
			case Assets::FileType::Palette:
				return "Palette";
			case Assets::FileType::Alpha:
				return "Alpha";
			case Assets::FileType::Sky:
				return "Sky";
			case Assets::FileType::Sprite:
				return "Sprite";
			case Assets::FileType::Ghost:
				return "Ghost";
			case Assets::FileType::Fade:
				return "Fade";
			case Assets::FileType::BigFade:
				return "BigFade";
			case Assets::FileType::Disp:
				return "Disp";
			case Assets::FileType::Cliff:
				return "Cliff";
			case Assets::FileType::Blocks:
				return "Blocks";
			case Assets::FileType::Level:
				return "Level";
			case Assets::FileType::Unknown:
				return "Unknown";
		}

		return "Unknown";
	}


	std::optional<std::string> ExtractAssetIdentifier( const std::string& sFilePath )
	{
		size_t uDash = sFilePath.find( '-' );
		size_t uDot = sFilePath.find( ".dat" );

		if ( uDash == std::string::npos || uDot == std::string::npos || uDash + 1 >= uDot ) {
			return std::nullopt;
		}

		return sFilePath.substr( uDash + 1, uDot - uDash - 1 );
	}

#define PALETTE_FORMAT "%s\\pal0-%s.dat"
#define SKY_FORMAT "%s\\sky0-%s.dat"
#define BIGFADE_FORMAT "%s\\bigf0-%s.dat"
#define FADE_FORMAT "%s\\fade0-%s.dat"
#define ALPHA_FORMAT "%s\\al0-%s.dat"
#define DISP_FORMAT "%s\\disp0-%s.dat"
#define CLIFF_FORMAT "%s\\cliff0-%s.dat"
#define GHOST_FORMAT "%s\\ghost0-%s.dat"
#define BLOCKS_FORMAT "%s\\bl320-%s.dat"

	std::optional<std::string> GenerateAssetPath( const std::string& sFilePath, const char* pszIdentifier, FileType eType )
	{
		char szPath[256];

		size_t uLastSlash = sFilePath.find_last_of( "/\\" );
		if ( uLastSlash != std::string::npos ) {
			std::string sDir = sFilePath.substr( 0, uLastSlash + 1 );
			switch ( eType ) {
				case FileType::Alpha:
					sprintf_s( szPath, ALPHA_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::BigFade:
					sprintf_s( szPath, BIGFADE_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Blocks:
					sprintf_s( szPath, BLOCKS_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Cliff:
					sprintf_s( szPath, CLIFF_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Fade:
					sprintf_s( szPath, FADE_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Ghost:
					sprintf_s( szPath, GHOST_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Disp:
					sprintf_s( szPath, DISP_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Palette:
					sprintf_s( szPath, PALETTE_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				case FileType::Sky:
					sprintf_s( szPath, SKY_FORMAT, sDir.c_str(), pszIdentifier );
					break;
				default:
				{
					assert( false && "GenerateAssetPath: Unknown Asset FileType" );
					return "GenerateAssetPath: Unknown Asset FileType";
				}
			}

			return std::string( szPath );
		}

		return std::nullopt;
	}

	Result QuickLoadPalette( CPalette* pPalette, std::string& sFilePath )
	{
		return pPalette->LoadBin( sFilePath );
	}

	Result QuickLoadAlpha( CAlpha* pAlpha, const std::string& sFilePath )
	{
		if ( auto osIdentifer = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifer).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pAlpha->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pAlpha->DestroyTexture();
		return pAlpha->Load( sFilePath );
	}

	Result QuickLoadSky( CSky* pSky, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pSky->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pSky->DestroyTexture();
		return pSky->LoadBin( sFilePath );
	}

	Result QuickLoadGhost( CGhost* pGhost, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pGhost->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pGhost->DestroyTexture();
		return pGhost->LoadBin( sFilePath );
	}

	Result QuickLoadFade( CFade* pFade, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pFade->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pFade->DestroyTexture();
		return pFade->LoadBin( sFilePath );
	}

	Result QuickLoadBigFade( CBigFade* pBigFade, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pBigFade->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pBigFade->DestroyTexture();
		return pBigFade->LoadBin( sFilePath );
	}

	Result QuickLoadDisp( CDisp* pDisp, const std::string& sFilePath )
	{
		pDisp->DestroyTexture();
		return pDisp->LoadBin( sFilePath );
	}

	Result QuickLoadCliff( CCliff* pCliff, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pCliff->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pCliff->DestroyTexture();
		return pCliff->LoadBin( sFilePath );
	}

	Result QuickLoadBlocks( CBlocks* pBlocks, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pBlocks->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pBlocks->DestroyTextures();
		return pBlocks->LoadBin( sFilePath );
	}

	Result QuickLoadLevel( CLevel* pLevel, const std::string& sFilePath )
	{
		if ( auto osIdentifier = ExtractAssetIdentifier( sFilePath ) ) {
			if ( auto osPalettePath = GenerateAssetPath( sFilePath, (*osIdentifier).c_str(), FileType::Palette ) ) {
				if ( QuickLoadPalette( pLevel->GetPalette(), *osPalettePath ) == Result::FAIL_LOAD ) {
					return Result::FAIL_LOAD;
				}
			}
		}

		pLevel->DestroyTexture();
		return pLevel->LoadBin( sFilePath );
	}

	Result OpenWnd( const std::string& sFilePath, FileType eFileType )
	{
		auto pDevice = g_Editor.GetDevice();
		switch ( eFileType ) {
			case Assets::FileType::Alpha:
			{
				auto pWnd = g_WndMngr.AddWindow<CAlphaWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadAlpha( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::BigFade:
			{
				auto pWnd = g_WndMngr.AddWindow<CBigFadeWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadBigFade( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Blocks:
			{
				auto pWnd = g_WndMngr.AddWindow<CBlocksWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadBlocks( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Cliff:
			{
				auto pWnd = g_WndMngr.AddWindow<CCliffWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadCliff( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Disp:
			{
				auto pWnd = g_WndMngr.AddWindow<CDispWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadDisp( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Fade:
			{
				auto pWnd = g_WndMngr.AddWindow<CFadeWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadFade( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Ghost:
			{
				auto pWnd = g_WndMngr.AddWindow<CGhostWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadGhost( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Level:
			{
				auto pWnd = g_WndMngr.AddWindow<CLevelWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadLevel( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Palette:
			{
				assert( false && "TODO: Implement PaletteWnd" );
				break;
			}
			case Assets::FileType::Sky:
			{
				auto pWnd = g_WndMngr.AddWindow<CSkyWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
				return QuickLoadSky( pWnd->GetAsset(), sFilePath );
			}
			case Assets::FileType::Sprite:
				assert( false && "TODO: Implement SpriteWnd" );
				break;
		}

		return Result::FAIL;
	}
}
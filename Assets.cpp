/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <assert.h>

#include "Texture.h"
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
#include "PaletteWnd.h"
#include "SpriteWnd.h"

#include "Assets.h"

namespace Util
{
	namespace FileSystem
	{
		extern [[nodiscard]] std::string GetFileName( const std::string& sFilePath );
	}
}

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

		if ( uDot == std::string::npos ) {
			uDot = sFilePath.find( ".DAT" );
		}

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
			std::string sAssetName = GenerateAssetName( pszIdentifier, eType );

			if ( sAssetName.empty() ) {
				return std::nullopt;
			}

			sprintf_s( szPath, "%s%s", sDir.c_str(), sAssetName.c_str() );
			return std::string( szPath );
		}

		return std::nullopt;
	}

	std::string GenerateAssetName( const char* pszIdentifier, FileType eType )
	{
		char szAssetName[256];

		switch ( eType ) {
			case FileType::Alpha:
				sprintf_s( szAssetName, ALPHA_FORMAT, "", pszIdentifier );
				break;
			case FileType::BigFade:
				sprintf_s( szAssetName, BIGFADE_FORMAT, "", pszIdentifier );
				break;
			case FileType::Blocks:
				sprintf_s( szAssetName, BLOCKS_FORMAT, "", pszIdentifier );
				break;
			case FileType::Cliff:
				sprintf_s( szAssetName, CLIFF_FORMAT, "", pszIdentifier );
				break;
			case FileType::Fade:
				sprintf_s( szAssetName, FADE_FORMAT, "", pszIdentifier );
				break;
			case FileType::Ghost:
				sprintf_s( szAssetName, GHOST_FORMAT, "", pszIdentifier );
				break;
			case FileType::Disp:
				sprintf_s( szAssetName, DISP_FORMAT, "", pszIdentifier );
				break;
			case FileType::Palette:
				sprintf_s( szAssetName, PALETTE_FORMAT, "", pszIdentifier );
				break;
			case FileType::Sky:
				sprintf_s( szAssetName, SKY_FORMAT, "", pszIdentifier );
				break;
			default:
				assert( false && "GenerateAssetName: Unknown Asset FileType" );
				return "";
		}

		return std::string( szAssetName + 1 );
	}


	Result QuickLoadPalette( CPalette* pPalette, const std::string& sFilePath )
	{
		pPalette->DestroyTexture();
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
		return pAlpha->LoadBin( sFilePath );
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
		pLevel->DestroyTexture();
		auto result = pLevel->LoadBin( sFilePath );
		if ( result == Result::OK_LOAD ) {
			pLevel->GeneratePreview( 10, -0.6f, -0.6f, true );
		}
		return result;
	}

	Result QuickLoadSprite( CSprite* pSprite, const std::string& sFilePath )
	{
		auto result = pSprite->LoadBin( sFilePath );
		return result;
	}

	Result QuickLoad( void* pAsset, const std::string& sFilePath, FileType eFileType )
	{
		switch ( eFileType ) {
			case Assets::FileType::Alpha:
				return QuickLoadAlpha( static_cast<CAlpha*>(pAsset), sFilePath );
			case Assets::FileType::BigFade:
				return QuickLoadBigFade( static_cast<CBigFade*>(pAsset), sFilePath );
			case Assets::FileType::Blocks:
				return QuickLoadBlocks( static_cast<CBlocks*>(pAsset), sFilePath );
			case Assets::FileType::Cliff:
				return QuickLoadCliff( static_cast<CCliff*>(pAsset), sFilePath );
			case Assets::FileType::Disp:
				return QuickLoadDisp( static_cast<CDisp*>(pAsset), sFilePath );
			case Assets::FileType::Fade:
				return QuickLoadFade( static_cast<CFade*>(pAsset), sFilePath );
			case Assets::FileType::Ghost:
				return QuickLoadGhost( static_cast<CGhost*>(pAsset), sFilePath );
			case Assets::FileType::Level:
				return QuickLoadLevel( static_cast<CLevel*>(pAsset), sFilePath );
			case Assets::FileType::Palette:
				return QuickLoadPalette( static_cast<CPalette*>(pAsset), sFilePath );
			case Assets::FileType::Sky:
				return QuickLoadSky( static_cast<CSky*>(pAsset), sFilePath );
			case Assets::FileType::Sprite:
				return QuickLoadSprite( static_cast<CSprite*>(pAsset), sFilePath );
		}

		return Result::FAIL_LOAD;
	}

	Result OpenWnd( const std::string& sFilePath, FileType eFileType )
	{
		if ( g_WndMngr.GetWindow( sFilePath ) ) {
			return Result::OK;
		}

		auto pDevice = g_Editor.GetDevice();
		switch ( eFileType ) {
			case Assets::FileType::Alpha:
			{
				auto pWnd = g_WndMngr.AddWindow<CAlphaWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::BigFade:
			{
				auto pWnd = g_WndMngr.AddWindow<CBigFadeWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Blocks:
			{
				auto pWnd = g_WndMngr.AddWindow<CBlocksWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Cliff:
			{
				auto pWnd = g_WndMngr.AddWindow<CCliffWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Disp:
			{
				auto pWnd = g_WndMngr.AddWindow<CDispWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Fade:
			{
				auto pWnd = g_WndMngr.AddWindow<CFadeWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Ghost:
			{
				auto pWnd = g_WndMngr.AddWindow<CGhostWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Level:
			{
				auto pWnd = g_WndMngr.AddWindow<CLevelWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Palette:
			{
				auto pWnd = g_WndMngr.AddWindow<CPaletteWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset().get(), sFilePath, eFileType );
			}
			case Assets::FileType::Sky:
			{
				auto pWnd = g_WndMngr.AddWindow<CSkyWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
			case Assets::FileType::Sprite:
			{
				auto pWnd = g_WndMngr.AddWindow<CSpriteWnd>( pDevice, sFilePath );
				pWnd->SetAssetName( Util::FileSystem::GetFileName( sFilePath ) );
				return QuickLoad( (void*)pWnd->GetAsset(), sFilePath, eFileType );
			}
		}

		return Result::FAIL;
	}

	CTexture2D* LoadTexture( const std::string& sFilePath, const Assets::FileType eType, int nSize )
	{
		CTexture2D* pTex = nullptr;
		auto pDevice = g_Editor.GetDevice();

		if ( (pTex = g_TextureManager.GetTexture2D( sFilePath )) ) {
			return pTex;
		}

		// // // 
		if ( eType == Assets::FileType::Palette ) {
			Assets::CPalette* pPalette = new CPalette();

			if ( !pPalette ) {
				g_ErrHandler.LogFmt( Log::Level::CRT, "Failed to create asset: %s", sFilePath );
				return nullptr;
			}

			Assets::QuickLoad( static_cast<void*>(pPalette), sFilePath, eType );

			if ( !pPalette->CreateTexture( pDevice ) ) {
				g_ErrHandler.LogFmt( Log::Level::ERR, "Failed to create texture for asset: %s", sFilePath );
				delete pPalette;
				return nullptr;
			}

			pTex = CopyTexture( pDevice, pPalette->GetTexture(), sFilePath, nSize );
			g_TextureManager.AddTexture( sFilePath, pTex );

			delete pPalette;
			return pTex;
		}
		// // // 

		Assets::CAsset* pAsset = nullptr;

		switch ( eType ) {
			case Assets::FileType::Alpha: pAsset = new Assets::CAlpha(); break;
			case Assets::FileType::BigFade: pAsset = new Assets::CBigFade(); break;
			case Assets::FileType::Blocks: pAsset = new Assets::CBlocks(); break;
			case Assets::FileType::Cliff: pAsset = new Assets::CCliff(); break;
			case Assets::FileType::Disp: pAsset = new Assets::CDisp(); break;
			case Assets::FileType::Fade: pAsset = new Assets::CFade(); break;
			case Assets::FileType::Ghost: pAsset = new Assets::CGhost(); break;
			case Assets::FileType::Level: pAsset = new Assets::CLevel(); break;
			case Assets::FileType::Sky: pAsset = new Assets::CSky(); break;
			default: return nullptr;
		}

		if ( !pAsset ) {
			g_ErrHandler.LogFmt( Log::Level::CRT, "Failed to create asset: %s", sFilePath );
			return nullptr;
		}

		Assets::QuickLoad( pAsset, sFilePath, eType );

		if ( eType == Assets::FileType::Blocks ) {
			static_cast<CBlocks*>(pAsset)->CreateTexture( pDevice, Blocks::k_uWidth / 4, Blocks::k_uHeight / 4 );
		}
		else
			pAsset->CreateTexture( pDevice );

		if ( !pAsset->GetTexture() ) {
			g_ErrHandler.LogFmt( Log::Level::ERR, "Failed to create texture for asset: %s", sFilePath );
			delete pAsset;
			return nullptr;
		}

		pTex = CopyTexture( pDevice, pAsset->GetTexture(), sFilePath, nSize );
		g_TextureManager.AddTexture( sFilePath, pTex );

		delete pAsset;
		return pTex;
	}
}

/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <fstream>
#include <assert.h>
#include <D3dx9tex.h>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Blocks.h"

namespace Assets
{
	using namespace Blocks;

	Result CBlocks::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CBlocks::LoadImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );
		BMP BMP;

		if ( BMP.ReadFromFile( sFilePath.c_str() ) ) {
			auto nWidth = BMP.TellWidth();
			auto nHeight = BMP.TellHeight();
			if ( nWidth != k_uWidth || nHeight != k_uHeight ) {
				g_ErrHandler.LogFmt( Log::Level::CRT, "LoadImg: Image dimensions mismatch. Got: %ix%i, Expected: %ux%u", nWidth, nHeight, k_uWidth, k_uHeight );
				return Result::FAIL_LOAD;
			}

			for ( auto y = 0; y < nHeight; y++ ) {
				for ( auto x = 0; x < nWidth; x++ ) {
					auto clr = BMP.GetPixel( x, y );
					m_Data[y * k_uWidth + x] = GetPalette()->FindColor( { clr.Red, clr.Green, clr.Blue } );
				}
			}

			const auto uAlpha = m_Data[k_uSize - 1];
			for ( size_t i = 0; i < k_uSize; i++ ) {
				if ( m_Data[i] == uAlpha ) {
					m_Data[i] = GetPalette()->FindColor( *GetPalette()->GetColor( uAlpha ) );
				}
			}

			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CBlocks::LoadSubImg( LPDIRECT3DDEVICE9 pD3DDevice, const std::string& sFilePath, size_t uIndex )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

		if ( uIndex >= k_uNumBlocks ) {
			assert( false && "CBlocks::LoadSubImg out of bounds" );
			return Result::FAIL_LOAD;
		}

		BMP BMP;
		if ( !BMP.ReadFromFile( sFilePath.c_str() ) ) {
			return Result::FAIL_LOAD;
		}

		auto nWidth = BMP.TellWidth();
		auto nHeight = BMP.TellHeight();

		if ( nWidth != k_uBlockWidth || nHeight != k_uBlockHeight ) {
			g_ErrHandler.LogFmt( Log::Level::CRT, "LoadSubImg: Image dimensions mismatch. Got: %ix%i, Expected: %ux%u", nWidth, nHeight, k_uBlockWidth, k_uBlockHeight );
			return Result::FAIL_LOAD;
		}

		size_t uCol = uIndex / (k_uWidth / k_uBlockWidth);
		size_t uRow = uIndex % (k_uWidth / k_uBlockWidth);
		for ( size_t y = 0; y < k_uBlockHeight; y++ ) {
			for ( size_t x = 0; x < k_uBlockWidth; x++ ) {
				auto clr = BMP.GetPixel( x, y );
				const auto uAtlasX = uRow * k_uBlockWidth + x;
				const auto uAtlasY = uCol * k_uBlockHeight + y;
				m_Data[uAtlasY * k_uWidth + uAtlasX] = GetPalette()->FindColor( { clr.Red, clr.Green, clr.Blue } );
			}
		}

		if ( m_pSubTextures[uIndex] ) {
			delete m_pSubTextures[uIndex];
		}

		CreateSubTexture( pD3DDevice, uIndex );
		
		return Result::OK_LOAD;
	}


	Result CBlocks::ExportImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

		BMP BMP;
		size_t uIndex = 0;
		auto pColorTable = GetPalette()->GetColorTable();

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				BMP.SetPixel( x, y,
							  {
								  pColorTable[m_Data[uIndex]].b,
								  pColorTable[m_Data[uIndex]].g,
								  pColorTable[m_Data[uIndex]].r,
								  0
							  } );
				uIndex++;
			}
		}

		auto sNewFilePath = Util::FileSystem::RemoveFileExtension( sFilePath ) + ".bmp";
		if ( !BMP.WriteToFile( sNewFilePath.c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	Result CBlocks::ExportSubImg( const std::string& sFilePath, size_t uIndex )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

		if ( uIndex >= k_uNumBlocks ) {
			assert( false && "CBlocks::ExportSubImg out of bounds" );
			return Result::FAIL_EXPORT;
		}

		BMP BMP;
		BMP.SetSize( k_uBlockWidth, k_uBlockHeight );
		BMP.SetBitDepth( 24 );

		size_t uCol = uIndex / (k_uWidth / k_uBlockWidth);
		size_t uRow = uIndex % (k_uWidth / k_uBlockWidth);
		auto pColorTable = GetPalette()->GetColorTable();

		for ( size_t y = 0; y < k_uBlockHeight; y++ ) {
			for ( size_t x = 0; x < k_uBlockWidth; x++ ) {
				const auto uAtlasX = uRow * k_uBlockWidth + x;
				const auto uAtlasY = uCol * k_uBlockHeight + y;
				Color* clr = &pColorTable[m_Data[uAtlasY * k_uWidth + uAtlasX]];
				BMP.SetPixel( x, y, { clr->b, clr->g, clr->r, 0 } );
			}
		}

		auto sNewFilePath = Util::FileSystem::RemoveFileExtension( sFilePath ) + "_" + std::to_string( uIndex ) + ".bmp";
		if ( !BMP.WriteToFile( sNewFilePath.c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	Result CBlocks::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
	}

	bool CBlocks::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], m_pPalette.get() );
		return true;
	}

	bool CBlocks::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice, int nWidth, int nHeight )
	{
		m_pTexture = new CTexture2D( pD3DDevice, nWidth, nHeight, &m_Data[0], m_pPalette.get() );
		return true;
	}

	bool CBlocks::CreateSubTexture( LPDIRECT3DDEVICE9 pD3DDevice, size_t uIndex )
	{
		if ( uIndex >= k_uNumBlocks ) {
			assert( false && "CBlocks::CreateSubTexture oob" );
			return false;
		}

		D3DLOCKED_RECT rc;
		size_t uCol = uIndex / (k_uWidth / k_uBlockWidth);
		size_t uRow = uIndex % (k_uWidth / k_uBlockWidth);
		auto pColorTable = GetPalette()->GetColorTable();

		m_pSubTextures[uIndex] = new CTexture2D( pD3DDevice, k_uBlockWidth, k_uBlockHeight );
		m_pSubTextures[uIndex]->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uBlockHeight; y++ ) {
			for ( size_t x = 0; x < k_uBlockWidth; x++ ) {
				const auto uAtlasX = uRow * k_uBlockWidth + x;
				const auto uAtlasY = uCol * k_uBlockHeight + y;
				Color* clr = &pColorTable[m_Data[uAtlasY * k_uWidth + uAtlasX]];
				WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
			}
		}

		m_pSubTextures[uIndex]->GetTexture()->UnlockRect( 0 );
		return true;
	}
}

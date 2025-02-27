/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <fstream>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Ghost.h"

namespace Assets
{
	using namespace Ghost;

	Result CGhost::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CGhost::LoadImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );
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

			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CGhost::ExportImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );

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

	Result CGhost::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
	}

	void CGhost::ComputeTable( uint8_t uOpacity )
	{
		if ( uOpacity > 100 ) uOpacity = 100;

		Color* pColorPalette = GetPalette()->GetColorTable();
		uint8_t* pData = &m_Data[0];
		Color blend;

		for ( uint32_t y = 0; y < k_uHeight; ++y ) {
			for ( uint32_t x = 0; x < k_uWidth; ++x ) {
				const Color& base = pColorPalette[y % Palette::k_uNumColors];
				const Color& target = pColorPalette[x % Palette::k_uNumColors];

				blend.r = base.r + ((target.r - base.r) * uOpacity) / 100;
				blend.g = base.g + ((target.g - base.g) * uOpacity) / 100;
				blend.b = base.b + ((target.b - base.b) * uOpacity) / 100;

				*pData++ = GetPalette()->FindColor( blend );
			}
		}
	}

	Result CGhost::Generate()
	{
		g_ErrHandler.SetFileType( FileType::Ghost );
		ComputeTable( m_uOpacity );
		DestroyTexture();
		return Result::OK_GENERATE;
	}

	bool CGhost::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], m_pPalette.get() );
		return true;
	}
}

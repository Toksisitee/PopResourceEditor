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
#include <algorithm>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Disp.h"

namespace Assets
{
	using namespace Disp;

	Result CDisp::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CDisp::LoadImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );
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
					uint8_t uGray = static_cast<uint8_t>(0.299 * clr.Red + 0.587 * clr.Green + 0.114 * clr.Blue);
					uGray -= k_uGrayscaleOffset;
					m_Data[y * k_uWidth + x] = uGray;
				}
			}

			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CDisp::ExportImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

		BMP BMP;
		size_t uIndex = 0;

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				BMP.SetPixel( x, y, RGBApixel(
					m_Data[uIndex] + k_uGrayscaleOffset,
					m_Data[uIndex] + k_uGrayscaleOffset,
					m_Data[uIndex] + k_uGrayscaleOffset,
					0 ) );
				uIndex++;
			}
		}

		auto sNewFilePath = Util::FileSystem::RemoveFileExtension( sFilePath ) + ".bmp";
		if ( !BMP.WriteToFile( sNewFilePath.c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	Result CDisp::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
	}

	Result CDisp::Generate( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

		// TODO
		assert( false && "TODO: missing impl." );

		return Result::OK_GENERATE;
	}

	bool CDisp::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );

		D3DLOCKED_RECT rc;
		m_pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );

		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uHeight; y++ ) {
			for ( size_t x = 0; x < k_uWidth; x++ ) {
				uint8_t uValue = m_Data[y * k_uWidth + x] + k_uGrayscaleOffset;
				size_t uTexelIndex = (y * rc.Pitch) + (x * 4);
				pTexels[uTexelIndex] = uValue;
				pTexels[uTexelIndex + 1] = uValue;
				pTexels[uTexelIndex + 2] = uValue;
				pTexels[uTexelIndex + 3] = 255;
			}
		}

		m_pTexture->GetTexture()->UnlockRect( 0 );

		return true;
	}
}

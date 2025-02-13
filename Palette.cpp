#include <stdint.h>
#include <fstream>
#include <cassert>
#include <d3d9.h>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"
#include "Palette.h"

namespace Assets
{
	using namespace Palette;

	Result CPalette::LoadBin( const std::string& file )
	{
		g_ErrHandler.SetFileType( FileType::Palette );

		char pad;
		std::ifstream ifs( file, std::ios::binary );

		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			for ( size_t i = 0; i < k_uNumColors; i++ ) {
				ifs.read( reinterpret_cast<char*>(&m_ColorTable[i].r), sizeof( char ) );
				ifs.read( reinterpret_cast<char*>(&m_ColorTable[i].g), sizeof( char ) );
				ifs.read( reinterpret_cast<char*>(&m_ColorTable[i].b), sizeof( char ) );
				ifs.read( &pad, sizeof( char ) );
			}
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CPalette::ExportImg( const char* pFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Palette );

		BMP BMP;
		size_t uIndex = 0;

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uColorsPerRow; y++ ) {
			for ( uint32_t x = 0; x < k_uColorsPerRow; x++ ) {
				for ( size_t h = 0; h < k_uCellScale; h++ ) {
					for ( size_t w = 0; w < k_uCellScale; w++ ) {
						BMP.SetPixel( x * k_uCellScale + w, y * k_uCellScale + h,
									  {
										m_ColorTable[uIndex].b,
										m_ColorTable[uIndex].g,
										m_ColorTable[uIndex].r,
										0
									  } );
					}
				}
				uIndex++;
			}
		}

		if ( !BMP.WriteToFile( pFilePath ) ) {
			return Result::FAIL_EXPORT;
		}

		g_ErrHandler.LogFmt( "Successfully exported as image: %s", pFilePath );
		return Result::OK_EXPORT;
	}

	bool CPalette::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
#if 0
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, this );
#else
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );

		D3DLOCKED_RECT rc;
		m_pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
		
		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);
		size_t uIndex = 0;
		for ( uint32_t y = 0; y < k_uColorsPerRow; y++ ) {
			for ( uint32_t x = 0; x < k_uColorsPerRow; x++ ) {
				for ( size_t h = 0; h < k_uCellScale; h++ ) {
					for ( size_t w = 0; w < k_uCellScale; w++ ) {
						size_t uTexelIndex = ((y * k_uCellScale + h) * rc.Pitch) + ((x * k_uCellScale + w) * 4);
						pTexels[uTexelIndex] = m_ColorTable[uIndex].b;
						pTexels[uTexelIndex + 1] = m_ColorTable[uIndex].g;
						pTexels[uTexelIndex + 2] = m_ColorTable[uIndex].r;
						pTexels[uTexelIndex + 3] = 255;
					}
				}
				uIndex++;
			}
		}

		m_pTexture->GetTexture()->UnlockRect( 0 );
#endif
		return true;
	}

	uint8_t CPalette::FindColor( const Color& clr, size_t uMin, size_t uMax )
	{
		int32_t dr, dg, db;
		int32_t nDistBest, nDist;
		size_t uIndex = 0;
		uint8_t* pptr;

		nDistBest = (clr.r * clr.r + clr.g * clr.g + clr.b * clr.b) * 2;

		pptr = &GetPtr()[uMin * 3];
		for ( size_t i = uMin; i <= uMax; i++ ) {
			dr = clr.r - *pptr++;
			dg = clr.g - *pptr++;
			db = clr.b - *pptr++;

			nDist = dr * dr + dg * dg + db * db;
			if ( nDist < nDistBest ) {
				if ( !nDist ) {
					return (uint8_t)i;
				}
				nDistBest = nDist;
				uIndex = i;
			}
		}

		return (uint8_t)uIndex;
	}

	uint8_t CPalette::FindExactColor( const Color& clr, bool bFallback )
	{
		for ( size_t i = 0; i < k_uNumColors; i++ ) {
			if ( std::memcmp( &clr, &m_ColorTable[i], sizeof( Color ) ) == 0 ) {
				return static_cast<uint8_t>(i);
			}
		}

		return bFallback ? FindColor( clr, 0, k_uNumColors ) : m_uColorKeys[0];
	}

	bool CPalette::IndexIsColorKey( size_t uIndex )
	{
		for ( size_t i = 0; i < (sizeof( m_uColorKeys ) / sizeof( m_uColorKeys[0] )); i++ ) {
			if ( uIndex == m_uColorKeys[i] )
				return true;
		}

		return false;
	}

	Color* CPalette::GetColorTable()
	{
		return &m_ColorTable[0];
	}

	uint8_t* CPalette::GetPtr()
	{
		return (uint8_t*)&m_ColorTable[0];
	}

	uint8_t CPalette::GetColorKey( size_t uSlot )
	{
		if ( uSlot < k_uNumColorKeys ) {
			m_uColorKeys[uSlot];
		}

		return m_uColorKeys[0];
	}
}
#include <stdint.h>
#include <fstream>
#include <cassert>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"
#include "AssetsErrHandler.h"
#include "Palette.h"

namespace Assets
{
	using namespace Palette;

	Result CPalette::Export( const char* pFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Palette );

		BMP BMP;
		size_t uIndex = 0;

		constexpr size_t k_uColorsPerRow = 16;
		constexpr size_t k_uCellScale = 8;
		const size_t k_uWidth = k_uCellScale * k_uColorsPerRow;
		const size_t k_uHeight = k_uCellScale * (k_uNumColors / k_uColorsPerRow);

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uColorsPerRow; y++ ) {
			for ( uint32_t x = 0; x < k_uColorsPerRow; x++ ) {
				for ( size_t h = 0; h < k_uCellScale; h++ ) {
					for ( size_t w = 0; w < k_uCellScale; w++ ) {
						BMP.SetPixel( x * k_uCellScale + w, y * k_uCellScale + h,
									  {
										m_ColorTable[uIndex].B,
										m_ColorTable[uIndex].G,
										m_ColorTable[uIndex].R,
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

	Result CPalette::Load( std::string& file )
	{
		g_ErrHandler.SetFileType( FileType::Palette );

		char pad;
		std::ifstream ifs( file, std::ios::binary );

		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			for ( size_t i = 0; i < k_uNumColors; i++ ) {
				ifs.read( reinterpret_cast<char*>(&m_ColorTable[i].R), sizeof( char ) );
				ifs.read( reinterpret_cast<char*>(&m_ColorTable[i].G), sizeof( char ) );
				ifs.read( reinterpret_cast<char*>(&m_ColorTable[i].B), sizeof( char ) );
				ifs.read( &pad, sizeof( char ) );
			}
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	uint8_t CPalette::FindClosestColor( const RGB& clr, bool bFullSearch )
	{
		uint8_t uIndex = m_uColorKeys[0];
		double dClosestDist = DBL_MAX;

		for ( size_t i = 0; i < k_uNumColors; i++ ) {
			auto deltaE = sqrt(
				pow( clr.R - m_ColorTable[i].R, 2 ) +
				pow( clr.G - m_ColorTable[i].G, 2 ) +
				pow( clr.B - m_ColorTable[i].B, 2 ) );

			if ( deltaE < dClosestDist ) {
				uIndex = (uint8_t)i;
				dClosestDist = deltaE;
			}

		}

		return uIndex;
	}

#if 0
	uint8_t CPalette::FindColor( const RGB& clr, bool bClosest )
	{
		if ( clr.R == m_ColorTable[0].R &&
			clr.G == m_ColorTable[0].G &&
			clr.B == m_ColorTable[0].B ) {
			return (0);
		}

		for ( uint32_t i = 128; i < k_uNumColors; i++ ) {
			if ( clr.R == m_ColorTable[i].R &&
				clr.G == m_ColorTable[i].G &&
				clr.B == m_ColorTable[i].B ) {
				return (i);
			}
		}

		// Fallback
		if ( bClosest ) {
			return FindClosestColor( clr );
		}

		return m_uColorKeys[0];
	}
#endif

	uint8_t CPalette::FindColor( uint8_t* pPalette, const RGB& clr, size_t uMin, size_t uMax )
	{
		int32_t dr, dg, db;
		int32_t nDistBest, nDist;
		size_t uIndex = 0;
		uint8_t* pptr;

		nDistBest = (clr.R * clr.R + clr.G * clr.G + clr.B * clr.B) * 2;

		pptr = &pPalette[uMin * 3];
		for ( size_t i = uMin; i <= uMax; i++ ) {
			dr = clr.R - *pptr++;
			dg = clr.G - *pptr++;
			db = clr.B - *pptr++;

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

	uint8_t CPalette::FindColorAll( const RGB& clr, bool bClosest )
	{
		for ( size_t i = 0; i < k_uNumColors; i++ ) {
			if ( clr.R == m_ColorTable[i].R &&
				clr.G == m_ColorTable[i].G &&
				clr.B == m_ColorTable[i].B ) {
				return (uint8_t)i;
			}
		}

		// Fallback
		if ( bClosest ) {
			return FindClosestColor( clr, true );
		}

		return m_uColorKeys[0];
	}

	uint8_t CPalette::FindBigFadeColor( const RGB& clr )
	{
		// TODO: avoid magic numbers
		for ( size_t i = 0; i < 112; i++ ) {
			if ( clr.R == m_ColorTable[i].R &&
				clr.G == m_ColorTable[i].G &&
				clr.B == m_ColorTable[i].B ) {
				return (uint8_t)i;
			}
		}

		return m_uColorKeys[0];
	}

	uint8_t CPalette::FindSkyColor( const RGB& clr )
	{
		// TODO: avoid magic numbers
		for ( size_t i = 112; i < 128; i++ ) {
			if ( clr.R == m_ColorTable[i].R &&
				clr.G == m_ColorTable[i].G &&
				clr.B == m_ColorTable[i].B ) {
				return (uint8_t)i;
			}
		}

		return m_uColorKeys[0];
	}

	bool CPalette::IndexIsColorKey( size_t uIndex )
	{
		for ( size_t i = 0; i < (sizeof( m_uColorKeys ) / sizeof( m_uColorKeys[0] )); i++ ) {
			if ( uIndex == m_uColorKeys[i] )
				return true;
		}

		return false;
	}

	RGB* CPalette::GetPalette()
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
#include <fstream>
#include <assert.h>
#include <set>

#include "EasyBMP/EasyBMP.h"

#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "BigFade.h"

namespace Assets
{
	using namespace BigFade;

	Result CBigFade::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::BigFade );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );

			for ( size_t i = 0; i < k_uWidth * k_uHeight; i++ )
				ifs.read( reinterpret_cast<char*>(&m_Data[i]), sizeof( uint8_t ) );

			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CBigFade::LoadImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::BigFade );
		BMP BMP;
		std::set<Color> colors;


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
					colors.insert( { clr.Red, clr.Green, clr.Blue } );
				}
			}

			if ( colors.size() > 112 ) {
				g_ErrHandler.LogFmt( Log::Level::ERR, "LoadImg: Too many unique colors (%i). Maximum allowed: %u", colors.size(), k_uNumColors );
				return Result::FAIL_LOAD;
			}

			if ( colors.size() < 112 ) {
				g_ErrHandler.LogFmt( Log::Level::WRN, "LoadImg: Image is not optimized. %i additional unique colors could still be used.", k_uNumColors - colors.size() );
			}

			for ( auto y = 0; y < nHeight; y++ ) {
				for ( auto x = 0; x < nWidth; x++ ) {
					auto clr = BMP.GetPixel( x, y );
					m_Data[y * k_uWidth + x] = m_Palette.FindColor( { clr.Red, clr.Green, clr.Blue }, 0, k_uNumColors );
				}
			}

			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CBigFade::ExportImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::BigFade );

		BMP BMP;
		size_t uIndex = 0;
		auto pColorTable = m_Palette.GetColorTable();

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				BMP.SetPixel( x, y,
							  {
								  pColorTable[m_Data[uIndex]].B,
								  pColorTable[m_Data[uIndex]].G,
								  pColorTable[m_Data[uIndex]].R,
								  0
							  } );
				uIndex++;
			}
		}

		if ( !BMP.WriteToFile( (sFilePath).c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	Result CBigFade::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::BigFade );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_GENERATE;
	}

	bool CBigFade::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}

	uint8_t CBigFade::FindColor( const Color& color )
	{
		for ( size_t i = 0; i < k_uNumColors; i++ ) {
			if ( std::memcmp( &color, m_Palette.GetColor( i ), sizeof( Color ) ) == 0 ) {
				return (uint8_t)i;
			}
		}
		return m_Palette.GetColorKey( 0 );
	}

	// TODO: Rename to "GetPal"?
	uint8_t CBigFade::GetColor( const uint32_t uIndex )
	{
		if ( uIndex > BigFade::k_uWidth * BigFade::k_uHeight ) {
			assert( false && "CBigFade: GetColor index out of bounds!" );
			return m_Data[0];
		}

		return m_Data[uIndex];
	}
}
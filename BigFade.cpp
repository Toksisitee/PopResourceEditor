#include <fstream>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "BigFade.h"

namespace Assets
{
	using namespace BigFade;

	Result CBigFade::Load( std::string& sFilePath )
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

	Result CBigFade::Export( std::string& sFilePath )
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

	// TODO: generate BigFade (raw data) from image
	Result CBigFade::Generate( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::BigFade );
		//BMP bmp;

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			for ( uint32_t y = 0; y < k_uHeight; y++ ) {
				for ( uint32_t x = 0; x < k_uWidth; x++ ) {
					//auto uIndex = m_Palette.FindBigFadeColor( color );
					//ofs.write( reinterpret_cast<const char*>(&uIndex), sizeof( char ) );
				}
				ofs.close();
			}
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
			if ( std::memcmp( &color, m_Palette.GetColor(i), sizeof( Color ) ) == 0 ) {
				return (uint8_t)i;
			}
		}
		return m_Palette.GetColorKey(0);
	}
}
#include <fstream>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Ghost.h"

namespace Assets
{
	using namespace Ghost;

	Result CGhost::Load( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );

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

	Result CGhost::Export( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );

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

	void CGhost::ComputeTable( uint8_t uOpacity )
	{
		if ( uOpacity > 100 ) uOpacity = 100;

		Color* pColorPalette = m_Palette.GetColorTable();
		uint8_t* pData = &m_Data[0];
		Color blend;

		for ( uint32_t y = 0; y < k_uHeight; ++y ) {
			for ( uint32_t x = 0; x < k_uWidth; ++x ) {
				const Color& base = pColorPalette[y % Palette::k_uNumColors];
				const Color& target = pColorPalette[x % Palette::k_uNumColors];

				blend.R = base.R + ((target.R - base.R) * uOpacity) / 100;
				blend.G = base.G + ((target.G - base.G) * uOpacity) / 100;
				blend.B = base.B + ((target.B - base.B) * uOpacity) / 100;

				*pData++ = m_Palette.FindColor( blend );
			}
		}
	}


	Result CGhost::Generate( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Ghost );

		ComputeTable( m_uOpacity );

		DestroyTexture();

#if 0
		std::ofstream ofs( fFilepath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(&m_Data), (k_uWidth * k_uHeight) );
			ofs.close();
			return Result::OK_GENERATE;
		}
#endif

		return Result::FAIL_GENERATE;
	}

	bool CGhost::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}
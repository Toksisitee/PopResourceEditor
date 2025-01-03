#include <fstream>
#include <assert.h>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Alpha.h"

namespace Assets
{
	using namespace Alpha;

	uint8_t auColors[16 * 3 + 3] =
	{
			245, 49, 37,
			247, 175, 0,
			245, 223, 204,
			0, 255, 0,
			0, 0, 255,
			251, 215, 51,
			145, 105, 70,
			175, 185, 255,

			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0,
			0, 0, 0
	};

	Result CAlpha::Load( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Alpha );

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


	Result CAlpha::Export( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Alpha );

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

	uint32_t CAlpha::ComputeTable( uint8_t* pData, const Color& targetColor, uint32_t uRow )
	{
		uint8_t r, g, b;

		for ( uint32_t y = 0; y < 16; y++ ) {
			Color* palette = m_Palette.GetColorTable();
			for ( uint32_t x = 0; x < k_uWidth; x++, palette++, pData++ ) {
				r = palette->R + ((targetColor.R - palette->R) * y) / 16;
				g = palette->G + ((targetColor.G - palette->G) * y) / 16;
				b = palette->B + ((targetColor.B - palette->B) * y) / 16;
				*pData = m_Palette.FindColor( Color( r, g, b ) );
			}
		}

		return uRow + 16;
	}

	Result CAlpha::Generate( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Alpha );

		uint8_t r, g, b;
		uint32_t i = 0;

		for ( uint32_t k = 0; k < 16; k++ ) {
			r = auColors[i++];
			g = auColors[i++];
			b = auColors[i++];

			uint32_t uRow = k * 16 * k_uWidth;
			ComputeTable( &m_Data[uRow], Color( r, g, b ), uRow );
		}

		DestroyTexture();

		return Result::OK_GENERATE;
	}

	Color CAlpha::GetAlphaColor( uint8_t uIndex )
	{
		if ( uIndex > 16 ) {
			assert( false && "CAlpha::GetAlphaColor oob index!" );
			uIndex = 0;
		}
		
		uint8_t i = uIndex * 3;
		return Color( auColors[i], auColors[i + 1], auColors[i + 2] );
	}

	bool CAlpha::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}
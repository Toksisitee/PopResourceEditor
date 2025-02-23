#include <fstream>
#include <assert.h>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
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

	Result CAlpha::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Alpha );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}


	Result CAlpha::ExportImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Alpha );

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

	Result CAlpha::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Alpha );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
	}

	uint32_t CAlpha::ComputeTable( uint8_t* pData, const Color& targetColor, uint32_t uRow )
	{
		uint8_t r, g, b;

		for ( uint32_t y = 0; y < 16; y++ ) {
			Color* palette = GetPalette()->GetColorTable();
			for ( uint32_t x = 0; x < k_uWidth; x++, palette++, pData++ ) {
				r = palette->r + ((targetColor.r - palette->r) * y) / 16;
				g = palette->g + ((targetColor.g - palette->g) * y) / 16;
				b = palette->b + ((targetColor.b - palette->b) * y) / 16;
				*pData = GetPalette()->FindColor( Color( r, g, b ) );
			}
		}

		return uRow + 16;
	}

	Result CAlpha::Generate()
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
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], m_pPalette.get() );
		return true;
	}
}
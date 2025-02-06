#include <fstream>
#include <assert.h>
#include <algorithm>

#include "EasyBMP/EasyBMP.h"

#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Fade.h"

namespace Assets
{
	using namespace Fade;

	Result CFade::Load( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Fade );

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


	Result CFade::Export( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Fade );

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

	void CFade::FadeColor( Color& color, float fFadeFactor )
	{
		color.R = std::clamp( static_cast<int32_t>(color.R * fFadeFactor), 0, 255 );
		color.G = std::clamp( static_cast<int32_t>(color.G * fFadeFactor), 0, 255 );
		color.B = std::clamp( static_cast<int32_t>(color.B * fFadeFactor), 0, 255 );
	}

	// TODO: This does not match Bullfrog's implementation
	void CFade::ComputeTable()
	{
		uint8_t* pData = &m_Data[0];
		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			Color* pPalette = m_Palette.GetColorTable();
			for ( uint32_t x = 0; x < k_uWidth; x++, pData++, pPalette++ ) {
				*pData = m_Palette.FindColor( *pPalette );
			}
		}

		pData = &m_Data[0];
		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			Color* pPalette = m_Palette.GetColorTable();
			float fRowFade = 0.1f + 0.6f * (static_cast<float>(y) / (k_uHeight - 1));
			for ( uint32_t x = 0; x < k_uWidth; x++, pData++, pPalette++ ) {
				float fColumnFade = 2.0f + (static_cast<float>(k_uWidth - x - 1) / k_uWidth) * 2.0f;
				float fFinalFade = std::clamp( fRowFade * fColumnFade, 0.0f, 3.0f );
				Color color = *pPalette;
				FadeColor( color, fFinalFade );
				*pData = m_Palette.FindColor( color );
			}
		}
	}

	Result CFade::Generate( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Fade );

		ComputeTable( );

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

	bool CFade::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}
#include <fstream>
#include <assert.h>
#include <algorithm>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Fade.h"

namespace Assets
{
	using namespace Fade;

	Result CFade::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Fade );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}


	Result CFade::ExportImg( const std::string& sFilePath )
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
								  pColorTable[m_Data[uIndex]].b,
								  pColorTable[m_Data[uIndex]].g,
								  pColorTable[m_Data[uIndex]].r,
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

	Result CFade::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Fade );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
	}

	void CFade::FadeColor( Color& color, float fFadeFactor )
	{
		color.r = std::clamp( static_cast<int32_t>(color.r * fFadeFactor), 0, 255 );
		color.g = std::clamp( static_cast<int32_t>(color.g * fFadeFactor), 0, 255 );
		color.b = std::clamp( static_cast<int32_t>(color.b * fFadeFactor), 0, 255 );
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
		g_ErrHandler.Log( Log::Level::WRN, "Fade generation does not exactly replicate Bullfrog's original algorithm." );
		ComputeTable();
		DestroyTexture();
		return Result::OK_GENERATE;
	}

	bool CFade::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}
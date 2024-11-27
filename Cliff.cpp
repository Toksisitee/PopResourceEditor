#include <fstream>
#include <assert.h>
#include <algorithm>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Cliff.h"

namespace Assets
{
	using namespace Cliff;

	Result CCliff::Load( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Cliff );

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


	Result CCliff::Export( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Cliff );

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

	Color CCliff::BlendColors( const Color& color1, const Color& color2, float fFadeFactor )
	{
		const float fBlendFactor = fFadeFactor * 0.6f;
		Color blended;

		blended.R = static_cast<uint8_t>(color1.R * (1 - fBlendFactor) + color2.R * fBlendFactor);
		blended.G = static_cast<uint8_t>(color1.G * (1 - fBlendFactor) + color2.G * fBlendFactor);
		blended.B = static_cast<uint8_t>(color1.B * (1 - fBlendFactor) + color2.B * fBlendFactor);

		return blended;
	}

	Color CCliff::IncreaseLuminance( const Color& color, float fLuminanceFactor )
	{
		Color boostedColor;
		const float fLuminance = 0.2126f * color.R + 0.7152f * color.G + 0.0722f * color.B;

		boostedColor.R = static_cast<uint8_t>(std::min( 255.0f, color.R + fLuminance * fLuminanceFactor ));
		boostedColor.G = static_cast<uint8_t>(std::min( 255.0f, color.G + fLuminance * fLuminanceFactor ));
		boostedColor.B = static_cast<uint8_t>(std::min( 255.0f, color.B + fLuminance * fLuminanceFactor ));

		return boostedColor;
	}

	// This does not match Bullfrog's implementation.
	void CCliff::ComputeTable( uint8_t uMode )
	{
		Color blendedColor;
		uint8_t* pData = &m_Data[0];
		Color ditherColor = *m_Palette.GetColor( 244 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			auto pPalette = m_Palette.GetColorTable();

			float fFade = static_cast<float>(y) / (k_uHeight - 1);
			fFade = fFade * fFade;
			float fLuminance = static_cast<float>(y) / (k_uHeight - 1);
			fLuminance = fLuminance * fLuminance;

			for ( uint32_t x = 0; x < k_uWidth; x++, pData++, pPalette++ ) {
				Color originalColor = *pPalette;

				if ( uMode == 0 ) {
					blendedColor = BlendColors( originalColor, ditherColor, fFade );
				}
				else if (uMode == 1) {
					blendedColor = BlendColors( originalColor, ditherColor, fFade );
					blendedColor = IncreaseLuminance( blendedColor, fLuminance );
					blendedColor = BlendColors( blendedColor, ditherColor, fFade );
				}
				else {
					blendedColor = IncreaseLuminance( originalColor, fLuminance );
					blendedColor = BlendColors( blendedColor, ditherColor, fFade );
				}

				*pData = m_Palette.FindClosestColor( blendedColor );
			}
		}
	}


	Result CCliff::Generate( uint8_t uMode )
	{
		g_ErrHandler.SetFileType( FileType::Cliff );
		ComputeTable( uMode );
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

	bool CCliff::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}
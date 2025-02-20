#include <fstream>
#include <assert.h>
#include <algorithm>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Cliff.h"

namespace Assets
{
	using namespace Cliff;

	Result CCliff::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Cliff );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CCliff::LoadImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Cliff );
		BMP BMP;

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
					m_Data[y * k_uWidth + x] = m_Palette.FindColor( { clr.Red, clr.Green, clr.Blue } );
				}
			}

			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CCliff::ExportImg( const std::string& sFilePath )
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

	Result CCliff::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Cliff );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
	}

	Color CCliff::BlendColors( const Color& color1, const Color& color2, const float fFadeFactor )
	{
		const float fBlendFactor = fFadeFactor * 0.6f;
		Color blended;

		blended.r = static_cast<uint8_t>(color1.r * (1 - fBlendFactor) + color2.r * fBlendFactor);
		blended.g = static_cast<uint8_t>(color1.g * (1 - fBlendFactor) + color2.g * fBlendFactor);
		blended.b = static_cast<uint8_t>(color1.b * (1 - fBlendFactor) + color2.b * fBlendFactor);

		return blended;
	}

	Color CCliff::IncreaseLuminance( const Color& color, const float fLuminanceFactor )
	{
		Color boostedColor;
		const float fLuminance = 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;

		boostedColor.r = static_cast<uint8_t>(std::min( 255.0f, color.r + fLuminance * fLuminanceFactor ));
		boostedColor.g = static_cast<uint8_t>(std::min( 255.0f, color.g + fLuminance * fLuminanceFactor ));
		boostedColor.b = static_cast<uint8_t>(std::min( 255.0f, color.b + fLuminance * fLuminanceFactor ));

		return boostedColor;
	}

	// TODO: This does not match Bullfrog's implementation.
	Result CCliff::ComputeTable( Cliff::Generation eMode )
	{
		if ( eMode >= Cliff::Generation::MAX ) {
			assert( false && "CCliff::ComputeTable eMode invalid!" );
			return Result::FAIL_GENERATE;
		}

		Color blendedColor;
		uint8_t* pData = &m_Data[0];
		const Color ditherColor = *m_Palette.GetColor( 244 );

		const float fCenter = static_cast<float>(k_uHeight) / 2.0f;

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			auto pPalette = m_Palette.GetColorTable();

			float fFade = static_cast<float>(y) / (k_uHeight - 1);
			fFade = fFade * fFade;

			float fLuminance = static_cast<float>(y) / (k_uHeight - 1);
			fLuminance = fLuminance * fLuminance;

			float fDistCenter = fabs( static_cast<float>(y) - fCenter ) / fCenter;
			float fMiddleFade = static_cast<float>(1.0f - pow( 1.0f - fDistCenter, 3 ));
			fLuminance *= fMiddleFade;

			for ( uint32_t x = 0; x < k_uWidth; x++, pData++, pPalette++ ) {
				float fColumnFade = pow( static_cast<float>(y) / (k_uHeight - 1), 3.0f );
				fColumnFade *= 1.5f;

				float fFinalLuminance = std::clamp( fLuminance + fColumnFade, 0.0f, 2.0f );

				Color originalColor = *pPalette;

				// TODO: temp uMode switch
				if ( eMode == Cliff::Generation::NO_LUMINANCE ) {
					blendedColor = BlendColors( originalColor, ditherColor, fFade );
				}
				else if ( eMode == Cliff::Generation::LUMINANCE_1 ) {
					blendedColor = BlendColors( originalColor, ditherColor, fFade );
					blendedColor = IncreaseLuminance( blendedColor, fFinalLuminance );
					blendedColor = BlendColors( blendedColor, ditherColor, fFade );
				}
				else if ( eMode == Cliff::Generation::LUMINANCE_2 ) {
					blendedColor = IncreaseLuminance( originalColor, fFinalLuminance );
					float fDitherStrength = 0.7f;
					blendedColor = BlendColors( blendedColor, ditherColor, fFade * fDitherStrength );
					blendedColor = BlendColors( blendedColor, ditherColor, fFade * fDitherStrength );
				}

				*pData = m_Palette.FindColor( blendedColor );
			}
		}

		return Result::OK_GENERATE;
	}

	Result CCliff::Generate( Cliff::Generation eMode )
	{
		Result result;
		g_ErrHandler.SetFileType( FileType::Cliff );
		g_ErrHandler.Log( Log::Level::WRN, "Cliff generation does not exactly replicate Bullfrog's original algorithm." );
		result = ComputeTable( eMode );
		if ( result == Result::OK_GENERATE ) {
			DestroyTexture();
		}
		return result;
	}

	bool CCliff::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}
}
#include <fstream>
#include <assert.h>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Fade.h"

namespace Assets
{
	using namespace Fade;

	Result CFade::Load( std::string& sFilePath )
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


	Result CFade::Export( std::string& sFilePath )
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

	void CFade::FadeDark( uint8_t* pPalette, uint8_t* pData, uint32_t uLights, uint32_t uDarkness )
	{
		// TODO
		assert( false && "Missing implementation" );
	}

	void CFade::FadeLight( uint8_t* pPalette, uint8_t* pData, uint32_t uLights, uint32_t uLightness )
	{
		// TODO
		assert( false && "Missing implementation" );
	}

	void CFade::FadeColor( uint8_t& r, uint8_t& g, uint8_t& b, float fFadeFactor )
	{
		r = std::clamp( static_cast<int32_t>(r * fFadeFactor), 0, 255 );
		g = std::clamp( static_cast<int32_t>(g * fFadeFactor), 0, 255 );
		b = std::clamp( static_cast<int32_t>(b * fFadeFactor), 0, 255 );
	}

	uint8_t CFade::GetLuminance( uint8_t r, uint8_t g, uint8_t b )
	{
		return static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
	}

	void CFade::GenerateTable()
	{
		// TODO
		assert( false && "Missing implementation" );
	}

	Result CFade::Generate( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Fade );

		GenerateTable( );

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
		auto pColorTable = m_Palette.GetColorTable();

		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );

		D3DLOCKED_RECT rc;
		m_pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );

		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uHeight; y++ ) {
			for ( size_t x = 0; x < k_uWidth; x++ ) {
				uint8_t paletteIndex = m_Data[y * k_uWidth + x];
				Color clr = pColorTable[paletteIndex];

				size_t iTexelIndex = (y * rc.Pitch) + (x * 4);
				pTexels[iTexelIndex] = clr.B;
				pTexels[iTexelIndex + 1] = clr.G;
				pTexels[iTexelIndex + 2] = clr.R;
				pTexels[iTexelIndex + 3] = 255;
			}
		}

		m_pTexture->GetTexture()->UnlockRect( 0 );

		return true;
	}
}
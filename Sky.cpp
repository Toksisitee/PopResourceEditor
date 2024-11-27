#include <fstream>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Sky.h"

namespace Assets
{
	using namespace Sky;

	Result CSky::Load( std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

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

	Result CSky::Export( std::string& fFilepath, Color* pColorTable )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

		BMP BMP;
		size_t uIndex = 0;

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				BMP.SetPixel( x, y,
							  {
								  pColorTable[m_Data[uIndex] + k_uColorStart].B,
								  pColorTable[m_Data[uIndex] + k_uColorStart].G,
								  pColorTable[m_Data[uIndex] + k_uColorStart].R,
								  0
							  } );
				uIndex++;
			}
		}

		if ( !BMP.WriteToFile( (fFilepath).c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	Result CSky::Generate( std::string& fFilepath, CPalette* pPalette )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

		std::ofstream ofs( fFilepath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			for ( uint32_t y = 0; y < k_uHeight; y++ ) {
				for ( uint32_t x = 0; x < k_uWidth; x++ ) {
					/*
					auto rgb = bmp->GetPixel( x, y );
					auto uIndex = pPalette->FindSkyColor( { rgb.R, rgb.G, rgb.B } );
					uIndex = uIndex - k_uColorStart;
					ofs.write( reinterpret_cast<const char*>(&uIndex), sizeof( char ) );
					*/
				}
			}

			ofs.close();
			return Result::OK_GENERATE;
		}

		return Result::FAIL_GENERATE;
	}

	bool CSky::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice, CPalette* pPalette )
	{
		auto pColorTable = pPalette->GetColorTable();

		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );

		D3DLOCKED_RECT rc;
		m_pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );

		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uHeight; y++ ) {
			for ( size_t x = 0; x < k_uWidth; x++ ) {
				size_t index = y * k_uWidth + x;
				uint8_t paletteIndex = m_Data[index] + k_uColorStart;

				Color clr = pColorTable[paletteIndex];

				size_t uTexelIndex = (y * rc.Pitch) + (x * 4);
				pTexels[uTexelIndex] = clr.B;
				pTexels[uTexelIndex + 1] = clr.G;
				pTexels[uTexelIndex + 2] = clr.R;
				pTexels[uTexelIndex + 3] = 255;
			}
		}

		m_pTexture->GetTexture()->UnlockRect( 0 );

		return true;
	}

}
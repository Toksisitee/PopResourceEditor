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

	Result CSky::Export( std::string& fFilepath )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

		BMP BMP;
		size_t uIndex = 0;
		auto pColorTable = m_Palette.GetColorTable();

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

	Result CSky::Generate( std::string& fFilepath )
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

	bool CSky::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		D3DLOCKED_RECT rc;
		auto pColorTable = m_Palette.GetColorTable();

		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );
		m_pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uHeight; y++ ) {
			for ( size_t x = 0; x < k_uWidth; x++ ) {
				uint8_t paletteIndex = m_Data[y * k_uWidth + x] + k_uColorStart;
				Color* clr = &pColorTable[paletteIndex];
				WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
			}
		}

		m_pTexture->GetTexture()->UnlockRect( 0 );

		return true;
	}

	uint8_t CSky::FindColor( const Color& color )
	{
		for ( size_t i = k_uColorStart; i < k_uColorStart + k_uNumColors; i++ ) {
			if ( std::memcmp( &color, m_Palette.GetColor( i ), sizeof( Color ) ) == 0 ) {
				return (uint8_t)i;
			}
		}
		return m_Palette.GetColorKey( 0 );
	}
}
#include <fstream>
#include <D3dx9tex.h>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Utils.h"

#include "Sky.h"

namespace Assets
{
	using namespace Sky;

	Result CSky::LoadBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

		std::ifstream ifs( sFilePath, std::ios::binary );
		if ( ifs.is_open() ) {
			ifs.seekg( 0 );
			ifs.read( reinterpret_cast<char*>(&m_Data[0]), sizeof( m_Data ) );
			ifs.close();
			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CSky::ExportImg( const std::string& sFilePath )
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
								  pColorTable[m_Data[uIndex] + k_uColorStart].b,
								  pColorTable[m_Data[uIndex] + k_uColorStart].g,
								  pColorTable[m_Data[uIndex] + k_uColorStart].r,
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

	Result CSky::ExportBin( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

		std::ofstream ofs( sFilePath, std::ios::binary | std::ios::trunc );
		if ( ofs.is_open() ) {
			ofs.write( reinterpret_cast<const char*>(m_Data), sizeof( m_Data ) );
			ofs.close();
			return Result::OK_EXPORT;
		}

		return Result::FAIL_EXPORT;
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
		for ( auto i = k_uColorStart; i < k_uColorStart + k_uNumColors; i++ ) {
			if ( std::memcmp( &color, m_Palette.GetColor( i ), sizeof( Color ) ) == 0 ) {
				return static_cast<uint8_t>(i);
			}
		}
		return m_Palette.GetColorKey( 0 );
	}
}
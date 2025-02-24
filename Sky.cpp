#include <fstream>
#include <D3dx9tex.h>
#include <set>
#include <vector>
#include <algorithm>

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

	Result CSky::LoadImg( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Sky );
		BMP BMP;
		std::set<Color> colors;


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
					colors.insert( { clr.Red, clr.Green, clr.Blue } );
				}
			}

			if ( colors.size() > k_uNumColors ) {
				g_ErrHandler.LogFmt( Log::Level::ERR, "LoadImg: Too many unique colors (%i). Maximum allowed: %u", colors.size(), k_uNumColors );
				return Result::FAIL_LOAD;
			}

			if ( colors.size() < k_uNumColors ) {
				g_ErrHandler.LogFmt( Log::Level::WRN, "LoadImg: Image is not optimized. %i additional unique colors could still be used.", k_uNumColors - colors.size() );
			}

			size_t uIndex = k_uColorStart;
			if ( k_uNumColors - colors.size() == 2 ) {
				auto oldColor = GetPalette()->GetColor( static_cast<uint8_t>(k_uColorStart) );
				*oldColor = Color{ 255, 0, 255 };
				oldColor = GetPalette()->GetColor( static_cast<uint8_t>(k_uColorStart + k_uNumColors - 1) );
				*oldColor = Color{ 255, 0, 255 };
				uIndex = k_uColorStart + 1;
			}

#define SORT_COLORS
#ifdef SORT_COLORS
			std::vector<Color> vSortedColors( colors.begin(), colors.end() );
			std::sort( vSortedColors.begin(), vSortedColors.end(),
				[]( const Color& a, const Color& b ) {
				return a.GetLuminance() < b.GetLuminance();
			} );

			std::vector<Color>::iterator it;
			for ( it = vSortedColors.begin(); it != vSortedColors.end(); it++ ) {
#else
			std::set<Color>::iterator it;
			for ( it = colors.begin(); it != colors.end(); it++ ) {
#endif
				auto oldColor = GetPalette()->GetColor( static_cast<uint8_t>(uIndex) );
				*oldColor = *it;
				uIndex++;
			}
#undef SORT_COLORS

			for ( auto y = 0; y < nHeight; y++ ) {
				for ( auto x = 0; x < nWidth; x++ ) {
					auto clr = BMP.GetPixel( x, y );
					m_Data[y * k_uWidth + x] = FindColor( { clr.Red, clr.Green, clr.Blue } ) - k_uColorStart;
				}
			}

			m_pPalette.get()->SetChanged( true );

			return Result::OK_LOAD;
		}

		return Result::FAIL_LOAD;
	}

	Result CSky::ExportImg( const std::string & sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Sky );

		BMP BMP;
		size_t uIndex = 0;
		auto pColorTable = GetPalette()->GetColorTable();

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

	Result CSky::ExportBin( const std::string & sFilePath )
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
		auto pColorTable = GetPalette()->GetColorTable();

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

	uint8_t CSky::FindColor( const Color & color )
	{
		for ( auto i = k_uColorStart; i < k_uColorStart + k_uNumColors; i++ ) {
			if ( std::memcmp( &color, GetPalette()->GetColor( i ), sizeof( Color ) ) == 0 ) {
				return static_cast<uint8_t>(i);
			}
		}
		return GetPalette()->GetColorKey( 0 );
	}
}
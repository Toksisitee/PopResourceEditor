#include <fstream>
#include <assert.h>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Disp.h"

namespace Assets
{
	using namespace Disp;

	Result CDisp::Load( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

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


	Result CDisp::Export( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

		BMP BMP;
		size_t uIndex = 0;

		BMP.SetSize( k_uWidth, k_uHeight );
		BMP.SetBitDepth( 24 );

		for ( uint32_t y = 0; y < k_uHeight; y++ ) {
			for ( uint32_t x = 0; x < k_uWidth; x++ ) {
				BMP.SetPixel( x, y, RGBApixel(
					m_Data[uIndex] + k_uGrayscaleOffset,
					m_Data[uIndex] + k_uGrayscaleOffset,
					m_Data[uIndex] + k_uGrayscaleOffset,
					0 ) );
				uIndex++;
			}
		}

		if ( !BMP.WriteToFile( (sFilePath).c_str() ) ) {
			return Result::FAIL_EXPORT;
		}

		return Result::OK_EXPORT;
	}

	Result CDisp::Generate( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Disp );

		// TODO
		assert( false && "TODO: missing impl." );

		return Result::OK_GENERATE;
	}

	bool CDisp::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );

		D3DLOCKED_RECT rc;
		m_pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );

		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uHeight; y++ ) {
			for ( size_t x = 0; x < k_uWidth; x++ ) {
				uint8_t uValue = m_Data[y * k_uWidth + x] + k_uGrayscaleOffset;
				size_t uTexelIndex = (y * rc.Pitch) + (x * 4);
				pTexels[uTexelIndex] = uValue;
				pTexels[uTexelIndex + 1] = uValue;
				pTexels[uTexelIndex + 2] = uValue;
				pTexels[uTexelIndex + 3] = 255;
			}
		}

		m_pTexture->GetTexture()->UnlockRect( 0 );

		return true;
	}
}
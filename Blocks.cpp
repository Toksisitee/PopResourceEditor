#include <fstream>
#include <assert.h>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"

#include "Blocks.h"

namespace Assets
{
	using namespace Blocks;

	Result CBlocks::Load( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

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

	Result CBlocks::Export( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

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

	Result CBlocks::Generate( const std::string& sFilePath )
	{
		g_ErrHandler.SetFileType( FileType::Blocks );

		DestroyTextures();

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

	bool CBlocks::CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice )
	{
		m_pTexture = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight, &m_Data[0], &m_Palette );
		return true;
	}

	bool CBlocks::CreateSubTexture( LPDIRECT3DDEVICE9 pD3DDevice, size_t uIndex )
	{
		if ( uIndex >= k_uNumBlocks ) {
			assert( false && "CBlocks::CreateSubTexture oob" );
			return false;
		}

		D3DLOCKED_RECT rc;
		size_t uRow = uIndex / (k_uWidth / k_uBlockWidth);
		size_t uCol = uIndex % (k_uWidth / k_uBlockWidth);
		auto pColorTable = m_Palette.GetColorTable();

		m_pSubTextures[uIndex] = new CTexture2D( pD3DDevice, k_uBlockWidth, k_uBlockHeight );
		m_pSubTextures[uIndex]->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
		BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

		for ( size_t y = 0; y < k_uBlockHeight; y++ ) {
			for ( size_t x = 0; x < k_uBlockWidth; x++ ) {
				const auto atlasX = uRow * k_uBlockWidth + x;
				const auto atlasY = uCol * k_uBlockHeight + y;
				Color* clr = &pColorTable[m_Data[atlasY * k_uWidth + atlasX]];
				WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
			}
		}

		m_pSubTextures[uIndex]->GetTexture()->UnlockRect( 0 );
		return true;
	}

}
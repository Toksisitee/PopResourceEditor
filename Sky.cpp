#include <fstream>

#include "EasyBMP/EasyBMP.h"

#include "Editor.h"
#include "Utils.h"
#include "Assets.h"
#include "AssetsErrHandler.h"
#include "Palette.h"

#include "Sky.h"

namespace Assets
{
	namespace Sky
	{
		Result CSky::Load( std::string& sFilePath )
		{
			CErrHandler::Instance().SetFileType( FileType::Sky );

			std::ifstream ifs( sFilePath, std::ios::binary );
			if ( ifs.is_open() ) {
				ifs.seekg( 0 );

				for ( size_t i = 0; i < k_uWidth * k_uWidth; i++ )
					ifs.read( reinterpret_cast<char*>(&m_Data[i]), sizeof( uint8_t ) );

				ifs.close();
				return Result::OK_LOAD;
			}

			return Result::FAIL_LOAD;
		}

		Result CSky::Export( std::string& fFilepath, RGB* pColorTable )
		{
			CErrHandler::Instance().SetFileType( FileType::Sky );
			
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
			CErrHandler::Instance().SetFileType( FileType::Sky );

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

	}
}
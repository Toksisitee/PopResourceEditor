#include <cstdint>
#include <fstream>
#include <algorithm>

#include "Utils.h"
#include "AssetsErrHandler.h"
#include "Editor.h"
#include "Sprite.h"

namespace Assets
{
	namespace Sprite
	{
		Result CSprite::LoadBank( const std::string& file )
		{
			g_ErrHandler.SetFileType( Assets::FileType::Sprite );

			if ( SprBank.Data.size() > 0 )
				this->Clear();

			std::ifstream ifs( file, std::ios::binary | std::ios::ate );
			if ( ifs.is_open() ) {
				m_nBufferLength = static_cast<uint32_t>(ifs.tellg());
				m_pBuffer = new char[m_nBufferLength];
				char* buffer = m_pBuffer;
				ifs.seekg( std::ios::beg );
				ifs.read( m_pBuffer, m_nBufferLength );
				ifs.close();

				SprBank.Header = *(reinterpret_cast<CSprite::BankHeader*>(buffer));
				buffer += sizeof( CSprite::BankHeader );

				if ( memcmp( SprBank.Header.Magic, "PSFB", 4 ) != 0 ) {
					g_ErrHandler.LogFmt( "Wrong magic number %s. Not a valid sprite bank?\n", SprBank.Header.Magic );
					this->Clear();
					return Result::FAIL_LOAD;
				}

				if ( SprBank.Header.Frames == 0 ) {
					g_ErrHandler.Log( "Couldn't find any sprite frames..\n" );
					this->Clear();
					return Result::FAIL_LOAD;
				}

				g_ErrHandler.LogFmt( "Found %i sprite frames.\n", SprBank.Header.Frames );

				for ( uint32_t i = 0; i < SprBank.Header.Frames; i++ ) {
					SprBank.Data.push_back( { *(reinterpret_cast<CSprite::TbSprite*>(buffer)) } );
					SprBank.Data.back().Data = reinterpret_cast<int8_t*>(m_pBuffer + SprBank.Data.back().Sprite.Offset);
					buffer += sizeof( CSprite::TbSprite );
#if 0
					printf( "#%i Height: %i, Width: %i, Offsets: (H)%06X && (D)%06X\n", i, SprBank.Data.back().Sprite.Height, SprBank.Data.back().Sprite.Width, ((1 + i) * sizeof( CSprite::BankHeader )), SprBank.Data.back().Sprite.Offset );
#endif
				}
				g_ErrHandler.Log( "Opened sprite bank file.\n" );
				return Result::OK_LOAD;
			}

			g_ErrHandler.LogFmt( "Could not open sprite bank: %s.\n", file.c_str() );

			if ( !Util::FileSystem::FileExists( file ) ) {
				g_ErrHandler.Log( "File does not exist\n" );
			}

			return Result::FAIL_LOAD;
		}

		void CSprite::MapSprite( uint16_t uIndex)
		{
			int8_t b;

			if ( SprBank.Data[uIndex].Map.size() < 1 ) {
				SprBank.Data[uIndex].Map.resize( SprBank.Data[uIndex].Sprite.Width, std::vector<int32_t>( SprBank.Data[uIndex].Sprite.Height, 0 ) );

				for ( uint32_t x = 0, y = 0, p = 0; y < SprBank.Data[uIndex].Sprite.Height; y++, x = 0 ) {
					while ( true ) {
						b = SprBank.Data[uIndex].Data[p];
						p++;

						if ( b == 0 ) {
							SprBank.Data[uIndex].Raw.push_back( { static_cast<uint8_t>(b), REND } );
							for ( ; x < SprBank.Data[uIndex].Sprite.Width; x++ )
								SprBank.Data[uIndex].Map[x][y] = m_pPalette->GetColorKey();
							break;
						}

						if ( b < 0 ) {
							SprBank.Data[uIndex].Raw.push_back( { static_cast<uint8_t>(b), RSKIP } );
							for ( uint32_t i = 0; i <= (-b); i++ )
								if ( (x + i) < SprBank.Data[uIndex].Sprite.Width )
									SprBank.Data[uIndex].Map[x + i][y] = m_pPalette->GetColorKey();

							x += -b;
						}
						else if ( b > 0 ) {
							SprBank.Data[uIndex].Raw.push_back( { static_cast<uint8_t>(b), RREAD } );

							for ( uint32_t i = 0, n = b; i < n; i++ ) {
								if ( x < SprBank.Data[uIndex].Sprite.Width && y < SprBank.Data[uIndex].Sprite.Height ) {
									b = SprBank.Data[uIndex].Data[p];
									SprBank.Data[uIndex].Map[x][y] = b;
									SprBank.Data[uIndex].Raw.push_back( { static_cast<uint8_t>(b), RPAL } );
									p++;
									x++;
								}

							}
						}
					}
				}
			}
		}

		void CSprite::ExportSprite( uint16_t uIndex )
		{
			auto pColorTable = m_pPalette->GetPalette();
			uint8_t uColorIndex;
			uint16_t uMin = 1;
			BMP BMP;

			BMP.SetSize( std::max( uMin, SprBank.Data[uIndex].Sprite.Width ), std::max( uMin, SprBank.Data[uIndex].Sprite.Height ) );
			BMP.SetBitDepth( 24 );



			MapSprite( uIndex );

			for ( uint16_t x = 0; x < SprBank.Data[uIndex].Sprite.Width; x++ ) {
				for ( uint16_t y = 0; y < SprBank.Data[uIndex].Sprite.Height; y++ ) {
					uColorIndex = SprBank.Data[uIndex].Map[x][y];

					if ( m_pPalette->IndexIsColorKey( uColorIndex ) )
						uColorIndex = m_pPalette->GetColorKey(0);

					BMP.SetPixel( x, y, { pColorTable[uColorIndex].B, pColorTable[uColorIndex].G, pColorTable[uColorIndex].R, 0 } );
				}
			}

			//BMP.WriteToFile( (Util::FileSystem::GetCurrentDir() + "//output//" + std::to_string( uIndex ) + ".bmp").c_str() );
		}

		void CSprite::ExportSprites()
		{
			if ( SprBank.Header.Frames > 0 ) {
				for ( uint32_t i = 0; i < SprBank.Header.Frames; i++ ) {
					ExportSprite( i );
				}
			}
		}


		bool CSprite::IsPixelEmpty( const RGBApixel& rgb )
		{
			if ( rgb.Red == 0 &&
				rgb.Green == 255 &&
				rgb.Blue == 255 ) {
				return true;
			}

			return false;
		}

		bool CSprite::IsPixelColorKey( const RGBApixel& rgb )
		{
			if ( rgb.Red == 255 &&
				rgb.Green == 0 &&
				rgb.Blue == 255 ) {
				return true;
			}

			return false;
		}

		bool CSprite::IsAlphaSprite( uint32_t index )
		{
			if ( m_IsHFX ) {
				if ( index >= 1090 && index <= 1499 ||
					index >= 1538 && index <= 1592 ) {
					return true;
				}
			}

			return false;
		}
	}
}
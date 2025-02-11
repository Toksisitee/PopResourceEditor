#include <cstdint>
#include <fstream>
#include <algorithm>
#include <assert.h>
#include <D3dx9tex.h>

#include "EasyBMP/EasyBMP.h"

#include "Texture.h"
#include "AssetsErrHandler.h"
#include "Utils.h"
#include "Editor.h"
#include "Sprite.h"

namespace Assets
{
	namespace Sprite
	{
		Result CSprite::Load( const std::string& file )
		{
			g_ErrHandler.SetFileType( Assets::FileType::Sprite );

			if ( Bank.Frames.size() > 0 )
				this->Clear();

			std::ifstream ifs( file, std::ios::binary | std::ios::ate );
			if ( ifs.is_open() ) {
				m_nBufferLength = static_cast<uint32_t>(ifs.tellg());
				m_pBuffer = new char[m_nBufferLength];
				char* buffer = m_pBuffer;
				ifs.seekg( std::ios::beg );
				ifs.read( m_pBuffer, m_nBufferLength );
				ifs.close();

				Bank.Header = *(reinterpret_cast<BankHeader*>(buffer));
				buffer += sizeof( BankHeader );

				if ( memcmp( Bank.Header.Magic, "PSFB", 4 ) != 0 ) {
					g_ErrHandler.LogFmt( "Wrong magic number %s. Not a valid sprite bank?\n", Bank.Header.Magic );
					this->Clear();
					return Result::FAIL_LOAD;
				}

				if ( Bank.Header.Count <= 0 ) {
					g_ErrHandler.Log( "Couldn't find any sprite frames..\n" );
					this->Clear();
					return Result::FAIL_LOAD;
				}

				g_ErrHandler.LogFmt( "Found %i sprite frames.\n", Bank.Header.Count );

				for ( uint32_t i = 0; i < Bank.Header.Count; i++ ) {
					Bank.Frames.push_back( { *(reinterpret_cast<SpriteInfo*>(buffer)) } );
					Bank.Frames.back().Data = reinterpret_cast<int8_t*>(m_pBuffer + Bank.Frames.back().Sprite.Offset);
					buffer += sizeof( SpriteInfo );
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

		void CSprite::Map( uint16_t uIndex )
		{
			int8_t b;

			if ( Bank.Frames[uIndex].Map.size() == 0 ) {
				const auto k_uWidth = Bank.Frames[uIndex].Sprite.Width;
				const auto k_uHeight = Bank.Frames[uIndex].Sprite.Height;
				Bank.Frames[uIndex].Map.resize( k_uWidth, decltype(Bank.Frames[uIndex].Map)::value_type( k_uHeight, 0 ) );

				for ( uint32_t x = 0, y = 0, p = 0; y < k_uHeight; y++, x = 0 ) {
					while ( true ) {
						b = Bank.Frames[uIndex].Data[p];
						p++;

						if ( b == 0 ) {
							for ( ; x < k_uWidth; x++ )
								Bank.Frames[uIndex].Map[x][y] = m_pPalette->GetColorKey();
							break;
						}

						if ( b < 0 ) {
							for ( uint32_t i = 0; i <= (-b); i++ )
								if ( (x + i) < k_uWidth )
									Bank.Frames[uIndex].Map[x + i][y] = m_pPalette->GetColorKey();
							x += -b;
						}
						else if ( b > 0 ) {
							for ( uint32_t i = 0, n = b; i < n; i++ ) {
								if ( x < k_uWidth && y < k_uHeight ) {
									b = Bank.Frames[uIndex].Data[p];
									Bank.Frames[uIndex].Map[x][y] = b;
									p++;
									x++;
								}
							}
						}
					}
				}
			}
		}

		void CSprite::ExportImg( uint16_t uIndex )
		{
			auto pColorTable = m_pPalette->GetColorTable();
			uint8_t uColorIndex;
			uint16_t uMin = 1;
			BMP BMP;


			const auto k_uWidth = Bank.Frames[uIndex].Sprite.Width;
			const auto k_uHeight = Bank.Frames[uIndex].Sprite.Height;

			BMP.SetSize( std::max( uMin, k_uWidth ), std::max( uMin, k_uHeight ) );
			BMP.SetBitDepth( 24 );



			Map( uIndex );

			for ( uint16_t x = 0; x < k_uWidth; x++ ) {
				for ( uint16_t y = 0; y < k_uHeight; y++ ) {
					uColorIndex = Bank.Frames[uIndex].Map[x][y];

					if ( m_pPalette->IndexIsColorKey( uColorIndex ) )
						uColorIndex = m_pPalette->GetColorKey( 0 );

					BMP.SetPixel( x, y, { pColorTable[uColorIndex].b, pColorTable[uColorIndex].g, pColorTable[uColorIndex].r, 0 } );
				}
			}

			//BMP.WriteToFile( (Util::FileSystem::GetCurrentDir() + "//output//" + std::to_string( uIndex ) + ".bmp").c_str() );
		}


		void CSprite::CreateTextures( LPDIRECT3DDEVICE9 pD3DDevice )
		{
			assert( m_pPalette != nullptr && "m_pPalette is nullptr" );
			assert( pD3DDevice != nullptr && "pD3DDevice is nullptr" );
			assert( m_Textures.size() == 0 && "Sprite textures were already created" );

			const auto pColorTable = m_pPalette->GetColorTable();
			CTexture2D* pTexture = nullptr;
			BYTE* pTexels = nullptr;
			D3DLOCKED_RECT rc;

			if ( m_Textures.size() > 0 ) {
				// TODO: log
				return;
			}

			m_Textures.resize( Bank.Header.Count );
			for ( uint32_t i = 0; i < Bank.Header.Count; i++ ) {

				const auto k_uWidth = Bank.Frames[i].Sprite.Width;
				const auto k_uHeight = Bank.Frames[i].Sprite.Height;

				if ( !IsValid( Bank.Frames[i].Sprite ) ) {
					// TODO: log?
					continue;
				}

				Map( i );
				m_Textures[i] = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );
				pTexture = m_Textures[i];

				pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
				pTexels = static_cast<BYTE*>(rc.pBits);

				for ( uint16_t y = 0; y < k_uHeight; y++ ) {
					for ( uint16_t x = 0; x < k_uWidth; x++ ) {
						auto index = Bank.Frames[i].Map[x][y];
						Color clr = pColorTable[index];
						WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
					}
				}

				pTexture->GetTexture()->UnlockRect( 0 );
			}
		}

		// TODO: use flags?
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

		void CSprite::Clear()
		{
			m_nBufferLength = 0;
			delete[] m_pBuffer;
			for ( size_t i = 0; i < m_Textures.size(); i++ ) {
				m_Textures[i]->Clear();
			}
			Bank.Frames.clear();
			m_IsHFX = false;
		}
	}
}
/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

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
		Result CSprite::LoadBin( const std::string& file )
		{
			g_ErrHandler.SetFileType( Assets::FileType::Sprite );

			if ( m_Bank.Frames.size() > 0 )
				this->Reset();

			std::ifstream ifs( file, std::ios::binary | std::ios::ate );
			if ( ifs.is_open() ) {
				m_nBufferLength = static_cast<uint32_t>(ifs.tellg());
				m_pBuffer = new char[m_nBufferLength];
				char* buffer = m_pBuffer;
				ifs.seekg( std::ios::beg );
				ifs.read( m_pBuffer, m_nBufferLength );
				ifs.close();

				m_Bank.Header = *(reinterpret_cast<BankHeader*>(buffer));
				buffer += sizeof( BankHeader );

				if ( memcmp( m_Bank.Header.Magic, "PSFB", 4 ) != 0 ) {
					g_ErrHandler.LogFmt( "Wrong magic number %s. Not a valid sprite bank?\n", m_Bank.Header.Magic );
					this->Reset();
					return Result::FAIL_LOAD;
				}

				if ( m_Bank.Header.Count <= 0 ) {
					g_ErrHandler.Log( "Couldn't find any sprite frames..\n" );
					this->Reset();
					return Result::FAIL_LOAD;
				}

				g_ErrHandler.LogFmt( "Found %i sprite frames.\n", m_Bank.Header.Count );

				for ( uint32_t i = 0; i < m_Bank.Header.Count; i++ ) {
					m_Bank.Frames.push_back( { *(reinterpret_cast<SpriteInfo*>(buffer)) } );
					m_Bank.Frames.back().Data = reinterpret_cast<int8_t*>(m_pBuffer + m_Bank.Frames.back().Sprite.Offset);
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

			if ( m_Bank.Frames[uIndex].Map.size() == 0 ) {
				const auto k_uWidth = m_Bank.Frames[uIndex].Sprite.Width;
				const auto k_uHeight = m_Bank.Frames[uIndex].Sprite.Height;
				m_Bank.Frames[uIndex].Map.resize( k_uWidth, decltype(m_Bank.Frames[uIndex].Map)::value_type( k_uHeight, 0 ) );

				for ( uint32_t x = 0, y = 0, p = 0; y < k_uHeight; y++, x = 0 ) {
					while ( true ) {
						b = m_Bank.Frames[uIndex].Data[p];
						p++;

						if ( b == 0 ) {
							for ( ; x < k_uWidth; x++ )
								m_Bank.Frames[uIndex].Map[x][y] = m_pPalette->GetColorKey();
							break;
						}

						if ( b < 0 ) {
							for ( uint32_t i = 0; i <= (-b); i++ )
								if ( (x + i) < k_uWidth )
									m_Bank.Frames[uIndex].Map[x + i][y] = m_pPalette->GetColorKey();
							x += -b;
						}
						else if ( b > 0 ) {
							for ( uint32_t i = 0, n = b; i < n; i++ ) {
								if ( x < k_uWidth && y < k_uHeight ) {
									b = m_Bank.Frames[uIndex].Data[p];
									m_Bank.Frames[uIndex].Map[x][y] = b;
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


			const auto k_uWidth = m_Bank.Frames[uIndex].Sprite.Width;
			const auto k_uHeight = m_Bank.Frames[uIndex].Sprite.Height;

			BMP.SetSize( std::max( uMin, k_uWidth ), std::max( uMin, k_uHeight ) );
			BMP.SetBitDepth( 24 );

			Map( uIndex );
			for ( uint16_t x = 0; x < k_uWidth; x++ ) {
				for ( uint16_t y = 0; y < k_uHeight; y++ ) {
					uColorIndex = m_Bank.Frames[uIndex].Map[x][y];

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
			assert( m_pTextures.size() == 0 && "Sprite textures were already created" );

			const auto pColorTable = m_pPalette->GetColorTable();
			CTexture2D* pTexture = nullptr;
			BYTE* pTexels = nullptr;
			D3DLOCKED_RECT rc;

			if ( m_pTextures.size() > 0 ) {
				g_ErrHandler.Log( "CSprite: Textures already created." );
				return;
			}

			m_pTextures.resize( m_Bank.Header.Count );
			for ( uint32_t i = 0; i < m_Bank.Header.Count; i++ ) {

				const auto k_uWidth = m_Bank.Frames[i].Sprite.Width;
				const auto k_uHeight = m_Bank.Frames[i].Sprite.Height;

				if ( !IsValid( m_Bank.Frames[i].Sprite ) ) {
					// TODO: log?
					continue;
				}

				Map( i );
				m_pTextures[i] = new CTexture2D( pD3DDevice, k_uWidth, k_uHeight );
				pTexture = m_pTextures[i];

				pTexture->GetTexture()->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
				pTexels = static_cast<BYTE*>(rc.pBits);

				for ( uint16_t y = 0; y < k_uHeight; y++ ) {
					for ( uint16_t x = 0; x < k_uWidth; x++ ) {
						auto index = m_Bank.Frames[i].Map[x][y];
						Color clr = pColorTable[index];
						WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
					}
				}

				pTexture->GetTexture()->UnlockRect( 0 );
			}
		}

		void CSprite::Reset()
		{
			m_nBufferLength = 0;
			delete[] m_pBuffer;
			for ( size_t i = 0; i < m_pTextures.size(); i++ ) {
				m_pTextures[i]->Clear();
			}
			m_Bank.Frames.clear();
			m_bIsHFX = false;
		}
	}
}

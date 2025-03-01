/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <vector>

#include "AssetTypes.h"

class CTexture2D;
class CPalette;

typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;
extern inline void SafeDestroyTexture( CTexture2D*& pTexture );

namespace Assets
{
	namespace Sprite
	{
		using PixelMap = std::vector<std::vector<uint8_t>>; // Must be uint8_t

		struct SpriteInfo
		{
			uint16_t	Width;
			uint16_t	Height;
			uint32_t	Offset;
		};

		struct Frame
		{
			SpriteInfo	Sprite;
			int8_t* Data;
			PixelMap	Map;
		};

		struct BankHeader
		{
			char		Magic[4];
			uint32_t	Count;
		};

		struct Bank
		{
			BankHeader Header;
			std::vector<Frame> Frames;
		};
	}

	class CSprite
	{
	public:
		explicit CSprite() = default;
		~CSprite() = default;

		Sprite::Bank m_Bank;
		Result LoadBin( const std::string& file );
		void ExportImg( uint16_t index );
		void Map( uint16_t index );
		void Reset();

		void SetPalette( CPalette* pPalette ) { m_pPalette = pPalette; }
		void SetHFX( bool b ) { m_bIsHFX = b; }

		void CreateTextures( LPDIRECT3DDEVICE9 pD3DDevice );
		[[nodiscard]] inline CTexture2D* GetTexture( uint32_t uSlot ) { return (uSlot < m_pTextures.size()) ? m_pTextures.at( uSlot ) : nullptr; }

	private:
		[[nodiscard]] inline bool IsAlphaSprite( uint32_t uIndex ) const { return m_bIsHFX && ((uIndex >= 1090 && uIndex <= 1499) || (uIndex >= 1538 && uIndex <= 1592)); }
		[[nodiscard]] inline bool IsValid( Sprite::SpriteInfo& sprInfo ) { return sprInfo.Width > 0 && sprInfo.Height > 0; }

	private:
		char* m_pBuffer = nullptr;
		uint32_t m_nBufferLength = 0;
		bool m_bIsHFX = false;;

		CPalette* m_pPalette;
		std::vector<CTexture2D*> m_pTextures;

	};

}


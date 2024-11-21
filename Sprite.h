#pragma once
#define NOMINMAX
#include <vector>

#include "Texture.h"
#include "Palette.h"
#include "EasyBMP/EasyBMP.h"

namespace Assets
{
	namespace Sprite
	{
		// Has to be uint8_t
		typedef std::vector<std::vector<uint8_t>> PixelMap;

		struct SpriteInfo
		{
			uint16_t	Width;
			uint16_t	Height;
			uint32_t	Offset;
		};

		struct Frame
		{
			SpriteInfo	Sprite;
			int8_t*		Data;
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

		class CSprite
		{
		public:
			~CSprite() = default;
			CSprite( CPalette* pPalette ) :
				m_pPalette(pPalette), m_IsHFX(false), m_pBuffer(nullptr), m_nBufferLength(0)
			{}

			Bank Bank;
			Result Load( const std::string& file );
			void Map( uint16_t index );
			void Export( uint16_t index );
			void CreateTextures( LPDIRECT3DDEVICE9 pD3DDevice );
			void Clear();

			void SetPalette( CPalette* pPalette )
			{
				m_pPalette = pPalette;
			}

			void SetHFX( bool b )
			{
				m_IsHFX = b;
			}

			// TODO: use flags?
			[[nodiscard]] bool IsValid( SpriteInfo& spr )
			{
				return (spr.Height == 0 || spr.Width == 0) ? false : true;
			}

			[[nodiscard]] CTexture2D* GetTexture( uint32_t uSlot )
			{
				return m_Textures.at(uSlot);
			}

		private:
			bool IsAlphaSprite( uint32_t index );

		private:
			char* m_pBuffer;
			uint32_t m_nBufferLength;
			
			CPalette* m_pPalette;
			std::vector<CTexture2D*> m_Textures;
			
			bool m_IsHFX;
		};
	}
}

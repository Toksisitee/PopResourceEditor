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
			explicit CSprite( CPalette* pPalette ) :
				m_pPalette( pPalette ), m_bIsHFX( false )
			{
			}
			~CSprite() = default;

			Bank m_Bank;
			Result LoadBin( const std::string& file );
			void ExportImg( uint16_t index );
			void Map( uint16_t index );
			void Reset();

			void SetPalette( CPalette* pPalette ) { m_pPalette = pPalette; }
			void SetHFX( bool b ) { m_bIsHFX = b; }

			void CreateTextures( LPDIRECT3DDEVICE9 pD3DDevice );
			[[nodiscard]] CTexture2D* GetTexture( uint32_t uSlot );

		private:
			[[nodiscard]] bool IsAlphaSprite( uint32_t uIndex ) const;
			[[nodiscard]] bool IsValid( SpriteInfo& sprInfo );

		private:
			char* m_pBuffer = nullptr;
			uint32_t m_nBufferLength = 0;
			bool m_bIsHFX = false;;

			CPalette* m_pPalette;
			std::vector<CTexture2D*> m_pTextures;

		};
	}
}

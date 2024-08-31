#pragma once
#include <vector>

#include "Palette.h"
#include "EasyBMP/EasyBMP.h"

#define RPAL	(0)
#define RREAD	(1)
#define RSKIP	(2)
#define REND	(3)

namespace Assets
{
	namespace Sprite
	{
		class CSprite
		{
		public:
			CSprite( CPalette* pPalette ) :
				m_pPalette(pPalette)
			{
				m_IsHFX = false;
				m_pBuffer = nullptr;
				m_nBufferLength = 0;
			};

			~CSprite() {};

			typedef std::vector<std::vector<int32_t>> MAP;

			struct RAW
			{
				uint8_t	Data;
				int8_t  Type;
			};

			struct TbSprite
			{
				uint16_t	Width;
				uint16_t	Height;
				uint32_t	Offset;
			};

			struct DATA
			{
				TbSprite			Sprite;
				int8_t* Data;
				std::vector<RAW>	Raw;
				MAP					Map;
			};

			struct BankHeader
			{
				char		Magic[4];
				uint32_t	Frames;
			};

			struct Bank
			{
				BankHeader Header;
				std::vector<DATA> Data;
			};

			Bank SprBank;
			Result LoadBank( const std::string& file );
			void MapSprite( uint16_t index );
			void ExportSprite( uint16_t index );
			void ExportSprites();
			void Clear() { delete[] m_pBuffer; SprBank.Data.clear(); m_IsHFX = false; }
			void SetHFX( bool b ) { m_IsHFX = b; };

		private:
			CPalette* m_pPalette;
			bool m_IsHFX ;
			char* m_pBuffer;
			uint32_t m_nBufferLength;
			bool IsPixelColorKey( const RGBApixel& rgb );
			bool IsPixelEmpty( const RGBApixel& rgb );
			bool IsAlphaSprite( uint32_t index );
		};
	}
}

#pragma once
#include "Texture.h"

namespace Assets
{
	namespace Sky
	{
		constexpr uint32_t	k_uWidth = 512;
		constexpr uint32_t	k_uHeight = 512;
		constexpr uint32_t	k_uMaxColors = 16;
		constexpr uint32_t	k_uColorStart = 112;

		class CSky
		{
		public:
			Result	Load( std::string& sFilePath );
			Result	Generate( std::string& fFilepath, CPalette* pPalette );
			Result	Export( std::string& fFilepath, RGB* pColorTable );
			bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice, CPalette* pPalette );
			[[nodiscard]] inline CTexture2D* GetTexture()
			{
				return m_pTexture;
			}
		private:
			uint8_t m_Data[k_uWidth * k_uHeight];
			CTexture2D* m_pTexture;
		};
	}
}
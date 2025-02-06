#pragma once
#include "Texture.h"
#include "Assets.h"
#include "Palette.h"

namespace Assets
{
	namespace Sky
	{
		constexpr uint32_t	k_uWidth = 512;
		constexpr uint32_t	k_uHeight = 512;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
		constexpr uint32_t	k_uNumColors = 16;		// Max numbers of colors in the palette reserved for the sky texture
		constexpr uint32_t	k_uColorStart = 112;	// Starting index in the palette for the sky texture
	}

	class CSky
	{
	public:
		Result	LoadBin( const std::string& sFilePath );
		Result	ExportBin( const std::string& fFilepath );
		Result	ExportImg( const std::string& fFilepath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );
		uint8_t FindColor( const Color& color );

		inline void DestroyTexture()
		{
			SafeDestroyTexture( m_pTexture );
		}
		[[nodiscard]] inline CTexture2D* GetTexture()
		{
			return m_pTexture;
		}
		[[nodiscard]] inline CPalette* GetPalette()
		{
			return &m_Palette;
		}
	private:
		uint8_t m_Data[Sky::k_uWidth * Sky::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
		uint8_t	m_uFlags;
	};
}
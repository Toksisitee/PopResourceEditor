#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace BigFade
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 1152;
		constexpr uint32_t	k_uNumColors = 112;		// Max numbers of colors in the palette reserved for the BigFade texture
	}

	class CBigFade
	{
	public:
		~CBigFade() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( std::string& sFilePath );
		Result	Generate( std::string& sFilePath );
		Result	Export( std::string& sFilePath );
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
		uint8_t m_Data[BigFade::k_uWidth * BigFade::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
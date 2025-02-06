#pragma once
#include "Texture.h"
#include "Palette.h"
#include "Assets.h"

namespace Assets
{
	namespace BigFade
	{
		constexpr uint32_t	k_uWidth = 256;			// U
		constexpr uint32_t	k_uHeight = 1152;		// V
		constexpr uint32_t	k_uSize = k_uHeight * k_uWidth;
		constexpr uint32_t	k_uNumColors = 112;		// Max numbers of colors in the palette reserved for the BigFade texture
	}

	class CBigFade
	{
	public:
		~CBigFade() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( const std::string& sFilePath );
		Result	LoadImg( const std::string& sFilePath );
		Result	Generate( const std::string& sFilePath );
		Result	Export( const std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );
		uint8_t FindColor( const Color& color );
		uint8_t GetColor( const uint32_t uIndex );

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
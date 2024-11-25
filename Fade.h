#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Fade
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 64;
	}

	class CFade
	{
	public:
		~CFade() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( std::string& sFilePath );
		Result	Generate( std::string& sFilePath );
		Result	Export( std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );

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

	protected:
		void FadeDark( uint8_t* pPalette, uint8_t* pData, uint32_t uLights, uint32_t uDarkness );
		void FadeLight( uint8_t* pPalette, uint8_t* pData, uint32_t uLights, uint32_t uLightness );
		void FadeColor( uint8_t& r, uint8_t& g, uint8_t& b, float fFadeFactor );
		uint8_t GetLuminance( uint8_t r, uint8_t g, uint8_t b );
		void GenerateTable();
	private:
		uint8_t m_Data[Fade::k_uWidth * Fade::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
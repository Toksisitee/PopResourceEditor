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
		constexpr uint32_t	k_uSize = k_uWidth*k_uHeight;
	}

	class CFade
	{
	public:
		~CFade() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( const std::string& sFilePath );
		Result	Generate( const std::string& sFilePath );
		Result	Export( const std::string& sFilePath );
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
		void FadeColor( Color& color, float fFadeFactor );
		void ComputeTable();
	private:
		uint8_t m_Data[Fade::k_uWidth * Fade::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
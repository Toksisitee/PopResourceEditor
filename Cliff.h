#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Cliff
	{
		constexpr uint32_t	k_uWidth = 128;
		constexpr uint32_t	k_uHeight = 64;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
	}

	class CCliff
	{
	public:
		~CCliff() { SafeDestroyTexture( m_pTexture ); }

		Result	LoadBin( const std::string& sFilePath );
		Result	Generate( uint8_t uMode );
		Result	ExportImg( const std::string& sFilePath );
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
		float m_fLuminance = 0.6f;
	protected:
		void ComputeTable( uint8_t uMode );
		Color BlendColors( const Color& color1, const Color& color2, const float fFadeFactor );
		Color IncreaseLuminance( const Color& color, const float fLuminanceFactor );
	private:
		uint8_t m_Data[Cliff::k_uWidth * Cliff::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
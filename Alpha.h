#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Alpha
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 256;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
	}

	class CAlpha
	{
	public:
		~CAlpha() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( const std::string& sFilePath );
		Result	Generate( const std::string& sFilePath );
		Result	Export( const std::string& sFilePath );
		Color	GetAlphaColor( uint8_t uIndex );
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
		uint32_t ComputeTable( uint8_t* pData, const Color& targetColor, uint32_t uRow );

	private:
		uint8_t m_Data[Alpha::k_uWidth * Alpha::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
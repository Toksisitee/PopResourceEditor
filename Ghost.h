#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Ghost
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 256;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
		constexpr uint8_t	k_uDefaultOpacity = 66; // Varies for every texture?
	}

	class CGhost
	{
	public:
		CGhost() : m_uOpacity( Ghost::k_uDefaultOpacity ) {}
		~CGhost() { SafeDestroyTexture( m_pTexture ); }

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
	public:
		uint8_t m_uOpacity;
	protected:
		void ComputeTable( uint8_t uOpacity );
	private:
		uint8_t m_Data[Ghost::k_uWidth * Ghost::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
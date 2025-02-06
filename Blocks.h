#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Blocks
	{
		constexpr uint32_t	k_uBlockWidth = 32;
		constexpr uint32_t	k_uBlockHeight = 32;
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 1024;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
		constexpr size_t	k_uNumBlocks = (k_uWidth / k_uBlockWidth) * (k_uHeight / k_uBlockHeight);
	}

	class CBlocks
	{
	public:
		~CBlocks() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( const std::string& sFilePath );
		Result	Generate( std::string& sFilePath );
		Result	Export( std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );
		bool	CreateSubTexture( LPDIRECT3DDEVICE9 pD3DDevice, size_t index );

		inline void DestroyTexture( size_t uIndex )
		{
			SafeDestroyTexture( m_pSubTextures[uIndex] );
		}
		inline void DestroyTexture()
		{
			SafeDestroyTexture( m_pTexture );
		}
		inline void DestroyTextures()
		{
			for ( size_t i = 0; i < Blocks::k_uNumBlocks; i++ ) {
				DestroyTexture( i );
			}
			DestroyTexture();
		}
		[[nodiscard]] inline CTexture2D* GetTexture( size_t uIndex )
		{
			return m_pSubTextures[uIndex];
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
		uint8_t m_Data[Blocks::k_uWidth * Blocks::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;	// Atlas
		CTexture2D* m_pSubTextures[256]; // Split textures (32x32)
	};
}
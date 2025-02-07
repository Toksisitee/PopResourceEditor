#pragma once
#include "Texture.h"
#include "Palette.h"
#include "Assets.h"

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

	class CBlocks : public CAsset
	{
	public:
		using CAsset::GetTexture;
		using CAsset::DestroyTexture;

		~CBlocks() { SafeDestroyTexture( m_pTexture ); }
		
		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportBin( const std::string& sFilePath ) override;
		Result	ExportImg( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		bool	CreateSubTexture( LPDIRECT3DDEVICE9 pD3DDevice, size_t index );
		inline void DestroyTexture( size_t uIndex )
		{
			SafeDestroyTexture( m_pSubTextures[uIndex] );
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

	private:
		uint8_t m_Data[Blocks::k_uWidth * Blocks::k_uHeight];
		CTexture2D* m_pSubTextures[256]; // Split textures (32x32)
	};
}
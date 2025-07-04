/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
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

		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override;
		Result	ExportBin( const std::string& sFilePath ) override;
		Result	ExportImg( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice, int nWidth, int nHeight );
		bool	CreateSubTexture( LPDIRECT3DDEVICE9 pD3DDevice, size_t index );
		Result	LoadSubImg( LPDIRECT3DDEVICE9 pD3DDevice, const std::string& sFilePath, size_t uIndex );
		Result	ExportSubImg( const std::string& sFilePath, size_t uIndex );

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
		uint8_t m_Data[Blocks::k_uSize];
		CTexture2D* m_pSubTextures[Blocks::k_uNumBlocks]; // Split textures (32x32)
	};
}

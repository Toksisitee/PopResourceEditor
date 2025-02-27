/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include "Assets.h"

namespace Assets
{
	namespace Sky
	{
		constexpr uint32_t	k_uWidth = 512;
		constexpr uint32_t	k_uHeight = 512;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
		constexpr uint32_t	k_uNumColors = 16;		// Max numbers of colors in the palette reserved for the sky texture
		constexpr uint32_t	k_uColorStart = 112;	// Starting index in the palette for the sky texture
	}

	class CSky : public CAsset
	{
	public:
		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath );
		Result	LoadImg( const std::string& sFilePath );
		Result	ExportImg( const std::string& sFilePath );
		Result	ExportBin( const std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		uint8_t FindColor( const Color& color );

	private:
		uint8_t m_Data[Sky::k_uWidth * Sky::k_uHeight];
	};
}

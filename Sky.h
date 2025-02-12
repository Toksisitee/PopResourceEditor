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
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportImg( const std::string& fFilepath );
		Result	ExportBin( const std::string& fFilepath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		uint8_t FindColor( const Color& color );

	private:
		uint8_t m_Data[Sky::k_uWidth * Sky::k_uHeight];
	};
}
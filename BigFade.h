#pragma once
#include "Texture.h"
#include "Palette.h"
#include "Assets.h"

namespace Assets
{
	namespace BigFade
	{
		constexpr uint32_t	k_uWidth = 256;			// U
		constexpr uint32_t	k_uHeight = 1152;		// V
		constexpr uint32_t	k_uSize = k_uHeight * k_uWidth;
		constexpr uint32_t	k_uNumColors = 112;		// Max numbers of colors in the palette reserved for the BigFade texture
	}

	class CBigFade : public CAsset
	{
	public:
		~CBigFade() { SafeDestroyTexture( m_pTexture ); }

		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override;
		Result	ExportImg( const std::string& sFilePath ) override;
		Result	ExportBin( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		uint8_t FindColor( const Color& color );
		uint8_t GetColor( const uint32_t uIndex );

	private:
		uint8_t m_Data[BigFade::k_uWidth * BigFade::k_uHeight];
	};
}
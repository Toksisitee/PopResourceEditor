#pragma once
#include "Texture.h"
#include "Assets.h"
#include "Palette.h"

namespace Assets
{
	namespace Alpha
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 256;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
	}

	class CAlpha : public CAsset
	{
	public:
		~CAlpha() { SafeDestroyTexture( m_pTexture ); }

		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportImg( const std::string& sFilePath ) override;
		Result	ExportBin( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		Result	Generate();
		Color	GetAlphaColor( uint8_t uIndex );

	protected:
		uint32_t ComputeTable( uint8_t* pData, const Color& targetColor, uint32_t uRow );

	private:
		uint8_t m_Data[Alpha::k_uWidth * Alpha::k_uHeight];
	};
}
#pragma once
#include "Texture.h"
#include "Assets.h"

namespace Assets
{
	namespace Disp
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 256;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
	}

	class CDisp : public CAsset
	{
	public:
		~CDisp() { SafeDestroyTexture( m_pTexture ); }

		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportImg( const std::string& sFilePath ) override;
		Result  ExportBin( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		Result	Generate( const std::string& sFilePath );

	private:
		const uint8_t k_uGrayscaleOffset = 128;
		uint8_t m_Data[Disp::k_uWidth * Disp::k_uHeight];
	};
}
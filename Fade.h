#pragma once
#include "Assets.h"

namespace Assets
{
	namespace Fade
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 64;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
	}

	class CFade : public CAsset
	{
	public:
		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override;
		Result	ExportImg( const std::string& sFilePath ) override;
		Result  ExportBin( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		Result	Generate();

	protected:
		void FadeColor( Color& color, float fFadeFactor );
		void ComputeTable();
	private:
		uint8_t m_Data[Fade::k_uWidth * Fade::k_uHeight];
	};
}
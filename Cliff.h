#pragma once
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Cliff
	{
		constexpr uint32_t	k_uWidth = 128;
		constexpr uint32_t	k_uHeight = 64;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
	}

	class CCliff : public CAsset
	{
	public:
		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportImg( const std::string& sFilePath ) override;
		Result  ExportBin( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		Result	Generate( uint8_t uMode );

	public:
		// TODO: Does it make sense for this to be public?
		float m_fLuminance = 0.6f;
	protected:
		void ComputeTable( uint8_t uMode );
		Color BlendColors( const Color& color1, const Color& color2, const float fFadeFactor );
		Color IncreaseLuminance( const Color& color, const float fLuminanceFactor );
	private:
		uint8_t m_Data[Cliff::k_uWidth * Cliff::k_uHeight];
	};
}
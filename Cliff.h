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

		enum class Generation : uint8_t
		{
			NO_LUMINANCE = 0,
			LUMINANCE_1,
			LUMINANCE_2,
			MAX,
		};
	}

	class CCliff : public CAsset
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

		Result	Generate( Cliff::Generation eMode );

	public:
		// TODO: Does it make sense for this to be public?
		float m_fLuminance = 0.6f;
	protected:
		Result ComputeTable( Cliff::Generation eMode );
		Color BlendColors( const Color& color1, const Color& color2, const float fFadeFactor );
		Color IncreaseLuminance( const Color& color, const float fLuminanceFactor );
	private:
		uint8_t m_Data[Cliff::k_uWidth * Cliff::k_uHeight];
	};
}
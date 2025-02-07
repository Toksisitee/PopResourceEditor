#pragma once
#include "Texture.h"
#include "Palette.h"
#include "Assets.h"

namespace Assets
{
	namespace Ghost
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 256;
		constexpr uint32_t	k_uSize = k_uWidth * k_uHeight;
		constexpr uint8_t	k_uDefaultOpacity = 66; // Varies for every texture?
	}

	class CGhost : public CAsset
	{
	public:
		CGhost() : m_uOpacity( Ghost::k_uDefaultOpacity ) {}
		~CGhost() { SafeDestroyTexture( m_pTexture ); }

		// ====== Virtual Overrides  ======
		Result	LoadBin( const std::string& sFilePath ) override;
		Result	LoadImg( const std::string& sFilePath ) override { return Result::FAIL_LOAD; }
		Result	ExportImg( const std::string& sFilePath ) override;
		Result	ExportBin( const std::string& sFilePath ) override;
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice ) override;
		inline void* GetPtr() override { return static_cast<void*>(&m_Data); }
		// ================================

		Result	Generate( const std::string& sFilePath );

	public:
		uint8_t m_uOpacity;
	protected:
		void ComputeTable( uint8_t uOpacity );
	private:
		uint8_t m_Data[Ghost::k_uWidth * Ghost::k_uHeight];
	};
}
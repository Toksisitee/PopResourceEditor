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

	class CDisp
	{
	public:
		~CDisp() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( const std::string& sFilePath );
		Result	Generate( const std::string& sFilePath );
		Result	Export( const std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );

		inline void DestroyTexture()
		{
			SafeDestroyTexture( m_pTexture );
		}
		[[nodiscard]] inline CTexture2D* GetTexture()
		{
			return m_pTexture;
		}
		[[nodiscard]] inline uint8_t* GetPtr()
		{
			return &m_Data[0];
		}

	private:
		const uint8_t k_uGrayscaleOffset = 128;
		uint8_t m_Data[Disp::k_uWidth * Disp::k_uHeight];
		CTexture2D* m_pTexture;
	};
}
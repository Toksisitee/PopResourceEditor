#pragma once
#include "Texture.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Disp
	{
		constexpr uint32_t	k_uWidth = 256;
		constexpr uint32_t	k_uHeight = 256;
	}

	class CDisp
	{
	public:
		~CDisp() { SafeDestroyTexture( m_pTexture ); }

		Result	Load( std::string& sFilePath );
		Result	Generate( std::string& sFilePath );
		Result	Export( std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );

		inline void DestroyTexture()
		{
			SafeDestroyTexture( m_pTexture );
		}
		[[nodiscard]] inline CTexture2D* GetTexture()
		{
			return m_pTexture;
		}

	private:
		const uint8_t k_uGrayscaleOffset = 128;
		uint8_t m_Data[Disp::k_uWidth * Disp::k_uHeight];
		CTexture2D* m_pTexture;
	};
}
#pragma once
#include "Texture.h"
#include "Palette.h"
#include "AssetsErrHandler.h"

namespace Assets
{
	namespace Cliff
	{
		constexpr uint32_t	k_uWidth = 128;
		constexpr uint32_t	k_uHeight = 64;
	}

	class CCliff
	{
	public:
		~CCliff() { SafeDestroyTexture( m_pTexture ); }

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
		[[nodiscard]] inline CPalette* GetPalette()
		{
			return &m_Palette;
		}
	protected:
		void ComputeTable();

	private:
		uint8_t m_Data[Cliff::k_uWidth * Cliff::k_uHeight];
		CPalette m_Palette;
		CTexture2D* m_pTexture;
	};
}
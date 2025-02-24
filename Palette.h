#pragma once
#include <string>

#include "AssetTypes.h"
#include "Editor.h"

class CTexture2D;
typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;
extern inline void SafeDestroyTexture( CTexture2D*& pTexture );

namespace Assets
{
	namespace Palette
	{
		constexpr size_t	k_uNumColors = 256;
		constexpr size_t	k_uColorsPerRow = 16;
		constexpr size_t	k_uCellScale = 8;
		constexpr size_t	k_uWidth = k_uCellScale * k_uColorsPerRow;
		constexpr size_t	k_uHeight = k_uCellScale * (k_uNumColors / k_uColorsPerRow);
		constexpr uint8_t	k_uNumColorKeys = 2;
		constexpr size_t	k_uSize = k_uNumColors * 4;
	}

	class CPalette
	{
	public:
		Result	LoadBin( const std::string& sFilePath );
		Result	ExportBin( const std::string& sFilePath );
		Result	ExportImg( const std::string& sFilePath );
		bool	CreateTexture( LPDIRECT3DDEVICE9 pD3DDevice );
		[[nodiscard]] uint8_t* GetPtr();

		inline void DestroyTexture()
		{
			SafeDestroyTexture( m_pTexture );
		}
		[[nodiscard]] inline CTexture2D* GetTexture()
		{
			return m_pTexture;
		}
		[[nodiscard]] inline bool HasChanged()
		{
			return m_bModified;
		}
		inline void SetChanged( bool bState )
		{
			m_bModified = bState;
		}

		[[nodiscard]] Color* GetColor( uint8_t uIndex ) { return &m_ColorTable[uIndex]; }
		[[nodiscard]] Color* GetColorTable();
		[[nodiscard]] uint8_t FindColor( const Color& clr, size_t uMin = 0, size_t uMax = Palette::k_uNumColors );
		[[nodiscard]] uint8_t FindExactColor( const Color& clr, bool bFallback = false );
		[[nodiscard]] uint8_t GetColorKey( size_t uSlot = 0 );
		[[nodiscard]] bool IndexIsColorKey( size_t uIndex );
	private:
		uint8_t m_uColorKeys[Palette::k_uNumColorKeys] = { 255, 255 };;
		Color m_ColorTable[Palette::k_uNumColors];
		uint8_t m_Data[Palette::k_uSize];
		CTexture2D* m_pTexture;
		bool m_bModified = false;
	};
};

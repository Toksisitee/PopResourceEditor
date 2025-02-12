#pragma once
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
		constexpr uint32_t	k_uWidth = 256; // Texture Width
		constexpr uint32_t	k_uHeight = 256; // Texture Height
		constexpr uint8_t	k_uNumColorKeys = 2;
		constexpr size_t	k_uSize = k_uNumColors * 4;
	}

	// TODO: Use CAsset as base?
	class CPalette
	{
	public:
		Result	LoadBin( const std::string& sFilePath );
		Result	ExportImg( const char* pFilePath );
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

		[[nodiscard]] Color* GetColor( uint8_t uIndex ) { return &m_ColorTable[uIndex]; }
		[[nodiscard]] Color* GetColorTable();
		//[[nodiscard]] uint8_t FindClosestColor( const Color& clr, bool bFullSearch = false );
		[[nodiscard]] uint8_t FindColor( const Color& clr, size_t uMin = 0, size_t uMax = Palette::k_uNumColors );
		//uint8_t FindColor( const RGB& clr, bool bClosest = false );
		[[nodiscard]] uint8_t FindExactColor( const Color& clr, bool bFallback = false );
		[[nodiscard]] uint8_t GetColorKey( size_t uSlot = 0 );
		[[nodiscard]] bool IndexIsColorKey( size_t uIndex );
	private:
		uint8_t m_uColorKeys[Palette::k_uNumColorKeys] = { 255, 255 };;
		Color m_ColorTable[Palette::k_uNumColors];
		uint8_t m_Data[Palette::k_uSize];
		CTexture2D* m_pTexture;
	};
};
//extern uint8_t FindColor( uint8_t* palette, int32_t r, int32_t g, int32_t b, uint16_t min = 0, uint16_t max = 255 );
#pragma once
#include "AssetsErrHandler.h"
#include "Editor.h"

namespace Assets
{
	namespace Palette
	{
		//namespace Palette TODO
		constexpr uint8_t k_uNumColorKeys = 2;
		constexpr size_t k_uNumColors = 256;
		constexpr size_t k_uSize = k_uNumColors;
	}

	class CPalette
	{
	public:
		Result Export( const char* pFilePath );
		Result Load( const std::string& sFilePath );
		[[nodiscard]] Color* GetColor( uint8_t uIndex ) { return &m_ColorTable[uIndex]; }
		[[nodiscard]] uint8_t* GetPtr();
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
	};
};
//extern uint8_t FindColor( uint8_t* palette, int32_t r, int32_t g, int32_t b, uint16_t min = 0, uint16_t max = 255 );
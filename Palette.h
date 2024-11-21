#pragma once
#include "AssetsErrHandler.h"
namespace Assets
{
	//namespace Palette TODO
	constexpr uint8_t k_uNumColorKeys = 2;
	constexpr size_t k_uNumColors = 256;

	class CPalette
	{
	public:
		Result Export( const char* pFilePath );
		Result Load( std::string& file );
		[[nodiscard]] uint8_t FindClosestColor( const RGB& clr, bool bFullSearch = false );
		[[nodiscard]] uint8_t FindColor( uint8_t* pPalette, const RGB& clr, size_t uMin, size_t uMax );
		//uint8_t FindColor( const RGB& clr, bool bClosest = false );
		[[nodiscard]] uint8_t FindColorAll( const RGB& clr, bool bClosest = false );
		[[nodiscard]] uint8_t FindBigFadeColor( const RGB& clr );
		[[nodiscard]] uint8_t FindSkyColor( const RGB& clr );
		[[nodiscard]] RGB* GetPalette(); // TODO: better name? accept index?
		[[nodiscard]] uint8_t* GetPtr();
		[[nodiscard]] uint8_t GetColorKey( size_t uSlot = 0 );
		[[nodiscard]] bool IndexIsColorKey( size_t uIndex );
	private:
		uint8_t m_uColorKeys[k_uNumColorKeys] = { 255, 255 };;
		RGB m_ColorTable[k_uNumColors];
	};
};
//extern uint8_t FindColor( uint8_t* palette, int32_t r, int32_t g, int32_t b, uint16_t min = 0, uint16_t max = 255 );
#pragma once
namespace Assets
{
	class CPalette
	{
	public:
		ErrorCode Export( const char* pFilePath );
		ErrorCode Load( std::string& file );
		uint8_t FindClosestColor( const RGB& clr, bool bFullSearch = false );
		uint8_t FindColor( uint8_t* pPalette, const RGB& clr, size_t uMin, size_t uMax );
		//uint8_t FindColor( const RGB& clr, bool bClosest = false );
		uint8_t FindColorAll( const RGB& clr, bool bClosest = false );
		uint8_t FindBigFadeColor( const RGB& clr );
		uint8_t FindSkyColor( const RGB& clr );
		RGB* GetPalette();
		uint8_t* GetData();
	private:
		bool IndexIsColorKey( size_t uIndex );
	private:
		uint8_t m_uColorKeys[2] = { 255, 255 };;
		const size_t m_kNumColors = 256;
		RGB m_ColorTable[256];
	};
};
//extern uint8_t FindColor( uint8_t* palette, int32_t r, int32_t g, int32_t b, uint16_t min = 0, uint16_t max = 255 );
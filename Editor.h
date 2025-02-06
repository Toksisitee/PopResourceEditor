#pragma once
#include <cstdint>

#define TO_STRING(s)	#s
#define STRINGIFY(s)	TO_STRING(s)

#define VERSION_MAJOR	0
#define VERSION_MINOR	1
#define VERSION_PATCH	0

#define EDITOR_NAME		L"PopResourceEditor"
#define EDITOR_VERSION	STRINGIFY(VERSION_MAJOR) "." STRINGIFY(VERSION_MINOR) "." STRINGIFY(VERSION_PATCH)
#define	EDITOR_DATE		__DATE__ " " __TIME__

#define EDITOR_DEBUG_FONTS	(0)
#define EDITOR_PI (3.14159265358979323846)


#define SAFE_FREE(pBuffer) if (pBuffer) \
	free(pBuffer); \

typedef unsigned char BYTE;

#pragma pack(push, 1)
struct Color {
	Color() = default;
	Color( uint8_t r, uint8_t g, uint8_t b ) : R( r ), G( g ), B( b ) {}
	
	uint8_t R, G, B;

	bool operator==( const Color& o ) const
	{
		return R == o.R && G == o.G && B == o.B;
	}
	bool operator!=( const Color& o ) const
	{
		return !(*this == o);
	}
	bool operator<( const Color& o ) const
	{
		if ( R != o.R ) return R < o.R;
		if ( G != o.G ) return G < o.G;
		return B < o.B;
	}
};
#pragma pack(pop)

struct fVec2 {
	fVec2() = default;
	fVec2( float _x, float _y ) : x( _x ), y( _y ) {}
	float x;
	float y;
};

enum eImFont : uint8_t
{
	Small = 0,
	Medium,
	Large,
	Max
};

inline void WriteRGBTexel( BYTE* pTexels, size_t x, size_t y, size_t pitch, const Color& rgb )
{
	size_t i = (y * pitch) + (x * 4);
	pTexels[i] = rgb.B;
	pTexels[i + 1] = rgb.G;
	pTexels[i + 2] = rgb.R;
	pTexels[i + 3] = 255;
};

inline void WriteRGBTexel( BYTE* pTexels, size_t x, size_t y, size_t pitch, Color* rgb )
{
	size_t i = (y * pitch) + (x * 4);
	pTexels[i] = rgb->B;
	pTexels[i + 1] = rgb->G;
	pTexels[i + 2] = rgb->R;
	pTexels[i + 3] = 255;
};
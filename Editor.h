/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <cstdint>

#define TO_STRING(s)	#s
#define STRINGIFY(s)	TO_STRING(s)

#if _DEBUG
#define EDITOR_CONFIG "Debug"
#else
#define EDITOR_CONFIG "Release"
#endif

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
	Color( uint8_t r, uint8_t g, uint8_t b ) : r( r ), g( g ), b( b ) {}
	
	uint8_t r, g, b;

	float GetLuminance() const
	{
		return 0.299f * r + 0.587f * g + 0.114f * b;
	}

	bool operator==( const Color& o ) const
	{
		return r == o.r && g == o.g && b == o.b;
	}
	bool operator!=( const Color& o ) const
	{
		return !(*this == o);
	}
	bool operator<( const Color& o ) const
	{
		if ( r != o.r ) return r < o.r;
		if ( g != o.g ) return g < o.g;
		return b < o.b;
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
	pTexels[i] = rgb.b;
	pTexels[i + 1] = rgb.g;
	pTexels[i + 2] = rgb.r;
	pTexels[i + 3] = 255;
};

inline void WriteRGBTexel( BYTE* pTexels, size_t x, size_t y, size_t pitch, Color* rgb )
{
	size_t i = (y * pitch) + (x * 4);
	pTexels[i] = rgb->b;
	pTexels[i + 1] = rgb->g;
	pTexels[i + 2] = rgb->r;
	pTexels[i + 3] = 255;
};

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


#define SAFE_FREE(pBuffer) if (pBuffer) \
	free(pBuffer); \

typedef unsigned char BYTE;

struct RGB
{
	uint8_t R, G, B;
};

enum eImFont : uint8_t
{
	Small = 0,
	Medium,
	Large,
	Max
};

inline void WriteRGBTexel( BYTE* pTexels, size_t x, size_t y, size_t pitch, const RGB& rgb )
{
	size_t i = (y * pitch) + (x * 4);
	pTexels[i] = rgb.B;
	pTexels[i + 1] = rgb.G;
	pTexels[i + 2] = rgb.R;
	pTexels[i + 3] = 255;
};
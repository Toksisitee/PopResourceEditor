/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#//include <D3dx9tex.h>
#include <string>
#include <map>

#include "Palette.h"

typedef struct IDirect3DTexture9* LPDIRECT3DTEXTURE9, * PDIRECT3DTEXTURE9;
typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;

class CTexture2D
{
public:
	CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, std::string sName, int nWidth, int nHeight );
	CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, std::string sName );
	CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight, Assets::CPalette* pPalette );
	CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight, uint8_t* pData, Assets::CPalette* pPalette );
	CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight );
	~CTexture2D();

	[[nodiscard]] LPDIRECT3DTEXTURE9 GetTexture() { return m_pD3DTexture; };
	[[nodiscard]] int GetHeight() const { return m_nHeight; };
	[[nodiscard]] int GetWidth() const { return m_nWidth; };
	void Clear();

private:
	LPDIRECT3DTEXTURE9 m_pD3DTexture;
	int m_nHeight, m_nWidth;
	LPDIRECT3DDEVICE9 m_pd3dDevice;
};

class CTextureManager
{
public:
	CTextureManager() = default;
	~CTextureManager();

	void AddTexture( const std::string& sTexture, CTexture2D* pTexture );
	CTexture2D* LoadTexture( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, const std::string& sName, int nWidth, int nHeight );
	CTexture2D* LoadTexture( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, const std::string& sName );
	CTexture2D* GetTexture2D( const std::string& sName );

	void DeleteAll();
	void ClearAll();

private:
	typedef std::map<std::string, CTexture2D*> Texture2DMap;
	Texture2DMap m_TextureMap;
};

CTexture2D* CopyTexture( LPDIRECT3DDEVICE9 pd3dDevice, CTexture2D* pSrcTex2D, const std::string& sName, int nDownscale = 0 );

extern CTextureManager g_TextureManager;

inline void SafeDestroyTexture( CTexture2D*& pTexture )
{
	if ( pTexture ) {
		delete pTexture;
		pTexture = nullptr;
	}
}

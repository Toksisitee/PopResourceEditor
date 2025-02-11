#pragma once
#include <D3dx9tex.h>
#include <string>
#include <map>

#include "Palette.h"

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

	CTexture2D* LoadTexture( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, const std::string& sName, int nWidth, int nHeight );
	CTexture2D* LoadTexture( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, const std::string& sName );
	CTexture2D*	CopyTexture( LPDIRECT3DDEVICE9 pd3dDevice, CTexture2D* pSrcTex2D, const std::string& sName );
	CTexture2D* GetTexture2D( const std::string& sName );
	void DeleteAll();
	void ClearAll();

private:
	typedef std::map<std::string, CTexture2D*> Texture2DMap;
	Texture2DMap m_TextureMap;
};

extern CTextureManager g_TextureManager;

inline void SafeDestroyTexture( CTexture2D*& pTexture )
{
	if ( pTexture ) {
		delete pTexture;
		pTexture = nullptr;
	}
}
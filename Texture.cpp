#pragma once
#include <string>
#include <map>
#include <memory>

#include <D3dx9tex.h>
#include <DxErr.h>

#include "spdlog\spdlog.h"

#include "Utils.h"
#include "Texture.h"

CTextureManager g_TextureManager;

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, std::string sTexName, int nWidth, int nHeight ) :
	m_pd3dDevice( pd3dDevice ), m_nWidth( nWidth ), m_nHeight( nHeight )
{
	HRESULT hr;
	std::string sPath = Util::FileSystem::GetAssetsDirectory() + sDirectory + "\\" + sTexName;
	std::wstring wStr = std::wstring( sPath.begin(), sPath.end() );
	if ( FAILED( (hr = D3DXCreateTextureFromFileEx( pd3dDevice, wStr.c_str(), nWidth, nHeight, D3DX_DEFAULT, 0,
		 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pD3DTexture )) ) ) {
		spdlog::error( "Failed to create Texture2D\nName: {}\nWidth: {}, Height: {}\nError: {} :: {}",
			sPath.c_str(), nWidth, nHeight, DXGetErrorStringA( hr ), DXGetErrorDescriptionA( hr ) );
		return;
	}
}

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, std::string sTexName ) :
	m_pd3dDevice( pd3dDevice ), m_nWidth( 0 ), m_nHeight( 0 )
{
	HRESULT hr;
	std::string sPath = Util::FileSystem::GetAssetsDirectory() + sDirectory + "\\" + sTexName;
	std::wstring wStr = std::wstring( sPath.begin(), sPath.end() );

	if ( FAILED( (hr = D3DXCreateTextureFromFile( pd3dDevice, wStr.c_str(), &m_pD3DTexture )) ) ) {
		spdlog::error( "Failed to create Texture2D\nName: {}\nError: {} :: {}",
			sPath.c_str(), DXGetErrorStringA( hr ), DXGetErrorDescriptionA( hr ) );
		return;
	}

	D3DSURFACE_DESC desc;
	m_pD3DTexture->GetLevelDesc( 0, &desc );
	m_nHeight = desc.Height;
	m_nWidth = desc.Width;
}

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight, Color* pPalette ) :
	m_pd3dDevice( pd3dDevice ), m_nWidth( nWidth ), m_nHeight( nHeight )
{
	pd3dDevice->CreateTexture( nWidth, nHeight, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pD3DTexture, NULL );

	D3DLOCKED_RECT rc;
	m_pD3DTexture->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );

	BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

	for ( size_t y = 0; y < nHeight; y++ ) {
		for ( size_t x = 0; x < nWidth; x++ ) {
			size_t uIndex = y * nWidth + x;

			Color clr = pPalette[uIndex];

			size_t iTexelIndex = (y * rc.Pitch) + (x * 4);
			pTexels[iTexelIndex] = clr.B;
			pTexels[iTexelIndex + 1] = clr.G;
			pTexels[iTexelIndex + 2] = clr.R;
			pTexels[iTexelIndex + 3] = 255;
		}
	}

	// Unlock the texture
	m_pD3DTexture->UnlockRect( 0 );
}

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight ) :
	m_pd3dDevice( pd3dDevice ), m_nWidth( nWidth ), m_nHeight( nHeight )
{
	pd3dDevice->CreateTexture( nWidth, nHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pD3DTexture, NULL );
}

void CTexture2D::Clear()
{
	if ( m_pD3DTexture ) {
		m_pD3DTexture->Release();
		m_pD3DTexture = nullptr;
	}
}

CTexture2D::~CTexture2D()
{
	Clear();
}

/*
	-------------------
	| CTextureManager |
	-------------------
*/

CTextureManager::~CTextureManager()
{
	DeleteAll();
}

void CTextureManager::ClearAll()
{
	for ( auto it = m_TextureMap.begin(); it != m_TextureMap.end(); ++it ) {
		it->second->Clear();
	}
}

CTexture2D* CTextureManager::LoadTexture( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, const std::string& sName )
{
	if ( auto it = m_TextureMap.find( sName ); it != m_TextureMap.end() ) {
		return it->second;
	}

	CTexture2D* pTex = new CTexture2D( pd3dDevice, sDirectory, sName );
	m_TextureMap[sName] = pTex;
	return pTex;
}

CTexture2D* CTextureManager::LoadTexture( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, const std::string& sName, int nHeight, int nWidth )
{
	if ( auto it = m_TextureMap.find( sName ); it != m_TextureMap.end() ) {
		return it->second;
	}

	CTexture2D* pTex = new CTexture2D( pd3dDevice, sDirectory, sName, nHeight, nWidth );
	m_TextureMap[sName] = pTex;
	return pTex;
}

CTexture2D* CTextureManager::GetTexture2D( const std::string& sName )
{
	if ( auto iter = m_TextureMap.find( sName ); iter != m_TextureMap.end() ) {
		return iter->second;
	}
	return nullptr;
}

void CTextureManager::DeleteAll()
{
	for ( auto it = m_TextureMap.begin(); it != m_TextureMap.end(); ++it ) {
		delete it->second;
	}

	m_TextureMap.clear();
}
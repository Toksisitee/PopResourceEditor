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

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight, Assets::CPalette* pPalette ) :
	m_pd3dDevice( pd3dDevice ), m_nWidth( nWidth ), m_nHeight( nHeight )
{
	D3DLOCKED_RECT rc;

	pd3dDevice->CreateTexture( nWidth, nHeight, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pD3DTexture, NULL );
	m_pD3DTexture->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
	BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

	for ( int y = 0; y < nHeight; y++ ) {
		for ( int x = 0; x < nWidth; x++ ) {
			const uint8_t uIndex = y * nWidth + x;
			Color* clr = pPalette->GetColor( uIndex );
			WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
		}
	}

	m_pD3DTexture->UnlockRect( 0 );
}

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, int nWidth, int nHeight, uint8_t* pData, Assets::CPalette* pPalette ) :
	m_pd3dDevice( pd3dDevice ), m_nWidth( nWidth ), m_nHeight( nHeight )
{
	D3DLOCKED_RECT rc;

	pd3dDevice->CreateTexture( nWidth, nHeight, 1, 0, D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pD3DTexture, NULL );
	m_pD3DTexture->LockRect( 0, &rc, NULL, D3DLOCK_DISCARD );
	BYTE* pTexels = static_cast<BYTE*>(rc.pBits);

	for ( int y = 0; y < nHeight; y++ ) {
		for ( int x = 0; x < nWidth; x++ ) {
			const size_t uIndex = y * nWidth + x;
			Color* clr = pPalette->GetColor( pData[uIndex] );
			WriteRGBTexel( pTexels, x, y, rc.Pitch, clr );
		}
	}

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

void CTextureManager::AddTexture( const std::string& sName, CTexture2D* pTexture )
{
	m_TextureMap[sName] = pTexture;
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

/*
	-------------------
	| Utils			  |
	-------------------
*/

CTexture2D* CopyTexture( LPDIRECT3DDEVICE9 pd3dDevice, CTexture2D* pSrcTex2D, const std::string& sName, int nDownscale )
{
	if ( !pSrcTex2D || !pSrcTex2D->GetTexture() ) {
		spdlog::error( "CopyTexture: Source texture is nullptr." );
		return nullptr;
	}

	IDirect3DTexture9* pSrcD3DTex = pSrcTex2D->GetTexture();
	int nWidth = nDownscale;
	int nHeight = nDownscale;
	DWORD dwFilter = D3DX_FILTER_BOX;

	if ( nDownscale <= 0 ) {
		D3DSURFACE_DESC desc;
		pSrcD3DTex->GetLevelDesc( 0, &desc );
		nWidth = desc.Width;
		nHeight = desc.Height;
		dwFilter = D3DX_FILTER_NONE;
	}

	CTexture2D* pTexture2D = new CTexture2D( pd3dDevice, nWidth, nHeight );
	IDirect3DTexture9* pDstD3DTex = pTexture2D->GetTexture();

	if ( !pDstD3DTex ) {
		spdlog::error( "CopyTexture: Failed to create destination texture." );
		delete pTexture2D;
		return nullptr;
	}

	IDirect3DSurface9* pSrcSurface = nullptr;
	IDirect3DSurface9* pDstSurface = nullptr;

	if ( SUCCEEDED( pSrcD3DTex->GetSurfaceLevel( 0, &pSrcSurface ) ) &&
		SUCCEEDED( pDstD3DTex->GetSurfaceLevel( 0, &pDstSurface ) ) ) {
		if ( FAILED( D3DXLoadSurfaceFromSurface( pDstSurface, nullptr, nullptr, pSrcSurface, nullptr, nullptr, dwFilter, 0 ) ) ) {
			spdlog::error( "CopyTexture: Failed to copy texture data." );
		}
	}

	if ( pSrcSurface ) pSrcSurface->Release();
	if ( pDstSurface ) pDstSurface->Release();

	return pTexture2D;
}

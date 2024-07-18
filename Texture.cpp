#pragma once
#include <string>
#include <map>
#include <memory>

#include <D3dx9tex.h>
#include <DxErr.h>

#include "Utils.h"
#include "spdlog\spdlog.h"
#include "Texture.h"

CTextureManager g_TextureManager;

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, std::string sTexName, int nHeight, int nWidth ) :
	m_pd3dDevice( pd3dDevice ), m_nHeight( nHeight ), m_nWidth( nWidth )
{
	HRESULT hr;
	std::string sPath = Util::FileSystem::GetAssetsDirectory() + sDirectory + "\\" + sTexName;
	std::wstring wStr = std::wstring( sPath.begin(), sPath.end() );
	if ( FAILED( (hr = D3DXCreateTextureFromFileEx( pd3dDevice, wStr.c_str(), nWidth, nHeight, D3DX_DEFAULT, 0,
		 D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &m_pTexture )) ) ) {
		spdlog::error( "Failed to create Texture2D\nName: {}\nWidth: {}, Height: {}\nError: {} :: {}",
			sPath.c_str(), nWidth, nHeight, DXGetErrorStringA( hr ), DXGetErrorDescriptionA( hr ) );
		return;
	}
}

CTexture2D::CTexture2D( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sDirectory, std::string sTexName ) :
	m_pd3dDevice( pd3dDevice ), m_nHeight( 0 ), m_nWidth( 0 )
{
	HRESULT hr;
	std::string sPath = Util::FileSystem::GetAssetsDirectory() + sDirectory + "\\" + sTexName;
	std::wstring wStr = std::wstring( sPath.begin(), sPath.end() );

	if ( FAILED( (hr = D3DXCreateTextureFromFile( pd3dDevice, wStr.c_str(), &m_pTexture )) ) ) {
		spdlog::error( "Failed to create Texture2D\nName: {}\nError: {} :: {}",
			sPath.c_str(), DXGetErrorStringA( hr ), DXGetErrorDescriptionA( hr ) );
		return;
	}

	D3DSURFACE_DESC desc;
	m_pTexture->GetLevelDesc( 0, &desc );
	m_nHeight = desc.Height;
	m_nWidth = desc.Width;
}

void CTexture2D::Clear()
{
	if ( m_pTexture ) {
		m_pTexture = 0;
	}
}

CTexture2D::~CTexture2D()
{
	if ( m_pTexture ) {
		m_pTexture->Release();
		m_pTexture = 0;
	}
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
/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <string>
#include <functional>

typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;

class CWindowBase {
public:
	CWindowBase( const std::string& sName ) : CWindowBase( nullptr, sName ) {}
	CWindowBase( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) :
		m_pd3dDevice( pd3dDevice ), m_sWindowName( sName ), m_bFirstPass( false )
	{
	}
	virtual ~CWindowBase() = default;

	virtual void RenderBegin() = 0;
	virtual void Render() = 0;
	virtual void RenderEnd() = 0;
	virtual void OnPaletteChange() = 0;
	virtual void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;
	virtual void Cleanup() = 0;

	void SetWindowName( std::string sWindowName ) { m_sWindowName = sWindowName; }
	void SetAssetName( std::string sAssetName ) { m_sAssetName = sAssetName; }
	[[nodiscard]] virtual const std::string& GetWindowName() const { return m_sWindowName; }
	[[nodiscard]] virtual const std::string& GetAssetName() const { return m_sAssetName; }
	[[nodiscard]] bool IsOpen() const { return m_bOpen; }

protected:
	bool                m_bFirstPass = false;
	bool                m_bOpen = true;
	std::string         m_sWindowName;
	std::string			m_sAssetName;
	LPDIRECT3DDEVICE9   m_pd3dDevice;
};


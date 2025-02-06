#pragma once
#include <string>
#include <d3d9.h>

#include "Ghost.h"
#include "WindowBase.h"

class CGhostWnd : public CWindowBase {
public:
	CGhostWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CGhost* GetAsset()
	{
		return &m_Ghost;
	}

	void Cleanup() override
	{}

private:
	Assets::CGhost m_Ghost;
};
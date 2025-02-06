#pragma once
#include <string>
#include <d3d9.h>

#include "WindowBase.h"
#include "Sky.h"

class CSkyWnd : public CWindowBase {
public:
	CSkyWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CSky* GetAsset()
	{
		return &m_Sky;
	}

	void Cleanup() override
	{}

private:
	Assets::CSky m_Sky;
};
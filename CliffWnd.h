#pragma once
#include <string>
#include <d3d9.h>

#include "Cliff.h"
#include "WindowBase.h"

class CCliffWnd : public CWindowBase {
public:
	CCliffWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CCliff* GetAsset()
	{
		return &m_Cliff;
	}

	void Cleanup() override
	{}

private:
	Assets::CCliff m_Cliff;
};
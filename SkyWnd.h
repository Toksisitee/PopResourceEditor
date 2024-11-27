#pragma once
#include <string>
#include <d3d9.h>

#include "Palette.h"
#include "Sky.h"
#include "WindowBase.h"

class CSkyWnd : public CWindowBase {
public:
	CSkyWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	void Cleanup() override
	{}

private:
	Assets::CPalette m_Palette;
	Assets::CSky m_Sky;
};
#pragma once
#include "WindowBase.h"
#include "Palette.h"

class CPaletteWnd : public CWindowBase {
public:
	CPaletteWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CPalette* GetAsset()
	{
		return &m_Palette;
	}

	void Cleanup() override
	{
	}

private:
	Assets::CPalette m_Palette;
	bool m_bRenderTexture = true;
};
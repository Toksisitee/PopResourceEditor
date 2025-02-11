#pragma once
#include "WindowBase.h"
#include "Fade.h"

class CFadeWnd : public CWindowBase {
public:
	CFadeWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CFade* GetAsset()
	{
		return &m_Fade;
	}

	void Cleanup() override
	{}

private:
	Assets::CFade m_Fade;
};
#pragma once
#include "WindowBase.h"
#include "Fade.h"

class CFadeWnd : public CWindowBase {
public:
	CFadeWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

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
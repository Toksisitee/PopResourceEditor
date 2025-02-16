#pragma once
#include "WindowBase.h"
#include "Alpha.h"

class CAlphaWnd : public CWindowBase {
public:
	CAlphaWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CAlpha* GetAsset()
	{
		return &m_Alpha;
	}

	void Cleanup() override
	{}

private:
	Assets::CAlpha m_Alpha;
};
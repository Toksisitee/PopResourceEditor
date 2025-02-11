#pragma once
#include "WindowBase.h"
#include "BigFade.h"

class CBigFadeWnd : public CWindowBase {
public:
	CBigFadeWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CBigFade* GetAsset()
	{
		return &m_BigFade;
	}

	void Cleanup() override
	{}

private:
	Assets::CBigFade m_BigFade;
};
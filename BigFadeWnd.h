#pragma once
#include "WindowBase.h"
#include "BigFade.h"

class CBigFadeWnd : public CWindowBase {
public:
	CBigFadeWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;
	void OnPaletteChange() override {};

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
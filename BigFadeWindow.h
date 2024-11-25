#pragma once
#include <string>
#include <d3d9.h>

#include "BigFade.h"
#include "WindowBase.h"

class CBigFadeWindow : public CWindowBase {
public:
	CBigFadeWindow( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	void Cleanup() override
	{}

private:
	Assets::CBigFade m_BigFade;
};
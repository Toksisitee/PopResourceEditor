#pragma once
#include "WindowBase.h"
#include "Ghost.h"

class CGhostWnd : public CWindowBase {
public:
	CGhostWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CGhost* GetAsset()
	{
		return &m_Ghost;
	}

	void Cleanup() override
	{}

private:
	Assets::CGhost m_Ghost;
};
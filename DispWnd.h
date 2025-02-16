#pragma once
#include "WindowBase.h"
#include "Disp.h"

class CDispWnd : public CWindowBase {
public:
	CDispWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	Assets::CDisp* GetAsset()
	{
		return &m_Disp;
	}

	void Cleanup() override
	{}

private:
	Assets::CDisp m_Disp;
};
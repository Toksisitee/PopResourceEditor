#pragma once
#include <string>
#include <d3d9.h>

#include "Disp.h"
#include "WindowBase.h"

class CDispWnd : public CWindowBase {
public:
	CDispWnd( LPDIRECT3DDEVICE9 pd3dDevice ) :CWindowBase( pd3dDevice ) {}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	void Cleanup() override
	{}

private:
	Assets::CDisp m_Disp;
};
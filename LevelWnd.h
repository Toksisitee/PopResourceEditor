#pragma once
#include "WindowBase.h"
#include "Level.h"

class CLevelWnd : public CWindowBase {
public:
	CLevelWnd( LPDIRECT3DDEVICE9 pd3dDevice ) : CWindowBase( pd3dDevice )
	{
		Initialize( pd3dDevice );
	}
	CLevelWnd( LPDIRECT3DDEVICE9 pd3dDevice, std::string sLevel )
		: CWindowBase( pd3dDevice ), m_sLevel( sLevel )
	{
		Initialize( pd3dDevice );
	}

	void Render() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
		m_bHighRes = true;
		m_bPointFiltering = true;
		m_bRenderWater = true;
		m_bRegenerate = true;
		//m_fv2LightDir = { -0.6f, -0.6f };
		m_fLightDirX = -0.6f;
		m_fLightDirY = -0.6f;
		m_uCliffTreshold = 10;
	}

	Assets::CLevel* GetAsset()
	{
		return &m_Level;
	}

	void Cleanup() override
	{
	}

private:
	Assets::CPalette m_Palette;
	Assets::CLevel m_Level;
	bool           m_bHighRes;
	bool           m_bPointFiltering;
	bool		   m_bRenderWater;
	bool		   m_bRegenerate;
	//fVec2		   m_fv2LightDir;
	float		   m_fLightDirX;
	float		   m_fLightDirY;
	uint16_t	   m_uCliffTreshold;
	std::string	   m_sLevel;
};
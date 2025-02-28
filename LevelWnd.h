/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include "WindowBase.h"
#include "Level.h"

class CLevelWnd : public CWindowBase {
public:
	CLevelWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) 
	{
		Initialize( pd3dDevice );
	}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;
	void OnPaletteChange() override {};

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

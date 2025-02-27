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
#include "Cliff.h"

class CCliffWnd : public CWindowBase {
public:
	CCliffWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;
	void OnPaletteChange() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
		m_eLastMode = Assets::Cliff::Generation::NO_LUMINANCE;
	}

	Assets::CCliff* GetAsset()
	{
		return &m_Cliff;
	}

	void Cleanup() override
	{}

private:
	Assets::CCliff m_Cliff;
	Assets::Cliff::Generation m_eLastMode;
};

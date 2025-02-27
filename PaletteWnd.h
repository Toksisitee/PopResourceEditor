/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <memory>
#include <unordered_set>

#include "WindowBase.h"
#include "Palette.h"

class CPaletteWnd : public CWindowBase {
public:
	CPaletteWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName )
	{
		Initialize( pd3dDevice );
		m_pPalette = std::make_shared<Assets::CPalette>();
	}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;
	void OnPaletteChange() override {};

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
	}

	std::shared_ptr<Assets::CPalette> GetAsset()
	{
		return m_pPalette;
	}

	void Cleanup() override
	{
	}

private:
	std::shared_ptr<Assets::CPalette> m_pPalette;
	std::unordered_set<uint8_t> m_osuSelectedIndicies;
	bool m_bRenderTexture = false;
};

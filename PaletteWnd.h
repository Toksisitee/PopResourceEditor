#pragma once
#include <memory>

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
	bool m_bRenderTexture = false;
};
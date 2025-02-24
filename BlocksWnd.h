#pragma once
#include "WindowBase.h"
#include "Blocks.h"

class CBlocksWnd : public CWindowBase {
public:
	CBlocksWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) {}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;
	void OnPaletteChange() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
		m_bDrawAtlas = false;
	}

	Assets::CBlocks* GetAsset()
	{
		return &m_Blocks;
	}

	void Cleanup() override
	{}

public:
	bool m_bDrawAtlas = false;
private:
	Assets::CBlocks m_Blocks;
};
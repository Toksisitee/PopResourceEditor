#pragma once
#include <vector>
#include <memory>

#include "WindowBase.h"
#include "AlphaWnd.h"
#include "BigFadeWnd.h"
#include "BlocksWnd.h"
#include "CliffWnd.h"
#include "DispWnd.h"
#include "FadeWnd.h"
#include "GhostWnd.h"
#include "PaletteWnd.h"
#include "SkyWnd.h"

class CTextureSetWnd : public CWindowBase {
public:
	CTextureSetWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName ) 
	{
		Initialize( pd3dDevice );
	}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;

		m_Windows.emplace_back( std::make_unique<CPaletteWnd>( m_pd3dDevice, "Palette" ) );
		m_Windows.emplace_back( std::make_unique<CAlphaWnd>( m_pd3dDevice, "Alpha" ) );
		m_Windows.emplace_back( std::make_unique<CBigFadeWnd>( m_pd3dDevice, "BigFade" ) );
		m_Windows.emplace_back( std::make_unique<CBlocksWnd>( m_pd3dDevice, "Blocks" ) );
		m_Windows.emplace_back( std::make_unique<CCliffWnd>( m_pd3dDevice, "Cliff" ) );
		m_Windows.emplace_back( std::make_unique<CDispWnd>( m_pd3dDevice, "Disp" ) );
		m_Windows.emplace_back( std::make_unique<CFadeWnd>( m_pd3dDevice, "Fade" ) );
		m_Windows.emplace_back( std::make_unique<CGhostWnd>( m_pd3dDevice, "Ghost" ) );
		m_Windows.emplace_back( std::make_unique<CSkyWnd>( m_pd3dDevice, "Sky" ) );

		for ( auto& pWnd : m_Windows ) {
			pWnd->Initialize( pd3dDevice );
		}
	}

	void Cleanup() override
	{
		m_Windows.clear();
	}

private:
	std::vector<std::unique_ptr<CWindowBase>> m_Windows;
};
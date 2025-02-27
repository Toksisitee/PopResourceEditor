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
private:
	enum class Wnd : uint8_t
	{
		Palette,
		Alpha,
		BigFade,
		Blocks,
		Cliff,
		Disp,
		Fade,
		Ghost,
		Sky,
		Max,
	};

	inline constexpr size_t IndexOf( Wnd eIndex )
	{
		return static_cast<size_t>(eIndex);
	}

	template <Wnd eIndex>
	struct WindowType;
	template <>
	struct WindowType<Wnd::Palette> { using type = CPaletteWnd; };
	template <>
	struct WindowType<Wnd::Alpha> { using type = CAlphaWnd; };
	template <>
	struct WindowType<Wnd::BigFade> { using type = CBigFadeWnd; };
	template <>
	struct WindowType<Wnd::Blocks> { using type = CBlocksWnd; };
	template <>
	struct WindowType<Wnd::Cliff> { using type = CCliffWnd; };
	template <>
	struct WindowType<Wnd::Disp> { using type = CDispWnd; };
	template <>
	struct WindowType<Wnd::Fade> { using type = CFadeWnd; };
	template <>
	struct WindowType<Wnd::Ghost> { using type = CGhostWnd; };
	template <>
	struct WindowType<Wnd::Sky> { using type = CSkyWnd; };

	template <Wnd eIndex>
	typename WindowType<eIndex>::type* GetWindow()
	{
		return static_cast<typename WindowType<eIndex>::type*>(m_Windows[IndexOf( eIndex )].get());
	}
public:
	CTextureSetWnd( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) : CWindowBase( pd3dDevice, sName )
	{
		Initialize( pd3dDevice );
	}

	void RenderBegin() override;
	void Render() override;
	void RenderEnd() override;
	void OnPaletteChange() override {};

	void SetAssetNames()
	{
		m_Windows[IndexOf( Wnd::Palette )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Palette ) );
		m_Windows[IndexOf( Wnd::Alpha )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Alpha ) );
		m_Windows[IndexOf( Wnd::BigFade )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::BigFade ) );
		m_Windows[IndexOf( Wnd::Blocks )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Blocks ) );
		m_Windows[IndexOf( Wnd::Cliff )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Cliff ) );
		m_Windows[IndexOf( Wnd::Disp )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Disp ) );
		m_Windows[IndexOf( Wnd::Fade )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Fade ) );
		m_Windows[IndexOf( Wnd::Ghost )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Ghost ) );
		m_Windows[IndexOf( Wnd::Sky )].get()->SetAssetName( Assets::GenerateAssetName( m_szIdentifier, Assets::FileType::Sky ) );
	}

	void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) override
	{
		m_pd3dDevice = pd3dDevice;
		m_Windows.resize( IndexOf( Wnd::Max ) );

		m_Windows[IndexOf( Wnd::Palette )] = std::make_unique<CPaletteWnd>( m_pd3dDevice, "Palette" );
		m_Windows[IndexOf( Wnd::Alpha )] = std::make_unique<CAlphaWnd>( m_pd3dDevice, "Alpha" );
		m_Windows[IndexOf( Wnd::BigFade )] = std::make_unique<CBigFadeWnd>( m_pd3dDevice, "BigFade" );
		m_Windows[IndexOf( Wnd::Blocks )] = std::make_unique<CBlocksWnd>( m_pd3dDevice, "Blocks" );
		m_Windows[IndexOf( Wnd::Cliff )] = std::make_unique<CCliffWnd>( m_pd3dDevice, "Cliff" );
		m_Windows[IndexOf( Wnd::Disp )] = std::make_unique<CDispWnd>( m_pd3dDevice, "Disp" );
		m_Windows[IndexOf( Wnd::Fade )] = std::make_unique<CFadeWnd>( m_pd3dDevice, "Fade" );
		m_Windows[IndexOf( Wnd::Ghost )] = std::make_unique<CGhostWnd>( m_pd3dDevice, "Ghost" );
		m_Windows[IndexOf( Wnd::Sky )] = std::make_unique<CSkyWnd>( m_pd3dDevice, "Sky" );

		SetAssetNames();

		for ( auto& pWnd : m_Windows ) {
			pWnd->Initialize( pd3dDevice );
		}

#define SET_PAL(eWnd, pPalette) GetWindow<eWnd>()->GetAsset()->SetPalette(pPalette)
		std::shared_ptr<Assets::CPalette> pPalette = GetWindow<Wnd::Palette>()->GetAsset();
		for ( size_t i = 1; i < IndexOf( Wnd::Max ); i++ ) {
			switch ( static_cast<Wnd>(i) ) {
				case Wnd::Alpha:   SET_PAL( Wnd::Alpha, pPalette );   break;
				case Wnd::BigFade: SET_PAL( Wnd::BigFade, pPalette ); break;
				case Wnd::Blocks:  SET_PAL( Wnd::Blocks, pPalette );  break;
				case Wnd::Cliff:   SET_PAL( Wnd::Cliff, pPalette );   break;
				case Wnd::Disp:    SET_PAL( Wnd::Disp, pPalette );    break;
				case Wnd::Fade:    SET_PAL( Wnd::Fade, pPalette );    break;
				case Wnd::Ghost:   SET_PAL( Wnd::Ghost, pPalette );   break;
				case Wnd::Sky:     SET_PAL( Wnd::Sky, pPalette );     break;
				default: break;
			}
		}
#undef SET_PAL
	}

	void Cleanup() override
	{
		m_Windows.clear();
	}

private:
	char m_szIdentifier[2] = "0";
	bool m_bEditTextureSlot;
	std::vector<std::unique_ptr<CWindowBase>> m_Windows;
};
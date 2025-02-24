#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "TextureSetWnd.h"
#include "imgui_internal.h"

void CTextureSetWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CTextureSetWnd::RenderEnd()
{
	ImGui::End();
}

void CTextureSetWnd::Render()
{
	if ( ImGui::BeginTabBar( "Windows" ) ) {
		for ( auto& pWnd : m_Windows ) {
			if ( ImGui::BeginTabItem( pWnd->GetWindowName().c_str() ) ) {
				pWnd->Render();
				ImGui::EndTabItem();
			}
		}

		auto* pPalette = GetWindow<Wnd::Palette>()->GetAsset().get();
		if ( pPalette->HasChanged() ) {
			for ( auto& pWnd : m_Windows ) {
				pWnd->OnPaletteChange();
			}
			pPalette->SetChanged( false );
		}
		ImGui::EndTabBar();
	}
}
#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "TextureSetWnd.h"
#include "imgui_internal.h"

void CTextureSetWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen, ImGuiWindowFlags_MenuBar );
}

void CTextureSetWnd::RenderEnd()
{
	ImGui::End();
}

void CTextureSetWnd::Render()
{
	if ( ImGui::BeginMenuBar() ) {
		if ( ImGui::BeginMenu( "File" ) ) {
			ImGui::EndMenu();
		}

		char szTextureSlot[16];
		sprintf_s( szTextureSlot, sizeof( szTextureSlot ), "Texture Slot: %s", m_szIdentifier );
		if ( ImGui::MenuItem( szTextureSlot ) ) {
			m_bEditTextureSlot = true;
		}

		if ( m_bEditTextureSlot ) {
			ImGui::SetNextItemWidth( 25 );

			if ( ImGui::InputText( "##Identifier", m_szIdentifier, sizeof( m_szIdentifier ),
				 ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_AutoSelectAll,
				 []( ImGuiInputTextCallbackData* pData ) -> int {
				if ( (pData->EventChar >= '0' && pData->EventChar <= '9') ||
					(pData->EventChar >= 'a' && pData->EventChar <= 'z') ) {
					return 0;
				}
				return 1;
			} ) ) {
				m_bEditTextureSlot = false;
				SetAssetNames();
			}

			if ( m_szIdentifier[0] == '\0' ) {
				m_szIdentifier[0] = '0';
				m_szIdentifier[1] = '\0';
			}
		}

		ImGui::EndMenuBar();
	}

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
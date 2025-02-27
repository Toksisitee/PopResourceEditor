/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

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
			if ( ImGui::MenuItem( "Export All (Bin)" ) ) {
#define EXPORT_BIN(eWnd) { \
							auto* pWnd = GetWindow<eWnd>(); \
							pWnd->GetAsset()->ExportBin( Util::FileSystem::FormatExportPathFromFileName( pWnd->GetAssetName() ) ); \
						 } \

				auto* pPalWnd = GetWindow<Wnd::Palette>();
				pPalWnd->GetAsset().get()->ExportBin( Util::FileSystem::FormatExportPathFromFileName( pPalWnd->GetAssetName() ) );
				for ( size_t i = 1; i < IndexOf( Wnd::Max ); i++ ) {
					switch ( static_cast<Wnd>(i) ) {
						case Wnd::Alpha:   EXPORT_BIN( Wnd::Alpha )   break;
						case Wnd::BigFade: EXPORT_BIN( Wnd::BigFade ) break;
						case Wnd::Blocks:  EXPORT_BIN( Wnd::Blocks )  break;
						case Wnd::Cliff:   EXPORT_BIN( Wnd::Cliff )   break;
						case Wnd::Disp:    EXPORT_BIN( Wnd::Disp )    break;
						case Wnd::Fade:    EXPORT_BIN( Wnd::Fade )    break;
						case Wnd::Ghost:   EXPORT_BIN( Wnd::Ghost )   break;
						case Wnd::Sky:     EXPORT_BIN( Wnd::Sky )     break;
						default: break;
					}
				}
#undef EXPORT_BIN
			}
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

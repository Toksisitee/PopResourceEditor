#include "imgui.h"

#include "ImEditor.h"
#include "PaletteWnd.h"

void CPaletteWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	ImGui::Checkbox( "Render Texture##pal", &m_bRenderTexture );

	if ( !m_bFirstPass ) {
		m_bFirstPass = true;
	}

	if ( m_Palette.GetTexture() == nullptr ) {
		m_Palette.CreateTexture( m_pd3dDevice );
	}
	else {
		if ( m_bRenderTexture ) {
			ImEditor::SetPointFiltering( m_pd3dDevice );
			ImEditor::RenderTexture( m_Palette.GetTexture() );
			ImEditor::ResetRenderState();
		}
	}

	ImGui::NewLine();

	if ( !m_bRenderTexture ) {
		uint8_t uIndex = 0;
		char szColorLabel[8];
		const int k_iColorEditFlags = ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoBorder;
		Color* pPalette = m_Palette.GetColorTable();

		auto getColor = [&pPalette]( uint8_t uIndex ) -> float* {
			float fColors[4];
			ImColor col = ImColor( pPalette[uIndex].r, pPalette[uIndex].g, pPalette[uIndex].b );
			memcpy( &fColors, &col, sizeof( float ) * 4 );
			return fColors;
		};

		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 5.0f, 5.0f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );
		for ( uint32_t y = 0; y < 16; y++ ) {
			for ( uint32_t x = 0; x < 16; x++ ) {
				sprintf_s( szColorLabel, sizeof( szColorLabel ), "%i", uIndex );
				ImGui::SameLine();
				ImGui::ColorEdit3( szColorLabel, getColor( uIndex ), k_iColorEditFlags );
				uIndex++;
			}
			ImGui::NewLine();
		}
		ImGui::PopStyleVar( 2 );
	}

	ImGui::End();
}
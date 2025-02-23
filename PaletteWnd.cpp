#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "PaletteWnd.h"

void CPaletteWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CPaletteWnd::RenderEnd()
{
	ImGui::End();
}

void CPaletteWnd::Render()
{
	if ( ImGui::Button( "Load Palette" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Palette.LoadBin( *osFilePath ) );
			m_Palette.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Palette.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Palette.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ImGui::Checkbox( "Render Texture##pal", &m_bRenderTexture );

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
		const int k_iColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoBorder;
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
				sprintf_s( szColorLabel, sizeof( szColorLabel ), "##%i", uIndex );
				ImGui::SameLine();
				ImGui::ColorEdit3( szColorLabel, getColor( uIndex ), k_iColorEditFlags );
				if ( ImGui::IsItemHovered() ) {
					ImGui::BeginTooltip();
					ImGui::Text( "Index: %d", uIndex );
					ImGui::EndTooltip();
				}
				uIndex++;
			}
			ImGui::NewLine();
		}
		ImGui::PopStyleVar( 2 );
	}
}
#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "BigFadeWnd.h"

void CBigFadeWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CBigFadeWnd::RenderEnd()
{
	ImGui::End();
}

void CBigFadeWnd::Render()
{
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_BigFade.LoadImg( *osFilePath ) );
			m_BigFade.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	// Palette
	{
		uint8_t uIndex = 0;
		char szColorLabel[8];
		const int k_iColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoBorder;
		Color* pPalette = m_BigFade.GetPalette()->GetColorTable();

		auto getColor = [&pPalette]( uint8_t uIndex ) -> float* {
			float fColors[4];
			ImColor col = ImColor( pPalette[uIndex].r, pPalette[uIndex].g, pPalette[uIndex].b );
			memcpy( &fColors, &col, sizeof( float ) * 4 );
			return fColors;
		};

		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 5.0f, 5.0f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );
		for ( uint32_t uIndex = Assets::BigFade::k_uColorStart;
			  uIndex < (Assets::BigFade::k_uColorStart + Assets::BigFade::k_uNumColors);
			  uIndex++ ) {
			if ( uIndex % 16 != 0 ) {
				ImGui::SameLine();
			}

			char szColorLabel[16];
			sprintf_s( szColorLabel, sizeof( szColorLabel ), "##%i", uIndex );
			ImGui::ColorEdit3( szColorLabel, getColor( uIndex ), k_iColorEditFlags );

			if ( ImGui::IsItemHovered() ) {
				ImGui::BeginTooltip();
				ImGui::Text( "Index: %d", uIndex );
				ImGui::EndTooltip();
			}
		}
		ImGui::PopStyleVar( 2 );
	}

	if ( m_BigFade.GetTexture() == nullptr ) {
		m_BigFade.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_BigFade.GetTexture() );
		ImEditor::ResetRenderState();
	}
}
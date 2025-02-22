#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "SkyWnd.h"

void CSkyWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( ImGui::Button( "Load Palette" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Sky.GetPalette()->LoadBin( *osFilePath ) );
			m_Sky.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Bin" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Sky.LoadBin( *osFilePath ) );
			m_Sky.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_Sky.LoadImg( *osFilePath ) );
			m_Sky.DestroyTexture();
		}
	}

	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Sky.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Sky.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	if ( m_Sky.GetTexture() == nullptr ) {
		m_Sky.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		auto pSkyTex = m_Sky.GetTexture();
		ImEditor::RenderTexture( m_Sky.GetTexture() );
		ImEditor::ResetRenderState();
	}

	uint8_t uIndex = 0;
	char szColorLabel[8];
	const int k_iColorEditFlags = ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoBorder;
	Color* pPalette = m_Sky.GetPalette()->GetColorTable();

	auto getColor = [&pPalette]( uint8_t uIndex ) -> float* {
		float fColors[4];
		ImColor col = ImColor( pPalette[uIndex].r, pPalette[uIndex].g, pPalette[uIndex].b );
		memcpy( &fColors, &col, sizeof( float ) * 4 );
		return fColors;
	};

	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 5.0f, 5.0f ) );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );
	for ( uint32_t uIndex = Assets::Sky::k_uColorStart; 
		  uIndex < (Assets::Sky::k_uColorStart + Assets::Sky::k_uNumColors); 
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

	ImGui::End();
}
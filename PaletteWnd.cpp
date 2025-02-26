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
			g_ErrHandler.HandleResult( m_pPalette.get()->LoadBin( *osFilePath ) );
			m_pPalette.get()->DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_pPalette.get()->ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_pPalette.get()->ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ImGui::Checkbox( "Render Texture##pal", &m_bRenderTexture );

	if ( m_pPalette.get()->GetTexture() == nullptr ) {
		m_pPalette.get()->CreateTexture( m_pd3dDevice );
	}
	else {
		if ( m_bRenderTexture ) {
			ImEditor::SetPointFiltering( m_pd3dDevice );
			ImEditor::RenderTexture( m_pPalette.get()->GetTexture() );
			ImEditor::ResetRenderState();
		}
	}

	ImGui::NewLine();

	if ( !m_bRenderTexture ) {
		ImEditor::RenderModifiablePalette( static_cast<void*>(m_pPalette.get()), 0, Assets::Palette::k_uNumColors, &m_osuSelectedIndicies );
	}
}
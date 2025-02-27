#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "GhostWnd.h"

void CGhostWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CGhostWnd::RenderEnd()
{
	ImGui::End();
}

void CGhostWnd::Render()
{
	if ( ImGui::Button( "Load Palette" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Ghost.GetPalette()->LoadBin( *osFilePath ) );
			m_Ghost.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Bin" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Ghost.LoadBin( *osFilePath ) );
			m_Ghost.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_Ghost.LoadImg( *osFilePath ) );
			m_Ghost.DestroyTexture();
		}
	}

	if ( ImGui::Button( "Generate" ) ) {
		g_ErrHandler.HandleResult( m_Ghost.Generate() );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Ghost.ExportImg( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Ghost.ExportBin( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	}

	static const uint8_t uMaxOpacity = 100;
	if ( ImEditor::SliderScalar( "Opacity", &m_Ghost.m_uOpacity, NULL, &uMaxOpacity ) ) {
		m_Ghost.Generate();
	}

	if ( m_Ghost.GetTexture() == nullptr ) {
		m_Ghost.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Ghost.GetTexture() );
		ImEditor::ResetRenderState();
	}
}

void CGhostWnd::OnPaletteChange()
{
	m_Ghost.DestroyTexture();
	m_Ghost.Generate();
}
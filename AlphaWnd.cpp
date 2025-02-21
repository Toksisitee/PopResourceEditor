#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "AlphaWnd.h"

void CAlphaWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( ImGui::Button( "Load Palette" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Alpha.GetPalette()->LoadBin( *osFilePath ) );
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Generate" ) ) {
		g_ErrHandler.HandleResult( m_Alpha.Generate() );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Alpha.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Alpha.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	if ( m_Alpha.GetTexture() == nullptr ) {
		m_Alpha.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Alpha.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "CliffWnd.h"

void CCliffWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CCliffWnd::RenderEnd()
{
	ImGui::End();
}

void CCliffWnd::Render()
{
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_Cliff.LoadImg( *osFilePath ) );
			m_Cliff.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Cliff.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Cliff.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	if ( ImGui::Button( "Generate (NO_LUMINANCE)" ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::NO_LUMINANCE );
		m_eLastMode = Assets::Cliff::Generation::NO_LUMINANCE;
	} ImGui::SameLine();
	if ( ImGui::Button( "Generate (LUMINANCE_1) " ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::LUMINANCE_1 );
		m_eLastMode = Assets::Cliff::Generation::LUMINANCE_1;

	}
	if ( ImGui::Button( "Generate (LUMINANCE_2)" ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::LUMINANCE_2 );
		m_eLastMode = Assets::Cliff::Generation::LUMINANCE_2;

	}
	if ( ImEditor::InputScalar( "m_fLuminance", &m_Cliff.m_fLuminance ) ) {
		if ( m_eLastMode == Assets::Cliff::Generation::NO_LUMINANCE ) {
			m_eLastMode = Assets::Cliff::Generation::LUMINANCE_1;
		}
		m_Cliff.Generate( m_eLastMode );
	}

	if ( m_Cliff.GetTexture() == nullptr ) {
		m_Cliff.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Cliff.GetTexture() );
		ImEditor::ResetRenderState();
	}
}

void CCliffWnd::OnPaletteChange()
{
	m_Cliff.DestroyTexture();
	m_Cliff.Generate( m_eLastMode );
}
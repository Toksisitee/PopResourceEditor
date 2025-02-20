#include "imgui.h"

#include "AssetsErrHandler.h"
#include "FileDialog.h"
#include "ImEditor.h"
#include "Utils.h"

#include "CliffWnd.h"

void CCliffWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( ImGui::Button( "Load Image" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) {
			m_PendingTask = [this, sFilePath]() {
				g_ErrHandler.HandleResult( m_Cliff.LoadImg( sFilePath ) );
				m_Cliff.DestroyTexture();
			};
		} );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Cliff.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Cliff.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ProcessTask();

	if ( ImGui::Button( "Generate (NO_LUMINANCE)" ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::NO_LUMINANCE );
	} ImGui::SameLine();
	if ( ImGui::Button( "Generate (LUMINANCE_1) " ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::LUMINANCE_1 );
	}
	if ( ImGui::Button( "Generate (LUMINANCE_2)" ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::LUMINANCE_2 );
	} 
	if ( ImEditor::InputScalar( "m_fLuminance", &m_Cliff.m_fLuminance ) ) {
		m_Cliff.Generate( Assets::Cliff::Generation::NO_LUMINANCE );
	}

	if ( m_Cliff.GetTexture() == nullptr ) {
		m_Cliff.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Cliff.GetTexture() );
		ImEditor::ResetRenderState();
	}

	ImGui::End();
}
#include "imgui.h"

#include "AssetsErrHandler.h"
#include "FileDialog.h"
#include "ImEditor.h"
#include "Utils.h"

#include "DispWnd.h"

void CDispWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( ImGui::Button( "Load Image" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) {
			m_PendingTask = [this, sFilePath]() {
				g_ErrHandler.HandleResult( m_Disp.LoadImg( sFilePath ) );
				m_Disp.DestroyTexture();
			};
		} );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Disp.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Disp.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ProcessTask();

	if ( m_Disp.GetTexture() == nullptr ) {
		m_Disp.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Disp.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
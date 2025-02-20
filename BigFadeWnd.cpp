#include "imgui.h"

#include "Utils.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "FileDialog.h"
#include "BigFadeWnd.h"

void CBigFadeWnd::OnLoadImage( const std::string& sFilePath )
{
	g_ErrHandler.HandleResult( m_BigFade.LoadImg( sFilePath ) );
	m_BigFade.DestroyTexture();
}

void CBigFadeWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( ImGui::Button( "Load Image" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) { 
			m_PendingTask = [this, sFilePath]() { OnLoadImage( sFilePath ); };
		});
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ProcessTask();

	if ( m_BigFade.GetTexture() == nullptr ) {
		m_BigFade.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_BigFade.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
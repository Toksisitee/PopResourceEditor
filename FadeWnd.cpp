#include "imgui.h"

#include "AssetsErrHandler.h"
#include "FileDialog.h"
#include "ImEditor.h"
#include "Utils.h"

#include "FadeWnd.h"

void CFadeWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( ImGui::Button( "Load Palette" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) {
			m_PendingTask = [this, sFilePath]() {
				g_ErrHandler.HandleResult( m_Fade.GetPalette()->LoadBin( sFilePath ) );
				m_Fade.DestroyTexture();
			};
		} );
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Bin" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) {
			m_PendingTask = [this, sFilePath]() {
				g_ErrHandler.HandleResult( m_Fade.LoadBin( sFilePath ) );
				m_Fade.DestroyTexture();
			};
		} );
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Image" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) {
			m_PendingTask = [this, sFilePath]() {
				g_ErrHandler.HandleResult( m_Fade.LoadImg( sFilePath ) );
				m_Fade.DestroyTexture();
			};
		} );
	}

	if ( ImGui::Button( "Generate" ) ) {
		g_ErrHandler.HandleResult( m_Fade.Generate() );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Fade.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Fade.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ProcessTask();

	if ( m_Fade.GetTexture() == nullptr ) {
		m_Fade.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Fade.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
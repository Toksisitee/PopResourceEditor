#include "imgui.h"

#include "Utils.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "FileDialog.h"
#include "BlocksWnd.h"

void CBlocksWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CBlocksWnd::RenderEnd()
{
	ImGui::End();
}

void CBlocksWnd::Render()
{
	if ( ImGui::Button( "Load Image" ) ) {
		CFileDialogManager::GetInstance().ShowFileDialog( FileDialog::FileDialogType::OpenFile,
				  [this]( const std::string& sFilePath ) {
			m_PendingTask = [this, sFilePath]() { 
				g_ErrHandler.HandleResult( m_Blocks.LoadImg( sFilePath ) );
				m_Blocks.DestroyTexture();
			};
		} );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Blocks.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Blocks.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	if ( m_Blocks.GetTexture() == nullptr ) {
		for ( size_t i = 0; i < Assets::Blocks::k_uNumBlocks; i++ ) {
			m_Blocks.CreateSubTexture( m_pd3dDevice, i );
		}
		m_Blocks.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );

		ImGui::Checkbox( "Draw Atlas", &m_bDrawAtlas );
		if (m_bDrawAtlas) {
			ImEditor::RenderTexture( m_Blocks.GetTexture() );
		}
		else {
			size_t uTexturesCountRow = Assets::Blocks::k_uWidth / Assets::Blocks::k_uBlockWidth;
			size_t uTexturesCountCol = Assets::Blocks::k_uHeight / Assets::Blocks::k_uBlockHeight;
			for ( size_t i = 0; i < uTexturesCountCol; i++ ) {
				for ( size_t j = 0; j < uTexturesCountRow; j++ ) {
					ImEditor::ImageButton( m_Blocks.GetTexture( j * uTexturesCountRow + i ) );
					ImGui::SameLine();
				}
				ImGui::NewLine();
			}
		}

		ImEditor::ResetRenderState();
	}
}

void CBlocksWnd::OnPaletteChange()
{
	m_Blocks.DestroyTextures();
}
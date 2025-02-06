#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "BlocksWnd.h"

void CBlocksWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
	auto pPalette = m_Blocks.GetPalette();

	if ( !m_bFirstPass ) {
		//auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		//g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		//sFilePath = Util::FileSystem::FormatPath( "BL320-U.dat" );
		//g_ErrHandler.HandleResult( m_Blocks.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Blocks.ExportBin( f );
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


	ImGui::End();
}
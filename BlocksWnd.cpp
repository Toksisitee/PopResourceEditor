#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "BlocksWnd.h"

void CBlocksWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );
	auto pPalette = m_Blocks.GetPalette();

	if ( !m_bFirstPass ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "BL320-U.dat" );
		g_ErrHandler.HandleResult( m_Blocks.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Blocks.Generate( f );
	}

	if ( m_Blocks.GetTexture() == nullptr ) {
		m_Blocks.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Blocks.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
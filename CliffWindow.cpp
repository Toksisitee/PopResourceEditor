#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "CliffWindow.h"

void CCliffWindow::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );
	auto pPalette = m_Cliff.GetPalette();

	if ( !m_bFirstPass ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "cliff0-u.dat" );
		g_ErrHandler.HandleResult( m_Cliff.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Cliff.Generate( f );
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
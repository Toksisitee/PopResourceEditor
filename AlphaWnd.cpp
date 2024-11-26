#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "AlphaWnd.h"

void CAlphaWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );
	auto pPalette = m_Alpha.GetPalette();

	if ( !m_bFirstPass ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "al0-u.dat" );
		g_ErrHandler.HandleResult( m_Alpha.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Alpha.Generate( f );
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
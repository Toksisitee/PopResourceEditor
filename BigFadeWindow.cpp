#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "BigFadeWindow.h"

void CBigFadeWindow::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );
	auto pPalette = m_BigFade.GetPalette();

	if ( !m_bFirstPass ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "bigf0-u.dat" );
		g_ErrHandler.HandleResult( m_BigFade.Load( sFilePath ) );
		m_bFirstPass = true;
	}

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
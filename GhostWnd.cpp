#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "GhostWnd.h"

void CGhostWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );
	auto pPalette = m_Ghost.GetPalette();

	if ( !m_bFirstPass ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "ghost0-u.dat" );
		g_ErrHandler.HandleResult( m_Ghost.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Ghost.Generate( f );
	}

	  uint8_t uMaxOpacity = 100;
	if ( ImEditor::SliderScalar( "Opacity", &m_Ghost.m_uOpacity, NULL, &uMaxOpacity ) ) {
		std::string f;
		m_Ghost.Generate( f );
	}

	if ( m_Ghost.GetTexture() == nullptr ) {
		m_Ghost.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Ghost.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
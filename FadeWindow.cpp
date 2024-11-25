#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "FadeWindow.h"

void CFadeWindow::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );
	auto pPalette = m_Fade.GetPalette();

	if ( !m_bFirstPass ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "fade0-u.dat" );
		g_ErrHandler.HandleResult( m_Fade.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Fade.Generate( f );
	}


	if ( ImEditor::InputScalar( "Dark Lights", &m_Fade.m_NumLightsDark ) ) {
		std::string f;
		m_Fade.Generate( f );
	}
	if ( ImEditor::InputScalar( "Dark Darkness", &m_Fade.m_NumDarknessDark ) ) {
		std::string f;
		m_Fade.Generate( f );
	}

	if ( ImEditor::InputScalar( "Light Lights", &m_Fade.m_NumLightsLight ) ) {
		std::string f;
		m_Fade.Generate( f );
	}
	if ( ImEditor::InputScalar( "Light Darkness", &m_Fade.m_NumDarknessLight ) ) {
		std::string f;
		m_Fade.Generate( f );
	}


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
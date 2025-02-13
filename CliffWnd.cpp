#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "CliffWnd.h"

void CCliffWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
	auto pPalette = m_Cliff.GetPalette();

	if ( !m_bFirstPass ) {
		//auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		//g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		//sFilePath = Util::FileSystem::FormatPath( "cliff0-u.dat" );
		//g_ErrHandler.HandleResult( m_Cliff.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate no lum" ) ) {
		std::string f;
		m_Cliff.Generate( 0 );
	}
	if ( ImGui::Button( "Generate w/ lum" ) ) {
		m_Cliff.Generate( 1 );
	}
	if ( ImGui::Button( "Generate w/ lum 2" ) ) {
		m_Cliff.Generate( 2 );
	}

	if ( ImEditor::InputScalar( "m_fLuminance", &m_Cliff.m_fLuminance ) ) {
		m_Cliff.Generate( 1 );
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
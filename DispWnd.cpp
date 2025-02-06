#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "DispWnd.h"

void CDispWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );

	if ( !m_bFirstPass ) {
		//auto sFilePath = Util::FileSystem::FormatPath( "disp0-u.dat" );
		//g_ErrHandler.HandleResult( m_Disp.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Generate" ) ) {
		std::string f;
		m_Disp.Generate( f );
	}

	if ( m_Disp.GetTexture() == nullptr ) {
		m_Disp.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Disp.GetTexture() );
		ImEditor::ResetRenderState();
	}


	ImGui::End();
}
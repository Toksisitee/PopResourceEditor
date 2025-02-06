#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "SkyWnd.h"

void CSkyWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
	auto pPalette = m_Sky.GetPalette();

	//if ( !m_bFirstPass ) {
	//	auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
	//	g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
	//	sFilePath = Util::FileSystem::FormatPath( "sky0-u.dat" );
	//	g_ErrHandler.HandleResult( m_Sky.Load( sFilePath ) );
	//	m_bFirstPass = true;
	//}

	if ( m_Sky.GetTexture() == nullptr ) {
		m_Sky.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		auto pSkyTex = m_Sky.GetTexture();
		ImEditor::RenderTexture( m_Sky.GetTexture() );
		ImEditor::ResetRenderState();
	}

	ImGui::End();
}
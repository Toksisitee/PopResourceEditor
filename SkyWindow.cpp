#include "imgui.h"

#include "Utils.h"
#include "ImEditor.h"
#include "SkyWindow.h"

void CSkyWindow::Render()
{
	ImGui::Begin( m_sWindowName.c_str() );

	static bool init = false;
	if ( !init ) {
		auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		g_ErrHandler.HandleResult( g_Palette.Load( sFilePath ) );
		sFilePath = Util::FileSystem::FormatPath( "sky0-u.dat" );
		g_ErrHandler.HandleResult( g_Sky.Load( sFilePath ) );
		init = true;
	}

	if ( g_Sky.GetTexture() == nullptr ) {
		g_Sky.CreateTexture( m_pd3dDevice, &g_Palette );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		auto pSkyTex = g_Sky.GetTexture();
		const ImVec2 texSize = ImVec2( pSkyTex->GetWidth(), pSkyTex->GetHeight() );
		ImEditor::RenderTexture( pSkyTex, texSize );
		ImEditor::ResetRenderState();

	}

	ImGui::End();
}
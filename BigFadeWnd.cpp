#include "imgui.h"

#include "Utils.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "BigFadeWnd.h"

void CBigFadeWnd::Render()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
	auto pPalette = m_BigFade.GetPalette();

	if ( !m_bFirstPass ) {
		//auto sFilePath = Util::FileSystem::FormatPath( "pal0-u.dat" );
		//g_ErrHandler.HandleResult( pPalette->Load( sFilePath ) );
		//sFilePath = Util::FileSystem::FormatPath( "bigf0-u.dat" );
		//g_ErrHandler.HandleResult( m_BigFade.Load( sFilePath ) );
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Load Image" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.LoadImg( "C:\\Users\\melyg\\Pictures\\bigfade.bmp" ) );
		m_BigFade.DestroyTexture();
	}

	if ( ImGui::Button( "Export" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.ExportImg( "C:\\Users\\melyg\\Pictures\\bigfade.bmp" ) );
	}

	if ( ImGui::Button( "Generate" ) ) {
		g_ErrHandler.HandleResult( m_BigFade.ExportBin( "C:\\Users\\melyg\\Pictures\\bigfade.dat" ) );
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
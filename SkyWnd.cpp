#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "SkyWnd.h"

void CSkyWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CSkyWnd::RenderEnd()
{
	ImGui::End();
}

void CSkyWnd::Render()
{
	if ( ImGui::Button( "Load Palette" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Sky.GetPalette()->LoadBin( *osFilePath ) );
			m_Sky.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Bin" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Sky.LoadBin( *osFilePath ) );
			m_Sky.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_Sky.LoadImg( *osFilePath ) );
			m_Sky.DestroyTexture();
		}
	}

	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Sky.ExportImg( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Sky.ExportBin( Util::FileSystem::FormatPathExportDirectory( GetWindowName() ) ) );
	}

	ImEditor::RenderModifiablePalette( static_cast<void*>(m_Sky.GetPalette()), Assets::Sky::k_uColorStart, Assets::Sky::k_uColorStart + Assets::Sky::k_uNumColors, nullptr );

	if ( m_Sky.GetTexture() == nullptr ) {
		m_Sky.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		auto pSkyTex = m_Sky.GetTexture();
		ImEditor::RenderTexture( m_Sky.GetTexture() );
		ImEditor::ResetRenderState();
	}
}

void CSkyWnd::OnPaletteChange()
{
	m_Sky.DestroyTexture();
}
/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include "imgui.h"

#include "FileDialogNative.h"
#include "AssetsErrHandler.h"
#include "ImEditor.h"
#include "Utils.h"

#include "DispWnd.h"

void CDispWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CDispWnd::RenderEnd()
{
	ImGui::End();
}

void CDispWnd::Render()
{
	if ( ImGui::Button( "Load Image" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::BMP ) ) {
			g_ErrHandler.HandleResult( m_Disp.LoadImg( *osFilePath ) );
			m_Disp.DestroyTexture();
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Disp.ExportImg( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Bin" ) ) {
		g_ErrHandler.HandleResult( m_Disp.ExportBin( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	}

	if ( m_Disp.GetTexture() == nullptr ) {
		m_Disp.CreateTexture( m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( m_pd3dDevice );
		ImEditor::RenderTexture( m_Disp.GetTexture() );
		ImEditor::ResetRenderState();
	}
}

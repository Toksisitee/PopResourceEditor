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
#include "Texture.h"
#include "Utils.h"

#include "LevelWnd.h"

void CLevelWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CLevelWnd::RenderEnd()
{
	ImGui::End();
}

void CLevelWnd::Render()
{
	if ( !m_bFirstPass ) {
		m_bRegenerate = true;
		m_bFirstPass = true;
	}

	if ( ImGui::Button( "Load Bin" ) ) {
		if ( auto osFilePath = FileDialog::OpenFile( FileDialog::Filter::DAT ) ) {
			g_ErrHandler.HandleResult( m_Level.LoadBin( *osFilePath ) );
			m_Level.DestroyTexture();
			m_bRegenerate = true;
		}
	} ImGui::SameLine();
	if ( ImGui::Button( "Export Image" ) ) {
		g_ErrHandler.HandleResult( m_Level.ExportImg( Util::FileSystem::FormatExportPathFromFileName( GetAssetName() ) ) );
	}

	ImGui::InputText( "###Level", (char*)m_sLevel.c_str(), sizeof( m_sLevel ) );
	ImGui::SameLine();
	if ( ImGui::Button( "Regenerate" ) ) {
		auto sFilePath = Util::FileSystem::FormatPath( m_sLevel.c_str() );
		g_ErrHandler.HandleResult( m_Level.LoadBin( sFilePath ) );
		m_bRegenerate = true;
	}

	if ( ImGui::Checkbox( "Calc Light", &m_Level.m_bCalcLight ) ) {
		m_bRegenerate = true;
	}
	ImGui::SameLine();
	ImGui::Checkbox( "512x512", &m_bHighRes );
	ImGui::SameLine();
	ImGui::Checkbox( "Point Filtering", &m_bPointFiltering );
	ImGui::SameLine();
	if ( ImGui::Checkbox( "Gaussian Blur", &m_Level.m_bGaussian ) ) {
		m_bRegenerate = true;
	}
	ImGui::SameLine();
	if ( ImGui::Checkbox( "Render Water", &m_bRenderWater ) ) {
		m_bRegenerate = true;
	}
	static uint16_t p_min = 0;
	static uint16_t p_max = 1024;
	static float fp_min = -1.0f;
	static float fp_max = 1.0f;
	if ( ImEditor::SliderScalar( "Cliff Threshold", &m_uCliffTreshold, &p_min, &p_max ) ) {
		m_bRegenerate = true;
	}
	if ( ImEditor::SliderScalar( "Light Y", &m_fLightDirY ) ) {
		m_bRegenerate = true;
	}
	ImGui::NewLine();

	if ( m_bRegenerate ) {
		m_Level.GeneratePreview( m_uCliffTreshold, m_fLightDirX, m_fLightDirY, m_bRenderWater );
		m_bRegenerate = false;
	}


	if ( m_Level.GetTexture() == nullptr ) {
		m_Level.CreateTexture( m_pd3dDevice );
	}
	else {
		auto pTex = m_Level.GetTexture();
		ImVec2 texSize = ImVec2( 512, 512 );
		if ( !m_bHighRes ) {
			texSize.x = static_cast<float>(pTex->GetWidth());
			texSize.y = static_cast<float>(pTex->GetHeight());
		}
		float fTexelU = 0.5f / texSize.x;
		float fTexelV = 0.5f / texSize.y;
		auto uv0 = ImVec2( fTexelU, fTexelV );
		auto uv1 = ImVec2( 1.0f - fTexelU, 1.0f - fTexelV );
		if ( m_bPointFiltering ) {
			ImEditor::SetPointFiltering( m_pd3dDevice );
		}
		ImEditor::RenderTexture( pTex, texSize, uv0, uv1 );
		if ( m_bPointFiltering ) {
			ImEditor::ResetRenderState();
		}
	}
}

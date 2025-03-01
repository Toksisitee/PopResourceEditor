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

#include "SpriteWnd.h"

void CSpriteWnd::RenderBegin()
{
	ImGui::Begin( m_sWindowName.c_str(), &m_bOpen );
}

void CSpriteWnd::RenderEnd()
{
	ImGui::End();
}

void CSpriteWnd::Render()
{

	static bool inputs_step = true;

#if 0
	if ( m_Sprite.m_Bank.Header.Count == 0 ) {
		auto sFilePath = Util::FileSystem::FormatPath( "HSPR0-0.dat" );
		m_Sprite.SetPalette( &g_Palette );
		g_ErrHandler.HandleResult( g_Sprite.LoadBin( sFilePath ) );
		g_Sprite.CreateTextures( g_Editor.m_pd3dDevice );
	}
	else {
		ImEditor::SetPointFiltering( g_Editor.m_pd3dDevice );

		static uint16_t uSprIndex = 5000;
		ImEditor::InputScalar( "##SpriteEditor", &uSprIndex );
		//ImGui::InputScalar( "##SpriteIndex", ImGuiDataType_U16, &uSprIndex, inputs_step ? &u16_one : NULL, NULL, "%u" );
		auto pTex = g_Sprite.GetTexture( uSprIndex );
		ImVec2 texSize = ImVec2( static_cast<float>(pTex->GetWidth()), static_cast<float>(pTex->GetHeight()) );
		texSize.x = 256;
		texSize.y = 256;

		static bool uv = false;
		ImGui::Checkbox( "UV", &uv );
		if ( uv ) {
			float fTexelU = 0.5f / texSize.x;
			float fTexelV = 0.5f / texSize.y;
			auto uv0 = ImVec2( fTexelU, fTexelV );
			auto uv1 = ImVec2( 1.0f - fTexelU, 1.0f - fTexelV );
			ImEditor::RenderTexture( pTex, texSize, uv0, uv1 );
		}
		else {
			ImEditor::RenderTexture( pTex, texSize );
		}

		ImEditor::ResetRenderState();
	}
#endif

}

void CSpriteWnd::OnPaletteChange()
{

}

#pragma once
#include "imgui.h"

#include "Editor.h"
#include "Texture.h"

namespace ImEditor
{
	extern void SetPointFiltering( LPDIRECT3DDEVICE9 pD3DDevice );
	extern void ResetRenderState();
	extern void RenderTexture( CTexture2D* pTexture, const ImVec2& size, const ImVec2& uv0 = ImVec2( 0, 0 ), const ImVec2& uv1 = ImVec2( 1, 1 ), const ImVec4& tint_col = ImVec4( 1, 1, 1, 1 ), const ImVec4& border_col = ImVec4( 0, 0, 0, 0 ) );
};
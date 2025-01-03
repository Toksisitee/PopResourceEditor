#include "Debug.h"

namespace Debug
{
	CSkyWnd g_SkyWnd( nullptr );
	CGhostWnd g_GhostWnd( nullptr );
	CGhostWnd g_GhostWnd2( nullptr );
	CFadeWnd g_FadeWnd( nullptr );
	CFadeWnd g_FadeWnd2( nullptr );
	CBigFadeWnd g_BigFadeWnd( nullptr );
	CCliffWnd g_CliffWnd( nullptr );
	CCliffWnd g_CliffWnd2( nullptr );
	CAlphaWnd g_AlphaWnd( nullptr );
	CDispWnd g_DispWnd( nullptr );
	CDispWnd g_DispWnd2( nullptr );
	CBlocksWnd g_BlocksWnd( nullptr );
	CLevelWnd g_LevelWnd( nullptr, "levl2017" );
	CLevelWnd g_LevelWnd2( nullptr, "levl2021" );

	void InitializeWindows( LPDIRECT3DDEVICE9 pDevice )
	{
		g_SkyWnd.Initialize( pDevice );
		g_GhostWnd.Initialize( pDevice );
		g_GhostWnd2.Initialize( pDevice );
		g_FadeWnd.Initialize( pDevice );
		g_FadeWnd2.Initialize( pDevice );
		g_BigFadeWnd.Initialize( pDevice );
		g_CliffWnd.Initialize( pDevice );
		g_CliffWnd2.Initialize( pDevice );
		g_AlphaWnd.Initialize( pDevice );
		g_DispWnd.Initialize( pDevice );
		g_DispWnd2.Initialize( pDevice );
		g_BlocksWnd.Initialize( pDevice );
		g_LevelWnd.Initialize( pDevice );
		g_LevelWnd2.Initialize( pDevice );

		g_SkyWnd.SetWindowName( "Sky Window" );
		g_GhostWnd.SetWindowName( "Ghost Window" );
		g_GhostWnd2.SetWindowName( "Ghost Window2" );
		g_FadeWnd.SetWindowName( "Fade Window" );
		g_FadeWnd2.SetWindowName( "Fade Window2" );
		g_BigFadeWnd.SetWindowName( "BigFade Window" );
		g_CliffWnd.SetWindowName( "Cliff Window" );
		g_CliffWnd2.SetWindowName( "Cliff Window2" );
		g_AlphaWnd.SetWindowName( "Alpha Window" );
		g_DispWnd.SetWindowName( "Displacement Window" );
		g_DispWnd2.SetWindowName( "Water Displacement Window" );
		g_BlocksWnd.SetWindowName( "Blocks Window" );
		g_LevelWnd.SetWindowName( "Level Window" );
		g_LevelWnd2.SetWindowName( "Level Window2" );
	}

	void RenderWindows()
	{
		g_SkyWnd.Render();
		g_GhostWnd.Render();
		g_GhostWnd2.Render();
		g_FadeWnd.Render();
		g_FadeWnd2.Render();
		g_BigFadeWnd.Render();
		g_CliffWnd.Render();
		g_CliffWnd2.Render();
		g_AlphaWnd.Render();
		g_DispWnd.Render();
		g_DispWnd2.Render();
		g_BlocksWnd.Render();
		g_LevelWnd.Render();
		g_LevelWnd2.Render();
	}

#if EDITOR_DEBUG_FONTS
	bool CFreeTypeTest::PreNewFrame()
	{
		if ( !WantRebuild )
			return false;

		ImFontAtlas* atlas = ImGui::GetIO().Fonts;
		for ( int n = 0; n < atlas->ConfigData.Size; n++ )
			((ImFontConfig*)&atlas->ConfigData[n])->RasterizerMultiply = RasterizerMultiply;

		// Allow for dynamic selection of the builder. 
		// In real code you are likely to just define IMGUI_ENABLE_FREETYPE and never assign to FontBuilderIO.
#ifdef IMGUI_ENABLE_FREETYPE
		if ( BuildMode == FontBuildMode_FreeType ) {
			atlas->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();
			atlas->FontBuilderFlags = FreeTypeBuilderFlags;
		}
#endif
#ifdef IMGUI_ENABLE_STB_TRUETYPE
		if ( BuildMode == FontBuildMode_Stb ) {
			atlas->FontBuilderIO = ImFontAtlasGetBuilderForStbTruetype();
			atlas->FontBuilderFlags = 0;
		}
#endif
		atlas->Build();
		WantRebuild = false;
		return true;
	}

	void CFreeTypeTest::ShowFontsOptionsWindow()
	{
		ImFontAtlas* atlas = ImGui::GetIO().Fonts;

		ImGui::Begin( "FreeType Options" );
		ImGui::ShowFontSelector( "Fonts" );
		WantRebuild |= ImGui::RadioButton( "FreeType", (int*)&BuildMode, FontBuildMode_FreeType );
		ImGui::SameLine();
		WantRebuild |= ImGui::RadioButton( "Stb (Default)", (int*)&BuildMode, FontBuildMode_Stb );
		WantRebuild |= ImGui::DragInt( "TexGlyphPadding", &atlas->TexGlyphPadding, 0.1f, 1, 16 );
		WantRebuild |= ImGui::DragFloat( "RasterizerMultiply", &RasterizerMultiply, 0.001f, 0.0f, 2.0f );
		ImGui::Separator();

		if ( BuildMode == FontBuildMode_FreeType ) {
#ifndef IMGUI_ENABLE_FREETYPE
			ImGui::TextColored( ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ), "Error: FreeType builder not compiled!" );
#endif
			WantRebuild |= ImGui::CheckboxFlags( "NoHinting", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_NoHinting );
			WantRebuild |= ImGui::CheckboxFlags( "NoAutoHint", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_NoAutoHint );
			WantRebuild |= ImGui::CheckboxFlags( "ForceAutoHint", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_ForceAutoHint );
			WantRebuild |= ImGui::CheckboxFlags( "LightHinting", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_LightHinting );
			WantRebuild |= ImGui::CheckboxFlags( "MonoHinting", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_MonoHinting );
			WantRebuild |= ImGui::CheckboxFlags( "Bold", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_Bold );
			WantRebuild |= ImGui::CheckboxFlags( "Oblique", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_Oblique );
			WantRebuild |= ImGui::CheckboxFlags( "Monochrome", &FreeTypeBuilderFlags, ImGuiFreeTypeBuilderFlags_Monochrome );
		}

		if ( BuildMode == FontBuildMode_Stb ) {
#ifndef IMGUI_ENABLE_STB_TRUETYPE
			ImGui::TextColored( ImVec4( 1.0f, 0.5f, 0.5f, 1.0f ), "Error: stb_truetype builder not compiled!" );
#endif
		}
		ImGui::End();
	}
#endif
}
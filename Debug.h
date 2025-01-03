#pragma once
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"

#include "Editor.h"

#include "SkyWnd.h"
#include "GhostWnd.h"
#include "FadeWnd.h"
#include "BigFadeWnd.h"
#include "CliffWnd.h"
#include "AlphaWnd.h"
#include "DispWnd.h"
#include "BlocksWnd.h"
#include "LevelWnd.h"

namespace Debug
{
	extern CSkyWnd g_SkyWnd;
	extern CGhostWnd g_GhostWnd;
	extern CGhostWnd g_GhostWnd2;
	extern CFadeWnd g_FadeWnd;
	extern CFadeWnd g_FadeWnd2;
	extern CBigFadeWnd g_BigFadeWnd;
	extern CCliffWnd g_CliffWnd;
	extern CCliffWnd g_CliffWnd2;
	extern CAlphaWnd g_AlphaWnd;
	extern CDispWnd g_DispWnd;
	extern CDispWnd g_DispWnd2;
	extern CBlocksWnd g_BlocksWnd;
	extern CLevelWnd g_LevelWnd;
	extern CLevelWnd g_LevelWnd2;

	extern void InitializeWindows( LPDIRECT3DDEVICE9 pDevice );
	extern void RenderWindows();

#if EDITOR_DEBUG_FONTS
	enum FontBuildMode { FontBuildMode_FreeType, FontBuildMode_Stb };
	class CFreeTypeTest
	{
	public:
		bool PreNewFrame();
		void ShowFontsOptionsWindow();
	private:
		FontBuildMode   BuildMode = FontBuildMode_FreeType;
		bool            WantRebuild = true;
		float           RasterizerMultiply = 1.0f;
		unsigned int    FreeTypeBuilderFlags = 0;

	};
#endif
}
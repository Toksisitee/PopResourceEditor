#include <tchar.h>
#include <thread>
#include <d3d9.h>

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#include "imgui_freetype.h"

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "imsink/imsink.h"

#include "ImEditor.h"
#include "Utils.h"
#include "D3DApp.h"
#include "Texture.h"

#include "AssetsErrHandler.h"

#include "Palette.h"
#include "Sprite.h"
#include "Sky.h"

#include "SkyWnd.h"
#include "GhostWnd.h"
#include "FadeWnd.h"
#include "BigFadeWnd.h"
#include "CliffWnd.h"
#include "AlphaWnd.h"
#include "DispWnd.h"
#include "BlocksWnd.h"

#include "App.h"


#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

ImGui_sink_mt_t g_ImGuiSink = ImGui_sink_mt();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

CEditorApp g_Editor;
Assets::CPalette g_Palette;
Assets::CSky g_Sky;
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
CBlocksWnd g_BlocksWnd( nullptr );
Assets::Sprite::CSprite g_Sprite( nullptr );
ImFont* g_ImFonts[eImFont::Max] = { 0 };

#if EDITOR_DEBUG_FONTS
struct FreeTypeTest
{
	enum FontBuildMode { FontBuildMode_FreeType, FontBuildMode_Stb };

	FontBuildMode   BuildMode = FontBuildMode_FreeType;
	bool            WantRebuild = true;
	float           RasterizerMultiply = 1.0f;
	unsigned int    FreeTypeBuilderFlags = 0;

	// Call _BEFORE_ NewFrame()
	bool PreNewFrame()
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

	// Call to draw UI
	void ShowFontsOptionsWindow()
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
};

#endif

void CEditorApp::Run()
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if ( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// TODO: refactor as own function
	std::string sRoot = Util::FileSystem::GetAssetsDirectory();
	char szRootPath[MAX_PATH] = { 0 };
	sprintf_s( szRootPath, sizeof( szRootPath ), "%s\\fonts", sRoot.c_str() );

	auto loadFont = [&szRootPath, &io]( const char* pszFont, const float fSize = 13.0f ) -> ImFont* {
		char szFilepath[128];
		sprintf_s( szFilepath, sizeof( szFilepath ), "%s\\%s", szRootPath, pszFont );
		return io.Fonts->AddFontFromFileTTF( szFilepath, fSize );
	};
	auto loadFontColored = [&szRootPath, &io]( const char* pszFont, const float fSize = 13.0f ) -> ImFont* {
		char szFilepath[128];
		sprintf_s( szFilepath, sizeof( szFilepath ), "%s\\%s", szRootPath, pszFont );
		static ImWchar ranges[] = { 0x1, 0x1FFFF, 0 };
		static ImFontConfig cfg;
		cfg.OversampleH = cfg.OversampleV = 1;
		cfg.MergeMode = true;
		cfg.FontBuilderFlags |= ImGuiFreeTypeBuilderFlags_LoadColor;
		return io.Fonts->AddFontFromFileTTF( szFilepath, fSize * 2, &cfg, ranges );
	};

	g_ImFonts[eImFont::Small] = loadFont( "Roboto-Medium.ttf" );
	g_ImFonts[eImFont::Medium] = loadFont( "Roboto-Medium.ttf", 20.0f );
	g_ImFonts[eImFont::Large] = loadFont( "Roboto-Medium.ttf", 30.0f );

	io.Fonts->AddFontDefault();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init( GetHwnd() );
	ImGui_ImplDX9_Init( GetDevice() );

	g_SkyWnd.Initialize( GetDevice() );
	g_GhostWnd.Initialize( GetDevice() );
	g_GhostWnd2.Initialize( GetDevice() );
	g_FadeWnd.Initialize( GetDevice() );
	g_FadeWnd2.Initialize( GetDevice() );
	g_BigFadeWnd.Initialize( GetDevice() );
	g_CliffWnd.Initialize( GetDevice() );
	g_CliffWnd2.Initialize( GetDevice() );
	g_AlphaWnd.Initialize( GetDevice() );
	g_DispWnd.Initialize( GetDevice() );
	g_BlocksWnd.Initialize( GetDevice() );
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
	g_BlocksWnd.SetWindowName( "Blocks Window" );

	bool bDone = false;
	while ( !bDone ) {
		MSG msg;
		while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
			if ( msg.message == WM_QUIT ) {
				bDone = true;
			}
		}
		if ( bDone ) {
			break;
		}

		HandleResize();

		// Start the Dear ImGui frame
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();

		//g_Editor.m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		//g_Editor.m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
		//g_Editor.m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

#if EDITOR_DEBUG_FONTS
		static FreeTypeTest freetype_test;
		if ( freetype_test.PreNewFrame() ) {
			// REUPLOAD FONT TEXTURE TO GPU
			ImGui_ImplDX9_InvalidateDeviceObjects();
			ImGui_ImplDX9_CreateDeviceObjects();
		}
#endif
		ImGui::NewFrame();
#if EDITOR_DEBUG_FONTS
		freetype_test.ShowFontsOptionsWindow();
#endif

		static bool bShowDemo = true;

		if ( bShowDemo ) {
			ImGui::ShowDemoWindow( &bShowDemo );
		}

		if ( ImGui::Button( "Test" ) ) {
			auto sFilePath = Util::FileSystem::FormatPath( "pal0-0.dat" );
			g_ErrHandler.HandleResult( g_Palette.Load( sFilePath ) );
			sFilePath = Util::FileSystem::FormatPath( "pal.bmp" );
			g_ErrHandler.HandleResult( g_Palette.Export( sFilePath.c_str() ) );

		}
		static float col2[4] = { 0.4f, 0.7f, 0.0f, 0.5f };

		ImGui::NewLine();

		uint8_t uIndex = 0;
		char szColorLabel[8];
		const int k_iColorEditFlags = ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoBorder;
		Color* pPalette = g_Palette.GetColorTable();

		auto getColor = [&pPalette]( uint8_t uIndex ) -> float* {
			float fColors[4];
			ImColor col = ImColor( pPalette[uIndex].R, pPalette[uIndex].G, pPalette[uIndex].B );
			memcpy( &fColors, &col, sizeof( float ) * 4 );
			return fColors;
		};

		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 5.0f, 5.0f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 0.0f, 0.0f ) );
		for ( uint32_t y = 0; y < 16; y++ ) {
			for ( uint32_t x = 0; x < 16; x++ ) {
				sprintf_s( szColorLabel, sizeof( szColorLabel ), "%i", uIndex );
				ImGui::SameLine();
				ImGui::ColorEdit3( szColorLabel, getColor( uIndex ), k_iColorEditFlags );
				uIndex++;
			}
			ImGui::NewLine();
		}
		ImGui::PopStyleVar( 2 );

		{
			static bool bLoaded = false;

#if 1
			if ( !bLoaded ) {
				auto sFilePath = Util::FileSystem::FormatPath( "pal0-b.dat" );
				g_ErrHandler.HandleResult( g_Palette.Load( sFilePath ) );
				sFilePath = Util::FileSystem::FormatPath( "pal.bmp" );
				g_ErrHandler.HandleResult( g_Palette.Export( sFilePath.c_str() ) );

				sFilePath = Util::FileSystem::FormatPath( "sky0-b.dat" );
				if ( g_ErrHandler.HandleResult( g_Sky.Load( sFilePath ) ) != Assets::Result::OK_LOAD ) {

				}
				bLoaded = true;
			}

			{
				ImGui::Begin( "Sky Texture" );
				if ( g_Sky.GetTexture() == nullptr ) {
					g_Sky.CreateTexture( g_Editor.m_pd3dDevice, &g_Palette );
				}
				else {
					ImEditor::SetPointFiltering( g_Editor.m_pd3dDevice );
					auto pSkyTex = g_Sky.GetTexture();
					const ImVec2 texSize = ImVec2( pSkyTex->GetWidth(), pSkyTex->GetHeight() );
					ImEditor::RenderTexture( pSkyTex, texSize );
					ImEditor::ResetRenderState();

				}
				ImGui::End();
			}
#endif

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
				g_BlocksWnd.Render();
			}

			{
				ImGui::Begin( "Sprite Textures" );

				static bool inputs_step = true;

				if ( g_Sprite.Bank.Header.Count == 0 ) {
					auto sFilePath = Util::FileSystem::FormatPath( "HSPR0-0.dat" );
					g_Sprite.SetPalette( &g_Palette );
					g_ErrHandler.HandleResult( g_Sprite.Load( sFilePath ) );
					g_Sprite.CreateTextures( g_Editor.m_pd3dDevice );
				}
				else {
					ImEditor::SetPointFiltering( g_Editor.m_pd3dDevice );

					static uint16_t uSprIndex = 5000;
					ImEditor::InputScalar( "##SpriteEditor", &uSprIndex );
					//ImGui::InputScalar( "##SpriteIndex", ImGuiDataType_U16, &uSprIndex, inputs_step ? &u16_one : NULL, NULL, "%u" );
					auto pTex = g_Sprite.GetTexture( uSprIndex );
					ImVec2 texSize = ImVec2( pTex->GetWidth(), pTex->GetHeight() );
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
				ImGui::End();
			}
		}
		g_ImGuiSink->render();

		// Rendering
		//ImGui::DockSpaceOverViewport();
		ImGui::EndFrame();
		Render3DEnvironment( &io );
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Cleanup3DEnviornment();
	DestroyWindow();
}

// TODO: move
void SetupSpdlog()
{
	std::vector<spdlog::sink_ptr> sinks;
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	sinks.push_back( console_sink );
	sinks.push_back( g_ImGuiSink );

	auto loggers = std::make_shared<spdlog::logger>( "logger", begin( sinks ), end( sinks ) );

	spdlog::register_logger( loggers );
	spdlog::set_default_logger( loggers );
	spdlog::set_pattern( "[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v" );
}

LRESULT CEditorApp::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( ImGui_ImplWin32_WndProcHandler( m_hWnd, uMsg, wParam, lParam ) )
		return true;

	switch ( uMsg ) {
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
				return 0;
			WndResized( (UINT)LOWORD( lParam ), (UINT)HIWORD( lParam ) );
			return 0;
		case WM_SYSCOMMAND:
			if ( (wParam & 0xfff0) == SC_KEYMENU ) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			// TODO: cleanup
			::PostQuitMessage( 0 );
			return 0;
		case WM_DPICHANGED:
			if ( ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports ) {
				//const int dpi = HIWORD(wParam);
				//printf("WM_DPICHANGED to %d (%.0f%%)\n", dpi, (float)dpi / 96.0f * 100.0f);
				const RECT* suggested_rect = (RECT*)lParam;
				::SetWindowPos( m_hWnd, nullptr,
					suggested_rect->left, suggested_rect->top,
					suggested_rect->right - suggested_rect->left,
					suggested_rect->bottom - suggested_rect->top,
					SWP_NOZORDER | SWP_NOACTIVATE );
			}
			break;
	}
	return ::DefWindowProcW( m_hWnd, uMsg, wParam, lParam );
}

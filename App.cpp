/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

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

#include "IniFile.h"
#include "Debug.h"
#include "ImEditor.h"
#include "Utils.h"
#include "D3DApp.h"
#include "Texture.h"

#include "AssetsErrHandler.h"
#include "AssetPicker.h"
#include "FileDialogNative.h"

#include "Palette.h"
#include "Sprite.h"
#include "Sky.h"

#include "WindowManager.h"
#include "SkyWnd.h"
#include "GhostWnd.h"
#include "FadeWnd.h"
#include "BigFadeWnd.h"
#include "CliffWnd.h"
#include "AlphaWnd.h"
#include "DispWnd.h"
#include "BlocksWnd.h"
#include "LevelWnd.h"
#include "TextureSetWnd.h"

#include "App.h"


#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

ImGui_sink_mt_t g_ImGuiSink = ImGui_sink_mt();
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

CEditorApp g_Editor;
Assets::CPalette g_Palette;
Assets::CSky g_Sky;
extern CIniFile g_IniFile;

ImFont* g_ImFonts[eImFont::Max] = { 0 };

void RenderSetupScreen()
{
	static char* pszBuffer = nullptr;
	static char szPath[500] = "";
	ImGuiViewport* pViewport = ImGui::GetMainViewport();

	ImVec2 vCenterPos = ImVec2(
		pViewport->Pos.x + (pViewport->Size.x * 0.5f),
		pViewport->Pos.y + (pViewport->Size.y * 0.5f)
	);

	ImVec2 vWindowSize = ImVec2( 300, 200 );
	ImVec2 vWindowPos = ImVec2(
		vCenterPos.x - (vWindowSize.x * 0.5f),
		vCenterPos.y - (vWindowSize.y * 0.5f)
	);

	ImGui::SetNextWindowPos( vWindowPos, ImGuiCond_Always );
	ImGui::SetNextWindowSize( vWindowSize );

	ImGui::Begin( "Setup", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar );

	if ( !g_Editor.GetPopDirectory().empty() ) {
		sprintf_s( szPath, sizeof( szPath ), "%s", g_Editor.GetPopDirectory().c_str() );
	}

	ImGui::TextUnformatted( "Populous Directory: " );
	ImGui::InputText( "##path", szPath, sizeof( szPath ), ImGuiInputTextFlags_ReadOnly );
	ImGui::SameLine();
	if ( ImGui::Button( "Browse##path" ) ) {
		if ( auto osFolderPath = FileDialog::OpenFolder() ) {
			std::string sFilePathCopy = *osFolderPath;
			g_Editor.SetPopDirectory( sFilePathCopy );
			g_IniFile.SetString( EIniSetting::PopulousDirectory, sFilePathCopy );
			AssetPicker::GetAllFiles( sFilePathCopy );
			sprintf_s( szPath, sizeof( szPath ), "%s", sFilePathCopy.c_str() );
		}
	}

	ImGui::End();
}

void DockspaceBuild( ImGuiID dockspaceID, bool& bDockspaceBuilt )
{
	bDockspaceBuilt = true;
	ImGuiViewport* pViewport = ImGui::GetMainViewport();

	ImGui::DockBuilderRemoveNode( dockspaceID );
	ImGui::DockBuilderAddNode( dockspaceID, ImGuiDockNodeFlags_DockSpace );
	ImGui::DockBuilderSetNodeSize( dockspaceID, pViewport->Size );

	ImGuiID dock_main_id = dockspaceID;
	ImGuiID dock_id_top = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Up, 0.70f, NULL, &dock_main_id );
	ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Down, 0.30f, NULL, &dock_main_id );

	ImGuiDockNode* node = ImGui::DockBuilderGetNode( dock_main_id );
	node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
	node = ImGui::DockBuilderGetNode( dock_id_bottom );
	node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;

	ImGui::DockBuilderDockWindow( "Root", dock_main_id );
	ImGui::DockBuilderDockWindow( "Console", dock_id_bottom );

	ImGui::DockBuilderFinish( dockspaceID );
}

void DockspaceDraw()
{
	static ImGuiID dockspaceID = 0;
	static bool bDockSpaceBuilt = false;
	ImGuiWindowFlags windowFlags = 0;
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoResize;

	dockspaceID = ImGui::GetID( "PopResourceEditorDockspace" );
	ImGuiViewport* pViewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos( pViewport->Pos );
	ImGui::SetNextWindowSize( pViewport->Size );
	ImGui::SetNextWindowViewport( pViewport->ID );

	if ( !bDockSpaceBuilt ) {
		DockspaceBuild( dockspaceID, bDockSpaceBuilt );
	}

	ImGui::Begin( "Main", nullptr, windowFlags );
	ImGui::DockSpace( dockspaceID, ImVec2( 0, 0 ), ImGuiDockNodeFlags_None );
	ImGui::End();
}


void CEditorApp::Run()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
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

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4( 0.85f, 0.80f, 0.70f, 1.00f );
	colors[ImGuiCol_TextDisabled] = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
	colors[ImGuiCol_WindowBg] = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
	colors[ImGuiCol_ChildBg] = ImVec4( 0.11f, 0.11f, 0.11f, 1.00f );
	colors[ImGuiCol_PopupBg] = ImVec4( 0.08f, 0.08f, 0.08f, 0.94f );
	colors[ImGuiCol_Border] = ImVec4( 0.15f, 0.15f, 0.13f, 1.00f );
	colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ImGuiCol_FrameBg] = ImVec4( 0.13f, 0.13f, 0.13f, 1.00f );
	colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.19f, 0.16f, 0.13f, 1.00f );
	colors[ImGuiCol_FrameBgActive] = ImVec4( 0.30f, 0.25f, 0.21f, 1.00f );
	colors[ImGuiCol_TitleBg] = ImVec4( 0.17f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_TitleBgActive] = ImVec4( 0.22f, 0.24f, 0.25f, 1.00f );
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 0.00f, 0.00f, 0.00f, 0.51f );
	colors[ImGuiCol_MenuBarBg] = ImVec4( 0.17f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.07f, 0.07f, 0.07f, 1.00f );
	colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.17f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.24f, 0.26f, 0.27f, 1.00f );
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.35f, 0.38f, 0.40f, 1.00f );
	colors[ImGuiCol_CheckMark] = ImVec4( 0.87f, 0.81f, 0.72f, 1.00f );
	colors[ImGuiCol_SliderGrab] = ImVec4( 0.48f, 0.45f, 0.39f, 1.00f );
	colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.38f, 0.35f, 0.31f, 1.00f );
	colors[ImGuiCol_Button] = ImVec4( 0.14f, 0.16f, 0.16f, 1.00f );
	colors[ImGuiCol_ButtonHovered] = ImVec4( 0.18f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_ButtonActive] = ImVec4( 0.24f, 0.27f, 0.28f, 1.00f );
	colors[ImGuiCol_Header] = ImVec4( 0.14f, 0.16f, 0.16f, 1.00f );
	colors[ImGuiCol_HeaderHovered] = ImVec4( 0.18f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_HeaderActive] = ImVec4( 0.24f, 0.27f, 0.28f, 1.00f );
	colors[ImGuiCol_Separator] = ImVec4( 0.17f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_SeparatorHovered] = ImVec4( 0.17f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_SeparatorActive] = ImVec4( 0.17f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_ResizeGrip] = ImVec4( 0.26f, 0.29f, 0.30f, 1.00f );
	colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.30f, 0.33f, 0.35f, 1.00f );
	colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.35f, 0.38f, 0.40f, 1.00f );
	colors[ImGuiCol_Tab] = ImVec4( 0.14f, 0.16f, 0.16f, 1.00f );
	colors[ImGuiCol_TabHovered] = ImVec4( 0.18f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_TabActive] = ImVec4( 0.28f, 0.28f, 0.24f, 1.00f );
	colors[ImGuiCol_TabUnfocused] = ImVec4( 0.07f, 0.10f, 0.15f, 0.97f );
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4( 0.16f, 0.18f, 0.19f, 0.00f );
	colors[ImGuiCol_DockingPreview] = ImVec4( 0.12f, 0.14f, 0.16f, 0.70f );
	colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
	colors[ImGuiCol_PlotLines] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
	colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
	colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
	colors[ImGuiCol_TableHeaderBg] = ImVec4( 0.19f, 0.19f, 0.20f, 1.00f );
	colors[ImGuiCol_TableBorderStrong] = ImVec4( 0.31f, 0.31f, 0.35f, 1.00f );
	colors[ImGuiCol_TableBorderLight] = ImVec4( 0.23f, 0.23f, 0.25f, 1.00f );
	colors[ImGuiCol_TableRowBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	colors[ImGuiCol_TableRowBgAlt] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
	colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.60f, 0.60f, 0.60f, 0.35f );
	colors[ImGuiCol_DragDropTarget] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
	colors[ImGuiCol_NavHighlight] = ImVec4( 0.16f, 0.16f, 0.14f, 1.00f );
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );

	style.WindowPadding.x = 10.0f;
	style.WindowPadding.y = 10.0f;
	style.FramePadding.x = 8.0f;
	style.FramePadding.y = 7.0f;
	style.ItemSpacing.x = 10.0f;
	style.ItemSpacing.y = 4.0f;
	style.ItemInnerSpacing.x = 12.0f;
	style.ScrollbarSize = 13.0f;
	style.GrabMinSize = 6.0f;
	style.TabBarBorderSize = 1.0f;
	style.FrameRounding = 6.0f;
	style.PopupRounding = 6.0f;
	style.ChildRounding = 12.0f;
	style.ScrollbarRounding = 7.0f;
	style.GrabRounding = 4.0f;
	style.TabRounding = 10.0f;
	style.CellPadding.y = 4.0f;
	style.WindowTitleAlign.x = 0.50f;
	style.WindowTitleAlign.y = 0.50f;
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.SeparatorTextBorderSize = 1.0f;
	style.SeparatorTextPadding.x = 15.0f;

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

	//Debug::InitializeWindows( GetDevice() );

	bool bPopDirectorySet = false;
	g_IniFile.Initialize();

	std::string sPopDir = g_IniFile.GetString( EIniSetting::PopulousDirectory, "" );
	if ( !sPopDir.empty() ) {
		SetPopDirectory( sPopDir );
		AssetPicker::GetAllFiles( sPopDir );
	}

	auto sExportDir = g_IniFile.GetString( EIniSetting::ExportDirectory, Util::FileSystem::GetApplicationDirectory() + "\\Export" );
	g_Editor.SetExportDirectory( sExportDir );
	g_IniFile.SetString( EIniSetting::ExportDirectory, sExportDir );
	Util::FileSystem::CreateFolder( sExportDir );

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

#if EDITOR_DEBUG_FONTS
		static Debug::CFreeTypeTest freetype_test;
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

		static bool bShowDemo = false;

		if ( !IsPopDirectorySet() ) {
			RenderSetupScreen();
		}
		else {
			if ( bShowDemo ) {
				ImGui::ShowDemoWindow( &bShowDemo );
			}

			//Debug::RenderWindows();

			DockspaceDraw();
			g_WndMngr.Render();
			ImGui::Begin( "Root", nullptr, ImGuiWindowFlags_MenuBar );
			{
				{
					static bool bAboutWnd = false;

					ImVec2 vecRootPos = ImGui::GetWindowPos();
					ImVec2 vecRootSize = ImGui::GetWindowSize();

					if ( ImGui::BeginMenuBar() ) {
						if ( ImGui::BeginMenu( "File" ) ) {
							if ( ImGui::MenuItem( "Open Export Directory" ) ) {
								Util::OpenDirectory( Util::FileSystem::GetExportDirectory() );
							}
							ImGui::EndMenu();
						}
						if ( ImGui::MenuItem( "About" ) ) {
							bAboutWnd = true;
						}
						if ( ImGui::MenuItem( "Donate" ) ) {
							Util::OpenBrowser( "https://ko-fi.com/toksisitee" );
						}
						ImGui::EndMenuBar();
					}

					if ( bAboutWnd ) {
						bAboutWnd = false;
						ImGui::OpenPopup( "About" );
					}

					ImVec2 vecPickerCenter = ImVec2( vecRootPos.x + vecRootSize.x * 0.5f, vecRootPos.y + vecRootSize.y * 0.5f );
					ImVec2 vecAboutWndSize = ImVec2( vecRootSize.x * 0.65f, vecRootSize.y * 0.85f );
					ImGui::SetNextWindowSize( vecAboutWndSize );
					ImGui::SetNextWindowPos( ImVec2( vecPickerCenter.x - vecAboutWndSize.x * 0.5f,
											 vecPickerCenter.y - vecAboutWndSize.y * 0.5f ),
											 ImGuiCond_Always );

					if ( ImGui::BeginPopupModal( "About", NULL, ImGuiWindowFlags_AlwaysAutoResize ) ) {
						const char* pszUrl = "https://github.com/Toksisitee/PopResourceEditor";
						ImGui::TextWrapped( "Open-source asset editor and manager written in C++ for Bullfrog's Populous: The Beginning, designed to preview, modify, and generate the game's assets." );
						ImGui::NewLine();
						ImGui::TextWrapped( "Project: %S", EDITOR_NAME );
						ImGui::TextWrapped( "Version: %s", EDITOR_VERSION );
						ImGui::TextWrapped( "Compiled: %s (%s)", EDITOR_DATE, EDITOR_CONFIG );
						ImGui::Text( "Repository: " ); 
						ImGui::SameLine();
						ImGui::TextColored( ImVec4( 0.0f, 0.5f, 1.0f, 1.0f ), pszUrl );

						if ( ImGui::IsItemHovered() ) {
							ImGui::SetMouseCursor( ImGuiMouseCursor_Hand );
						}

						if ( ImGui::IsItemClicked() ) {
							Util::OpenBrowser( pszUrl );
						}

						ImGui::NewLine();

						const char* pszLicense =
							"Copyright (c) 2024-2025 Toksisitee\n"
							"Permission is hereby granted, free of charge, to any person obtaining a copy"
							"of this software and associated documentation files (the \"Software\"), to deal"
							"in the Software without restriction, including without limitation the rights"
							"to use, copy, modify, merge, publish, distribute, sublicense, and/or sell"
							"copies of the Software, and to permit persons to whom the Software is"
							"furnished to do so, subject to the following conditions:"
							"The above copyright notice and this permission notice shall be included in all"
							"copies or substantial portions of the Software."
							"THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR"
							"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,"
							"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE"
							"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER"
							"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,"
							"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE";
						ImGui::TextWrapped( pszLicense );

						if ( ImGui::Button( "Close" ) ) {
							bAboutWnd = false;
							ImGui::CloseCurrentPopup();
						}
						ImGui::EndPopup();
					}

					if ( ImGui::Button( "Create Texture Set" ) ) {
						auto pWnd = g_WndMngr.AddWindow<CTextureSetWnd>( GetDevice(), "Texture Set" );
					}
				}
				AssetPicker::Render();
				ImGui::End();

			}
			g_ImGuiSink->Render();
		}
		// Rendering
		//ImGui::DockSpaceOverViewport();
		ImGui::EndFrame();
		Render3DEnvironment( (void*)&io );
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


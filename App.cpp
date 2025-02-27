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

Assets::Sprite::CSprite g_Sprite( nullptr );
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
	ImGuiID dock_id_top = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Up, 0.35f, NULL, &dock_main_id );
	ImGuiID dock_id_left = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Left, 0.18f, NULL, &dock_main_id );
	ImGuiID dock_id_right = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Right, 0.25f, NULL, &dock_main_id );
	ImGuiID dock_id_rightBottom = ImGui::DockBuilderSplitNode( dock_id_right, ImGuiDir_Down, 0.20f, NULL, &dock_id_right );
	float fChatSplit = 0.13f;
#if _DEBUG
	fChatSplit = 0.55f;
#endif
	ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode( dock_main_id, ImGuiDir_Down, fChatSplit, NULL, &dock_main_id );
	ImGuiID dock_id_bottomConsole = ImGui::DockBuilderSplitNode( dock_id_bottom, ImGuiDir_Down, 0.45f, NULL, &dock_id_bottom );

	ImGuiDockNode* node = ImGui::DockBuilderGetNode( dock_id_top );
	node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
	node->LocalFlags |= ImGuiDockNodeFlags_NoResizeY;
	node = ImGui::DockBuilderGetNode( dock_id_bottom );
	node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
	node = ImGui::DockBuilderGetNode( dock_id_bottomConsole );
	node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
	node = ImGui::DockBuilderGetNode( dock_main_id );
	node->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
#if !_DEBUG
	node->LocalFlags |= ImGuiDockNodeFlags_NoResizeY;
#endif

	ImGui::DockBuilderDockWindow( "Asset Picker", dock_main_id );
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

	ImGui::Begin( "PopResourceEditor", nullptr, windowFlags );
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

		static bool bShowDemo = true;

		if ( !IsPopDirectorySet() ) {
			RenderSetupScreen();
		}
		else {
			if ( bShowDemo ) {
				ImGui::ShowDemoWindow( &bShowDemo );
			}

#if 0
			if ( ImGui::Button( "Test" ) ) {
				auto sFilePath = Util::FileSystem::FormatPath( "pal0-0.dat" );
				g_ErrHandler.HandleResult( g_Palette.LoadBin( sFilePath ) );
				sFilePath = Util::FileSystem::FormatPath( "pal.bmp" );
				g_ErrHandler.HandleResult( g_Palette.ExportImg( sFilePath.c_str() ) );
			}
			ImGui::NewLine();
			{
				static bool bLoaded = false;

#if 1
				if ( !bLoaded ) {
					auto sFilePath = Util::FileSystem::FormatPath( "pal0-b.dat" );
					g_ErrHandler.HandleResult( g_Palette.LoadBin( sFilePath ) );
					sFilePath = Util::FileSystem::FormatPath( "pal.bmp" );
					g_ErrHandler.HandleResult( g_Palette.ExportImg( sFilePath.c_str() ) );

					sFilePath = Util::FileSystem::FormatPath( "sky0-b.dat" );
					if ( g_ErrHandler.HandleResult( g_Sky.LoadBin( sFilePath ) ) != Assets::Result::OK_LOAD ) {

					}
					bLoaded = true;
				}
#endif


#endif

				{
					//Debug::RenderWindows();
				}

				{
					DockspaceDraw();
					if ( ImGui::Button( "Texture Set" ) ) {
						auto pWnd = g_WndMngr.AddWindow<CTextureSetWnd>( GetDevice(), "Texture Set" );
					}

					g_WndMngr.Render();
					AssetPicker::Render();
				}

#if 0
				{
					ImGui::Begin( "Sprite Textures" );

					static bool inputs_step = true;

					if ( g_Sprite.m_Bank.Header.Count == 0 ) {
						auto sFilePath = Util::FileSystem::FormatPath( "HSPR0-0.dat" );
						g_Sprite.SetPalette( &g_Palette );
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
					ImGui::End();
				}
			}
#endif
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


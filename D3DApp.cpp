/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <dwmapi.h>
#include "resource.h"

#include "imgui/imgui.h"
#include "imgui_impl_dx9.h"

#include "Editor.h"
#include "D3DApp.h"

#define MAKE_D3DCOLOR_RGBA(v4) D3DCOLOR_RGBA(	\
    (int)((v4).x * (v4).w * 255.0f),			\
    (int)((v4).y * (v4).w * 255.0f),			\
    (int)((v4).z * (v4).w * 255.0f),			\
    (int)((v4).w * 255.0f)) 					\


typedef LONG( WINAPI* RtlGetVersionPtr )(PRTL_OSVERSIONINFOW);
bool IsWindows11OrGreater()
{
	HMODULE hDll = GetModuleHandleW( L"ntdll.dll" );
	if ( hDll ) {
		RtlGetVersionPtr fnRtlGetVersion = (RtlGetVersionPtr)GetProcAddress( hDll, "RtlGetVersion" );
		if ( fnRtlGetVersion ) {
			RTL_OSVERSIONINFOW osvi = { sizeof( osvi ) };
			if ( fnRtlGetVersion( &osvi ) == 0 ) {
				return (osvi.dwMajorVersion == 10 && osvi.dwBuildNumber >= 10240);
			}
		}
	}
	return false;
}

CD3DApp::CD3DApp()
{
	m_hWnd = nullptr;
	m_hInstance = 0;
	m_lpszClassName = nullptr;

	m_pD3D = nullptr;
	m_pd3dDevice = nullptr;
	m_uResizeWidth = m_uResizeHeight = 0;
	m_d3dpp = {};
}

CD3DApp::~CD3DApp()
{
	if ( m_lpszClassName ) {
		free( (LPWSTR)m_lpszClassName );
		m_lpszClassName = nullptr;
	}
}

HRESULT CD3DApp::CreateDeviceD3D( HWND hWnd )
{
	if ( (m_pD3D = Direct3DCreate9( D3D_SDK_VERSION )) == nullptr )
		return E_FAIL;

	// Create the D3DDevice
	ZeroMemory( &m_d3dpp, sizeof( m_d3dpp ) );
	m_d3dpp.Windowed = TRUE;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
	//g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
	if ( m_pD3D->CreateDevice( D3DADAPTER_DEFAULT,
		 D3DDEVTYPE_HAL, hWnd,
		 D3DCREATE_HARDWARE_VERTEXPROCESSING,
		 &m_d3dpp, &m_pd3dDevice ) < 0 )
		return E_FAIL;

	return S_OK;
}


HRESULT CD3DApp::Initialize3DEnvironment()
{
	HRESULT hr;

	// Initialize Direct3D
	if ( FAILED( hr = !CreateDeviceD3D( m_hWnd ) ) ) {
		Cleanup3DEnviornment();
	}

	return hr;
}

HRESULT CD3DApp::Create( HINSTANCE hInstance )
{
	HRESULT hr;

	m_hInstance = hInstance;

	// Create application window
	//ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX wc = { 0 };
	wc.hInstance = hInstance;
	wc.lpszClassName = EDITOR_NAME;
	wc.lpfnWndProc = WndProcCallback;
	wc.style = CS_BYTEALIGNWINDOW;
	wc.cbSize = sizeof( WNDCLASSEX );
	wc.hIcon = ::LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
	wc.hIconSm = ::LoadIcon( hInstance, MAKEINTRESOURCE( IDI_ICON1 ) );
	wc.hCursor = ::LoadCursor( NULL, IDC_ARROW );
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 4;
	::RegisterClassEx( &wc );


	if ( m_hWnd == NULL ) {
		m_hWnd = ::CreateWindowEx( 0, wc.lpszClassName, EDITOR_NAME, WS_OVERLAPPEDWINDOW, 100, 100, 800, 600, nullptr, 0, hInstance, this );
		size_t length = wcslen( wc.lpszClassName ) + 1;
		m_lpszClassName = (LPCWSTR)malloc( length * sizeof( WCHAR ) );
	}

	if ( FAILED( hr = Initialize3DEnvironment() ) ) {
		::UnregisterClassW( wc.lpszClassName, wc.hInstance );
		return hr;
	}

	if ( IsWindows11OrGreater() ) {
		BOOL nDarkMode = 1;
		HRESULT hr = ::DwmSetWindowAttribute( m_hWnd, 20 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &nDarkMode, sizeof( nDarkMode ) );
		if ( FAILED( hr ) ) {
			hr = ::DwmSetWindowAttribute( m_hWnd, 19 /* DWMWA_USE_IMMERSIVE_DARK_MODE */, &nDarkMode, sizeof( nDarkMode ) );
		}
	}

	::ShowWindow( m_hWnd, SW_SHOWDEFAULT );
	::UpdateWindow( m_hWnd );

	return hr;
}

HRESULT CD3DApp::Render3DEnvironment( void* pIo )
{
	ImGuiIO* io = (ImGuiIO*)pIo;
	HRESULT result;
	ImVec4 clear_color = ImVec4( 0.45f, 0.55f, 0.60f, 1.00f );
	D3DCOLOR clear_col_dx = MAKE_D3DCOLOR_RGBA( clear_color );

	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	m_pd3dDevice->Clear( 0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0 );
	if ( m_pd3dDevice->BeginScene() >= 0 ) {
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData( ImGui::GetDrawData() );
		m_pd3dDevice->EndScene();
	}

	// Update and Render additional Platform Windows
	if ( io && io->ConfigFlags & ImGuiConfigFlags_ViewportsEnable ) {
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}

	result = m_pd3dDevice->Present( nullptr, nullptr, nullptr, nullptr );

	// Handle loss of D3D9 device
	if ( result == D3DERR_DEVICELOST && m_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET )
		Reset3DEnvironment();

	return result;
}

void CD3DApp::Run()
{
	bool done = false;
	while ( !done ) {
		MSG msg;
		while ( ::PeekMessage( &msg, nullptr, 0U, 0U, PM_REMOVE ) ) {
			::TranslateMessage( &msg );
			::DispatchMessage( &msg );
			if ( msg.message == WM_QUIT ) {
				done = true;
			}
		}
		if ( done ) {
			break;
		}

		Render3DEnvironment( nullptr );
	}

	Cleanup3DEnviornment();
	DestroyWindow();
}

void CD3DApp::DestroyWindow()
{
	::DestroyWindow( m_hWnd );
	if ( m_lpszClassName != nullptr ) {
		::UnregisterClassW( m_lpszClassName, m_hInstance );
	}
}

void CD3DApp::WndResized( UINT w, UINT h )
{
	m_uResizeWidth = w;
	m_uResizeHeight = h;
}

void CD3DApp::Cleanup3DEnviornment()
{
	if ( m_pd3dDevice ) {
		m_pd3dDevice->Release();
		m_pd3dDevice = nullptr;
	}
	if ( m_pD3D ) {
		m_pD3D->Release();
		m_pD3D = nullptr;
	}
}

void CD3DApp::Reset3DEnvironment()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = m_pd3dDevice->Reset( &m_d3dpp );
	if ( hr == D3DERR_INVALIDCALL )
		IM_ASSERT( 0 );
	ImGui_ImplDX9_CreateDeviceObjects();
}

void CD3DApp::HandleResize()
{
	// Handle window resize (we don't resize directly in the WM_SIZE handler)
	if ( m_uResizeWidth != 0 && m_uResizeHeight != 0 ) {
		m_d3dpp.BackBufferWidth = m_uResizeWidth;
		m_d3dpp.BackBufferHeight = m_uResizeHeight;
		m_uResizeWidth = m_uResizeHeight = 0;
		Reset3DEnvironment();
	}
}

LRESULT CALLBACK CD3DApp::WndProcCallback( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CD3DApp* wnd;

	if ( uMsg == WM_CREATE ) {
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		wnd = static_cast<CD3DApp*>(cs->lpCreateParams);
		wnd->m_hWnd = hwnd;
		::SetWindowLongPtr( hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd) );
	}
	else
		wnd = reinterpret_cast<CD3DApp*>(::GetWindowLongPtr( hwnd, GWLP_USERDATA ));

	if ( wnd ) {
		return wnd->WndProc( uMsg, wParam, lParam );
	}

	return ::DefWindowProc( hwnd, uMsg, wParam, lParam );
}

LRESULT CD3DApp::WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return DefWndProc( uMsg, wParam, lParam );
}

LRESULT CD3DApp::DefWndProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return ::DefWindowProc( m_hWnd, uMsg, wParam, lParam );
}


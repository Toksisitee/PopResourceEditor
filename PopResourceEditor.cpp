#include <Windows.h>
#include <d3d9.h>

#include "imgui/imgui.h"

#include "D3DApp.h"
#include "App.h"

extern void SetupSpdlog();

int WINAPI WinMain( _In_ HINSTANCE hInstance,
	_In_ HINSTANCE hPrevInstance,
	_In_ LPSTR     lpCmdLine,
	_In_ int       nCmdShow )
{

	SetupSpdlog();
	g_Editor.Create( hInstance );
	g_Editor.Run();
	return 0;
}

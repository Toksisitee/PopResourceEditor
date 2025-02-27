/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

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


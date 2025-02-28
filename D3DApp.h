/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <windows.h>
#include <d3d9.h> 

class CD3DApp
{
public:
	CD3DApp();
	~CD3DApp();

	virtual void			Run();
	virtual HRESULT			Create(HINSTANCE hInstance);
	virtual void			DestroyWindow();

	void					HandleResize();
	void					Cleanup3DEnviornment();
	[[nodiscard]] HWND		GetHwnd() { return m_hWnd; }
	[[nodiscard]] LPDIRECT3DDEVICE9	GetDevice() { return m_pd3dDevice; }

	UINT					GetWidth() { return m_d3dpp.BackBufferWidth; }
	UINT					GetHeight() { return m_d3dpp.BackBufferHeight; }

protected:
	virtual LRESULT			WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT			DefWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT					Render3DEnvironment(void* pIo);
	void					Reset3DEnvironment();
	HRESULT					Initialize3DEnvironment();
	HRESULT					CreateDeviceD3D(HWND hWnd);

	void					WndResized(UINT w, UINT h);

	HWND					m_hWnd;
	HINSTANCE				m_hInstance;
	LPCWSTR					m_lpszClassName;

	LPDIRECT3D9             m_pD3D;
	LPDIRECT3DDEVICE9       m_pd3dDevice;
	UINT					m_uResizeWidth;
	UINT					m_uResizeHeight;
	D3DPRESENT_PARAMETERS   m_d3dpp;

private:
	static LRESULT CALLBACK WndProcCallback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};



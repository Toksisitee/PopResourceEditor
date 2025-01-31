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


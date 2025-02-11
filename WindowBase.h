#pragma once
#include <string>

typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;

class CWindowBase {
public:
	CWindowBase() : CWindowBase( nullptr ) {}
	CWindowBase( LPDIRECT3DDEVICE9 pd3dDevice ) : m_pd3dDevice( pd3dDevice ), m_bFirstPass( false ) {}
	virtual ~CWindowBase() = default;

	virtual void Render() = 0;
	virtual void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;
	virtual void Cleanup() = 0;

	void SetWindowName( std::string sWindowName ) { m_sWindowName = sWindowName; }
	[[nodiscard]] virtual const std::string& GetWindowName() const { return m_sWindowName; }
	[[nodiscard]] bool IsOpen() const { return m_bOpen; }

protected:
	bool                m_bFirstPass = false;
	bool                m_bOpen = true;
	std::string         m_sWindowName;
	LPDIRECT3DDEVICE9   m_pd3dDevice;
};

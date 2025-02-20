#pragma once
#include <string>
#include <functional>

typedef struct IDirect3DDevice9* LPDIRECT3DDEVICE9, * PDIRECT3DDEVICE9;

class CWindowBase {
public:
	CWindowBase( const std::string& sName ) : CWindowBase( nullptr, sName ) {}
	CWindowBase( LPDIRECT3DDEVICE9 pd3dDevice, const std::string& sName ) :
		m_pd3dDevice( pd3dDevice ), m_sWindowName( sName ), m_bFirstPass( false )
	{
	}
	virtual ~CWindowBase() = default;

	virtual void Render() = 0;
	virtual void Initialize( LPDIRECT3DDEVICE9 pd3dDevice ) = 0;
	virtual void Cleanup() = 0;
	void ProcessTask()
	{
		if ( m_PendingTask ) {
			m_PendingTask();
			m_PendingTask = nullptr;
		}
	}

	void SetWindowName( std::string sWindowName ) { m_sWindowName = sWindowName; }
	[[nodiscard]] virtual const std::string& GetWindowName() const { return m_sWindowName; }
	[[nodiscard]] bool IsOpen() const { return m_bOpen; }

protected:
	bool                m_bFirstPass = false;
	bool                m_bOpen = true;
	std::string         m_sWindowName;
	LPDIRECT3DDEVICE9   m_pd3dDevice;
	std::function<void()> m_PendingTask;
};

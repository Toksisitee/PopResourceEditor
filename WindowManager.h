#include <vector>
#include <memory>
#include <iostream>

#include "WindowBase.h"

class CWindowManager {
public:
	CWindowManager() = default;
	~CWindowManager()
	{
		DestroyAll();
	}

	template<typename T, typename... Args>
	T* AddWindow( Args&&... args )
	{
		static_assert(std::is_base_of<CWindowBase, T>::value, "T must derive from CWindowBase");
		auto& pWnd = m_Windows.emplace_back( std::make_unique<T>( std::forward<Args>( args )... ) );
		return static_cast<T*>(pWnd.get());
	}

	void Render()
	{
		m_Windows.erase(std::remove_if( m_Windows.begin(), m_Windows.end(),
			[]( const std::unique_ptr<CWindowBase>& pWnd ) {
			return !pWnd->IsOpen();
			} ), m_Windows.end()
		);

		for ( auto& pWnd : m_Windows ) {
			pWnd->Render();
		}
	}

	void DestroyAll()
	{
		for ( auto& pWnd : m_Windows ) {
			pWnd->Cleanup();
		}
		m_Windows.clear();
	}

	CWindowBase* GetWindow( const std::string& sWndName )
	{
		for ( auto& pWnd : m_Windows ) {
			if ( pWnd->GetWindowName() == sWndName ) {
				return pWnd.get();
			}
		}
		return nullptr;
	}

private:
	std::vector<std::unique_ptr<CWindowBase>> m_Windows;
};

extern CWindowManager g_WndMngr;
/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <vector>
#include <memory>
#include <iostream>
#include <tuple>

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
		auto&& tuple = std::forward_as_tuple( args... );
		if ( GetWindow( std::get<1>( tuple ) ) == nullptr ) {
			auto& pWnd = m_Windows.emplace_back( std::make_unique<T>( std::forward<Args>( args )... ) );
			return static_cast<T*>(pWnd.get());
		}
		return nullptr;
	}

	void Render()
	{
		m_Windows.erase( std::remove_if( m_Windows.begin(), m_Windows.end(),
						 []( const std::unique_ptr<CWindowBase>& pWnd ) {
			if ( !pWnd->IsOpen() ) pWnd->Cleanup();
			return !pWnd->IsOpen();
		} ), m_Windows.end()
			);

		for ( auto& pWnd : m_Windows ) {
			pWnd->RenderBegin();
			pWnd->Render();
			pWnd->RenderEnd();
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

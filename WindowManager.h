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
        auto& wnd = m_Windows.emplace_back( std::make_unique<T>( std::forward<Args>( args )... ) );
        return static_cast<T*>(wnd.get()); // Return a raw pointer to the added window
    }

    void Render()
    {
        for ( auto& window : m_Windows ) {
            window->Render();
        }
    }

    void DestroyAll()
    {
        for ( auto& window : m_Windows ) {
            window->Cleanup();
        }
        m_Windows.clear();
    }

    CWindowBase* GetWindow( const std::string& name )
    {
        for ( auto& window : m_Windows ) {
            if ( window->GetWindowName() == name ) {
                return window.get();
            }
        }
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<CWindowBase>> m_Windows;
};

extern CWindowManager g_WndMngr;
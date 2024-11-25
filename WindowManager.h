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

    void AddWindow( std::unique_ptr<CWindowBase> window )
    {
        m_Windows.push_back( std::move( window ) );
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
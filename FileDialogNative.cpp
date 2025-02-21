#include <windows.h>
#include <shobjidl.h> 
#include <commdlg.h>
#include <iostream>

#include "Utils.h"
#include "FileDialogNative.h"

namespace FileDialog
{
    std::optional<std::string> OpenFile()
    {
        HRESULT hr;
        IFileOpenDialog* pFileOpen;
        std::optional<std::string> sFilePath = std::nullopt;

        hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );

        if ( SUCCEEDED( hr ) ) {
            hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen) );
            if ( SUCCEEDED( hr ) ) {
                hr = pFileOpen->Show( NULL );
                if ( SUCCEEDED( hr ) ) {
                    IShellItem* pItem;
                    hr = pFileOpen->GetResult( &pItem );
                    if ( SUCCEEDED( hr ) ) {
                        PWSTR pszFilePath;
                        hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );
                        if ( SUCCEEDED( hr ) ) {
                            sFilePath = Util::wcstombs( pszFilePath, _MAX_PATH );
                            CoTaskMemFree( pszFilePath );
                        }
                        pItem->Release();
                    }
                }
                pFileOpen->Release();
            }
            CoUninitialize();
        }

        return sFilePath;
    }


    std::optional<std::string> OpenFolder()
    {
        HRESULT hr;
        IFileOpenDialog* pFolderOpen;
        std::optional<std::string> sFolderPath = std::nullopt;

        hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );

        if ( SUCCEEDED( hr ) ) {
            hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFolderOpen) );
            if ( SUCCEEDED( hr ) ) {
                DWORD dwOptions;
                hr = pFolderOpen->GetOptions( &dwOptions );
                if ( SUCCEEDED( hr ) ) {
                    pFolderOpen->SetOptions( dwOptions | FOS_PICKFOLDERS );
                }
                hr = pFolderOpen->Show( NULL );
                if ( SUCCEEDED( hr ) ) {
                    IShellItem* pItem;
                    hr = pFolderOpen->GetResult( &pItem );
                    if ( SUCCEEDED( hr ) ) {
                        PWSTR pszFolderPath;
                        hr = pItem->GetDisplayName( SIGDN_FILESYSPATH, &pszFolderPath );
                        if ( SUCCEEDED( hr ) ) {
                            sFolderPath = Util::wcstombs( pszFolderPath, _MAX_PATH );
                            CoTaskMemFree( pszFolderPath );
                        }
                        pItem->Release();
                    }
                }
                pFolderOpen->Release();
            }
            CoUninitialize();
        }

        return sFolderPath;
    }
}
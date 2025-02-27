/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include <windows.h>
#include <shobjidl.h> 
#include <commdlg.h>
#include <iostream>

#include "Utils.h"
#include "FileDialogNative.h"

namespace FileDialog
{
    std::optional<std::string> OpenFile( Filter eFilter )
    {
        HRESULT hr;
        IFileOpenDialog* pFileOpen;
        std::optional<std::string> sFilePath = std::nullopt;

        hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );

        if ( SUCCEEDED( hr ) ) {
            hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen) );
            if ( SUCCEEDED( hr ) ) {

                std::vector<COMDLG_FILTERSPEC> fileTypes;
                switch ( eFilter ) {
                    case Filter::DAT:
                        fileTypes = { { L"DAT Files", L"*.dat" } };
                        break;
                    case Filter::BMP:
                        fileTypes = { { L"BMP Files", L"*.bmp" } };
                        break;
                    case Filter::ALL:
                    default:
                        fileTypes = { { L"All Files", L"*.*" } };
                        break;
                }
                pFileOpen->SetFileTypes( static_cast<UINT>(fileTypes.size()), fileTypes.data() );
                pFileOpen->SetFileTypeIndex( 1 );

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

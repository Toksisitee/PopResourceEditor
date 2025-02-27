/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#include "L2DFileDialog.h"
#include "FileDialog.h"

CFileDialogManager& CFileDialogManager::GetInstance()
{
    static CFileDialogManager instance;
    return instance;
}

void CFileDialogManager::ShowFileDialog( FileDialog::FileDialogType eType, std::function<void( const std::string& )> callback )
{
    if ( m_bFileDialogOpen ) return;

    m_bFileDialogOpen = true;
    m_Callback = callback;
    FileDialog::file_dialog_open = true;
    FileDialog::file_dialog_open_type = eType;
}

void CFileDialogManager::Update()
{
    if ( FileDialog::file_dialog_open ) {
        static char szPath[_MAX_PATH] = "";
        FileDialog::ShowFileDialog( &FileDialog::file_dialog_open, szPath, sizeof( szPath ),
            FileDialog::file_dialog_open_type,
            [this]( const std::string& sFilePath ) {
            if ( m_Callback ) m_Callback( sFilePath );
            m_bFileDialogOpen = false;
        } );
    }
    else m_bFileDialogOpen = false;
}

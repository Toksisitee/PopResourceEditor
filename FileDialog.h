/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <functional>
#include <string>

#include "L2DFileDialog.h"

class CFileDialogManager {
public:
    static CFileDialogManager& GetInstance();

    void ShowFileDialog( FileDialog::FileDialogType eType, std::function<void( const std::string& )> callback );
    void Update();

private:
    CFileDialogManager() = default;
    bool m_bFileDialogOpen = false;
    std::function<void( const std::string& )> m_Callback;
};

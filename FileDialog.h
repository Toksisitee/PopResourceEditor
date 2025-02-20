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
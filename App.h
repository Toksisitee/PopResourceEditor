/*
Copyright (c) 2024-2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <string>
#include "D3DApp.h"

class CEditorApp : public CD3DApp
{
public:
	CEditorApp() {};
	~CEditorApp() {};

	void			Run() override;
	void			InitializeImGui();

	bool			IsPopDirectorySet() const { return !m_sPopDirectory.empty(); }
	void			SetPopDirectory( const std::string& sDir ) { m_sPopDirectory = sDir; }
	void			SetExportDirectory( const std::string& sDir ) { m_sExportDirectory = sDir; }

	const std::string& GetPopDirectory() const { return m_sPopDirectory; }
	const std::string& GetExportDirectory() const { return m_sExportDirectory; }

protected:
	LRESULT			WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
private:
	std::string		m_sPopDirectory;
	std::string		m_sExportDirectory;
};

extern CEditorApp g_Editor;

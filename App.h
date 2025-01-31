#pragma once
#include "D3DApp.h"

class CEditorApp : public CD3DApp
{
public:
	CEditorApp() {};
	~CEditorApp() {};

	void			Run() override;
	void			InitializeImGui();
protected:
	LRESULT			WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam ) override;

private:

};

extern CEditorApp g_Editor;
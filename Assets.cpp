#include "Assets.h"

#include "Editor.h"
#include "App.h"

#include "WindowManager.h"
#include "SkyWnd.h"
#include "GhostWnd.h"
#include "FadeWnd.h"
#include "BigFadeWnd.h"
#include "CliffWnd.h"
#include "AlphaWnd.h"
#include "DispWnd.h"
#include "BlocksWnd.h"
#include "LevelWnd.h"

namespace Assets
{
	const char* GetFileTypeSz( FileType eFileType )
	{
		switch ( eFileType ) {
			case Assets::FileType::Palette:
				return "Palette";
			case Assets::FileType::Alpha:
				return "Alpha";
			case Assets::FileType::Sky:
				return "Sky";
			case Assets::FileType::Sprite:
				return "Sprite";
			case Assets::FileType::Ghost:
				return "Ghost";
			case Assets::FileType::Fade:
				return "Fade";
			case Assets::FileType::BigFade:
				return "BigFade";
			case Assets::FileType::Disp:
				return "Disp";
			case Assets::FileType::Cliff:
				return "Cliff";
			case Assets::FileType::Blocks:
				return "Blocks";
			case Assets::FileType::Level:
				return "Level";
			case Assets::FileType::Unknown:
				return "Unknown";
		}

		return "Unknown";
	}

	bool OpenWnd( const std::string& sFilePath, FileType eFileType )
	{
		auto pDevice = g_Editor.GetDevice();
		switch ( eFileType ) {
			case Assets::FileType::Palette:
			{

			}
			case Assets::FileType::Alpha:
			{
				auto pWnd = g_WndMngr.AddWindow<CAlphaWnd>( pDevice );
				pWnd->SetWindowName( sFilePath );
			}
			case Assets::FileType::Sky:
				return "Sky";
			case Assets::FileType::Sprite:
				return "Sprite";
			case Assets::FileType::Ghost:
				return "Ghost";
			case Assets::FileType::Fade:
				return "Fade";
			case Assets::FileType::BigFade:
				return "BigFade";
			case Assets::FileType::Disp:
				return "Disp";
			case Assets::FileType::Cliff:
				return "Cliff";
			case Assets::FileType::Blocks:
				return "Blocks";
			case Assets::FileType::Level:
				return "Level";
		}

		return false;
	}
}
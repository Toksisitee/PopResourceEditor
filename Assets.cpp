#include "Editor.h"

#include "Assets.h"

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
		}
		return "Unknown";
	}

	bool OpenAsset( const std::string& sFilePath, FileType eFileType )
	{
		return false;
	}
}
#pragma once

namespace Assets
{
	enum class FileType : uint8_t
	{
		None = 0,
		Palette,
		Alpha,
		Sky,
		Sprite,
		Ghost,
		Fade,
		BigFade,
		Cliff,
		Disp,
		Blocks,
		Level,
		Unknown
	};

	enum class Result : uint8_t
	{
		OK = 0,
		FAIL,
		OK_LOAD,
		FAIL_LOAD,
		OK_PARSE,
		FAIL_PARSE,
		OK_EXPORT,
		FAIL_EXPORT,
		OK_GENERATE,
		FAIL_GENERATE,
	};
}

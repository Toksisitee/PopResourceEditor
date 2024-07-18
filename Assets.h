#pragma once

namespace Assets
{
	enum class FileType : uint8_t
	{
		None = 0,
		Palette,
		Alpha
	};
	enum class ErrorCode : uint8_t
	{
		OK = 0,
		FAIL,
		LOAD,
		PARSE,
		EXPORT,
	};
}
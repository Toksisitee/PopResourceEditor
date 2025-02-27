/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once

namespace Assets
{
	enum class FileType : uint8_t
	{
		None = 0,
		Alpha,
		BigFade,
		Blocks,
		Cliff,
		Disp,
		Fade,
		Ghost,
		Level,
		Palette,
		Sky,
		Sprite,
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


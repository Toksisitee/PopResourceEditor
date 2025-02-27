/*
Copyright (c) 2025 Toksisitee. All rights reserved.

This work is licensed under the terms of the MIT license.
For a copy, refer to license.md or https://opensource.org/licenses/MIT

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
*/

#pragma once
#include <string>
#include <optional>

namespace FileDialog
{
	enum class Filter {
		ALL,
		DAT,
		BMP
	};

	extern std::optional<std::string> OpenFile( Filter eFilter = Filter::ALL );
	extern std::optional<std::string> OpenFolder();
}

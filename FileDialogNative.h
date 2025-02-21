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
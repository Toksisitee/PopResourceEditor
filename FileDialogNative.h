#pragma once
#include <string>
#include <optional>

namespace FileDialog
{
	extern std::optional<std::string> OpenFile();
	extern std::optional<std::string> OpenFolder();
}
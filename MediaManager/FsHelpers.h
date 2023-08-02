#pragma once
#include "defines.h"
/// <summary>
/// A collection of miscellaneous filesystem-related helper functions
/// </summary>
namespace FsHelpers
{
	std::string ReadFileContents(std::string file_path);
	std::string ReadFileContents(std::filesystem::path file_path);
	/// <summary>
	/// Converts a (utf8) string into a path object
	/// </summary>
	std::filesystem::path ToPath(std::string file_path);
	bool PathExists(std::string& file_path);
	bool PathExists(std::filesystem::path& file_path);

#ifdef mm_def_PLATFORM_WINDOWS
	std::string WideToMultibyte(std::wstring str);
#else
	std::string WideToMultibyte(std::string str);
#endif

	void WipeFolder(std::string cache_path);
};


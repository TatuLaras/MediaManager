#pragma once

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
	std::string WideToMultibyte(std::wstring str);
	void WipeFolder(std::string cache_path);
};


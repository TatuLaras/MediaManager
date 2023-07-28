#pragma once

namespace Helpers
{
	std::string ReadFileContents(std::string file_path);
	std::string ReadFileContents(std::filesystem::path file_path);
	/// <summary>
	/// Helps with using utf8 encoded std::string file paths with std::filesystem functions
	/// </summary>
	std::filesystem::path UTF8ToPath(std::string file_path);
	bool FilePathExists(std::string& file_path);
	bool FilePathExists(std::filesystem::path& file_path);
	std::string WStringToString(std::wstring str);
	void EmptyFolder(std::string cache_path);
};


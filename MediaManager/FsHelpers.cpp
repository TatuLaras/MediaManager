#include "pch.h"
#include "FsHelpers.h"


std::string FsHelpers::ReadFileContents(std::string file_path) {
	return ReadFileContents(ToPath(file_path));
}

std::string FsHelpers::ReadFileContents(std::filesystem::path file_path) {
	std::string read_string;
	std::string temp;
	std::ifstream input_file(file_path);

	if (input_file.fail()) return "";

	while (std::getline(input_file, temp)) {
		read_string += temp;
	}

	input_file.close();

	return read_string;
}



std::filesystem::path FsHelpers::ToPath(std::string file_path) {
	std::u8string utf8_string(file_path.cbegin(), file_path.cend());
	return std::filesystem::path(utf8_string);
}



bool FsHelpers::PathExists(std::string& file_path)
{
	return std::filesystem::exists(ToPath(file_path));
}

bool FsHelpers::PathExists(std::filesystem::path& file_path)
{
	return std::filesystem::exists(file_path);
}



std::string FsHelpers::WideToMultibyte(std::wstring str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}



void FsHelpers::WipeFolder(std::string cache_path)
{
	if (!FsHelpers::PathExists(cache_path)) return;

	for (const auto& entry : std::filesystem::directory_iterator(FsHelpers::ToPath(cache_path)))
		std::filesystem::remove_all(entry.path());
}
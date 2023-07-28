#include "pch.h"
#include "Helpers.h"

std::string Helpers::ReadFileContents(std::string file_path) {
	return ReadFileContents(UTF8ToPath(file_path));
}

std::string Helpers::ReadFileContents(std::filesystem::path file_path) {
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

std::filesystem::path Helpers::UTF8ToPath(std::string file_path) {
	std::u8string utf8_string(file_path.cbegin(), file_path.cend());
	return std::filesystem::path(utf8_string);
}

bool Helpers::FilePathExists(std::string& file_path)
{
	return std::filesystem::exists(UTF8ToPath(file_path));
}

bool Helpers::FilePathExists(std::filesystem::path& file_path)
{
	return std::filesystem::exists(file_path);
}


std::string Helpers::WStringToString(std::wstring str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

void Helpers::EmptyFolder(std::string cache_path)
{
	if (!Helpers::FilePathExists(cache_path)) return;

	for (const auto& entry : std::filesystem::directory_iterator(Helpers::UTF8ToPath(cache_path)))
		std::filesystem::remove_all(entry.path());
}
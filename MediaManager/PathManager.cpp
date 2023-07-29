#include "pch.h"
#include "PathManager.h"

std::string PathManager::GetBaseDataFolder(std::string subfolder)
{
	// Get native data storage folder
	if (base_data_folder.size() == 0) {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		char* pValue;
		size_t len;
		errno_t err =_dupenv_s(&pValue, &len, "APPDATA");
		if (!err && pValue != 0) base_data_folder = std::string(pValue) + "\\TatuLaras\\MediaManager";
#else
		base_data_folder = "~/.local/share/TatuLaras/MediaManager";
#endif
	}

	std::string directory = base_data_folder + mm_SLASH + subfolder;

	if (!FsHelpers::PathExists(directory))
		std::filesystem::create_directories(FsHelpers::ToPath(directory));

	return directory;
}

std::filesystem::path PathManager::GetPathOfExecutable()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	return path;
#else
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
#endif
}

std::string PathManager::GetBaseResourceFolder() {
	std::filesystem::path path = GetPathOfExecutable().parent_path();
	return FsHelpers::WideToMultibyte(path.native());
}
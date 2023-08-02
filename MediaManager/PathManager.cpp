#include "pch.h"
#include "PathManager.h"

std::string PathManager::GetBaseDataFolder(std::string subfolder)
{
	// Get native data storage folder
	if (base_data_folder.size() == 0) {
#ifdef mm_def_PLATFORM_WINDOWS
		char* pValue;
		size_t len;
		errno_t err =_dupenv_s(&pValue, &len, "APPDATA");
		if (!err && pValue != 0) base_data_folder = std::string(pValue) + "\\TatuLaras\\MediaManager";
#else
		base_data_folder = std::string(getenv("HOME")) + "/.local/share/TatuLaras/MediaManager";
#endif
	}

	std::string directory = base_data_folder + mm_SLASH + subfolder;

	if (!FsHelpers::PathExists(directory))
		std::filesystem::create_directories(FsHelpers::ToPath(directory));

	return directory;
}

std::filesystem::path PathManager::GetPathOfExecutable()
{
#ifdef mm_def_PLATFORM_WINDOWS
	wchar_t path[MAX_PATH] = { 0 };
	GetModuleFileNameW(NULL, path, MAX_PATH);
	return path;
#else
	char buff[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
      buff[len] = '\0';
      return std::string(buff);
    }
#endif

	return std::string("");
}

std::string PathManager::GetBaseResourceFolder() {
	std::filesystem::path path = GetPathOfExecutable().parent_path();
	return FsHelpers::WideToMultibyte(path.native());
}
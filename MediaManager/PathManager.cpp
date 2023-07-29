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

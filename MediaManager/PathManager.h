#pragma once
#include "FsHelpers.h"
#include "defines.h"

#ifndef mm_def_PLATFORM_WINDOWS
#include <unistd.h>
#include <limits.h>
#endif

class PathManager
{
public:
	static std::string GetBaseDataFolder(std::string subfolder);
	static std::string GetBaseResourceFolder();

private:
	inline static std::string base_data_folder = "";
	static std::filesystem::path GetPathOfExecutable();
};


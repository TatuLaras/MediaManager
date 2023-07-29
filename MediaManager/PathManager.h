#pragma once
#include "FsHelpers.h"
#include "defines.h"
class PathManager
{
public:
	static std::string GetBaseDataFolder(std::string subfolder);
	static std::string GetBaseResourceFolder();

private:
	inline static std::string base_data_folder = "";
	static std::filesystem::path GetPathOfExecutable();
};


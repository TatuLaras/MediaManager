#pragma once
#include "Helpers.h"
#include "defines.h"
class PathManager
{
public:
	static std::string GetBaseDataFolder(std::string subfolder);

private:
	inline static std::string base_data_folder = "";
};


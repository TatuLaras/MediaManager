#pragma once
#include "serialization.h"
#include "FsHelpers.h"
#include "defines.h"

/// <summary>
/// Handles storing user configuration data
/// </summary>
class Config
{
public:
	inline static std::string tmdb_key;
	inline static std::string movie_folder;
	inline static std::string tv_folder;

	inline static std::string video_player_command = mm_DEFAULT_PLAYER;

	inline static int font_size = mm_DEFAULT_FONT_SIZE;

	inline static std::vector<std::string> supported_file_formats = 
		{ ".mkv", ".mp4", ".avi", ".mov", ".m4v", ".m4p", ".flv", ".m4v", ".f4v", ".f4p", ".f4a", 
			".f4b", ".webm", ".vob", ".ogv", ".ogg", ".mts", ".m2ts", ".ts", ".qt",".wmv", ".rm", 
			".rmvb", ".viv", ".asf", ".amv", ".mpg", ".mpeg", ".mp2", ".mpv", ".mpe", ".m2v" };

	static void Init(std::string new_config_file_path) {
		initialized = true;
		config_file_path = new_config_file_path;
	}

	inline static bool IsInitialized() { return initialized; }

	static void SerializeJSON(nlohmann::json& json);
	static void DeserializeJSON(const nlohmann::json& json);
	static void SaveConfigToDisk();
	static bool LoadConfigFromDisk();

private:
	inline static std::string config_file_path;
	inline static bool initialized;

};


#pragma once
#include "serialization.h"
#include "FsHelpers.h"
#include "MathHelper.h"
#include "defines.h"

enum class ImageType {
	Poster,
	Still
};

/// <summary>
/// Handles storing user configured values
/// </summary>
class Config
{
public:
	inline static std::string tmdb_key;
	inline static std::string movie_folder;
	inline static std::string tv_folder;

	inline static std::string video_player_command = mm_DEFAULT_PLAYER;
	inline static int font_size = mm_DEFAULT_FONT_SIZE;
	inline static int label_max_length = mm_DEFAULT_LABEL_MAX_LENGTH;
	inline static bool use_large_images = mm_DEFAULT_USE_LARGE_IMAGES;
	inline static bool show_sublabels = mm_DEFAULT_SHOW_SUBLABELS;

	inline static std::vector<std::string> supported_file_formats = 
		{ ".mkv", ".mp4", ".avi", ".mov", ".m4v", ".m4p", ".flv", ".m4v", ".f4v", ".f4p", ".f4a", 
			".f4b", ".webm", ".vob", ".ogv", ".ogg", ".mts", ".m2ts", ".ts", ".qt",".wmv", ".rm", 
			".rmvb", ".viv", ".asf", ".amv", ".mpg", ".mpeg", ".mp2", ".mpv", ".mpe", ".m2v" };

	static void Init(std::string new_config_file_path) {
		initialized = true;
		config_file_path = new_config_file_path;
	}

	inline static bool IsInitialized() { return initialized; }
	inline static std::string GetImageSize(ImageType type) {

		if (type == ImageType::Poster) 
			return use_large_images ? 
			 mm_TMDB__IMAGE_SIZE_POSTER_LARGE : mm_TMDB__IMAGE_SIZE;

		if(type == ImageType::Still)
			return use_large_images ?
			mm_TMDB__IMAGE_SIZE_STILL_LARGE : mm_TMDB__IMAGE_SIZE;

		return mm_TMDB__IMAGE_SIZE;
	}
	

	static void SaveConfigToDisk();
	static bool LoadConfigFromDisk();


private:
	inline static std::string config_file_path;
	inline static bool initialized;

	static void SerializeJSON(nlohmann::json& json);
	static void DeserializeJSON(const nlohmann::json& json);
	/// <summary>
	/// Limits all values to certain safe ranges
	/// </summary>
	static void FilterValues();
};


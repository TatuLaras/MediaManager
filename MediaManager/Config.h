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
	inline static std::string movie_folders;
	inline static std::string tv_folders;

	inline static std::string video_player_command = mm_DEFAULT_PLAYER;
	inline static int font_size = mm_DEFAULT_FONT_SIZE;
	inline static uint32_t label_max_length = mm_DEFAULT_LABEL_MAX_LENGTH;
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
	
	static std::vector<std::string> GetMoviePaths() {
		std::string paths = Config::movie_folders;
		if (paths[paths.size() - 1] == ';') paths = paths.substr(0, paths.size() - 1);
		return SplitPaths(paths);
	}

	static std::vector<std::string> GetTVPaths() {
		std::string paths = Config::tv_folders;
		if (paths[paths.size() - 1] == ';') paths = paths.substr(0, paths.size() - 1);
		return SplitPaths(paths);
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

	static std::vector<std::string> SplitPaths(std::string paths) {
		std::vector<std::string> returnable;

		int last_cut = 0;
		for (uint32_t i = 0; i < paths.size(); i++) {

			std::string path;

			if (paths[i] == ';')
				path = paths.substr(last_cut, i - last_cut);

			if (i == paths.size() - 1)
				path = paths.substr(last_cut, i - last_cut + 1);


			if (path.size() > 0) {
				if (FsHelpers::PathExists(path))
					returnable.push_back(path);

				last_cut = i + 1;
			}
		}

		return returnable;
	}
};


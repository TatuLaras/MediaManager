#include "pch.h"
#include "Config.h"
void Config::SerializeJSON(nlohmann::json& json)
{
	json["tmdb_key"] = tmdb_key;
	json["movie_folder"] = movie_folder;
	json["tv_folder"] = tv_folder;
	json["video_player_command"] = video_player_command;
	json["font_size"] = font_size;
	json["label_max_length"] = label_max_length;
	json["large_images"] = large_images;
	json["show_sublabels"] = show_sublabels;
}

void Config::DeserializeJSON(const nlohmann::json& json)
{
	if (json.contains("tmdb_key") && json["tmdb_key"].is_string())
		tmdb_key = json["tmdb_key"].template get<std::string>();
	
	if (json.contains("movie_folder") && json["movie_folder"].is_string())
		movie_folder = json["movie_folder"].template get<std::string>();
	
	if (json.contains("tv_folder") && json["tv_folder"].is_string())
		tv_folder = json["tv_folder"].template get<std::string>();

	if (json.contains("video_player_command") && json["video_player_command"].is_string())
		video_player_command = json["video_player_command"].template get<std::string>();
	
	if (json.contains("font_size") && json["font_size"].is_number_integer())
		font_size = json["font_size"].template get<int>();

	if (json.contains("label_max_length") && json["label_max_length"].is_number_integer())
		label_max_length = json["label_max_length"].template get<int>();

	if (json.contains("large_images") && json["large_images"].is_boolean())
		large_images = json["large_images"].template get<bool>();

	if (json.contains("show_sublabels") && json["show_sublabels"].is_boolean())
		show_sublabels = json["show_sublabels"].template get<bool>();

	// Filter
	font_size = MathHelpers::Clamp(font_size, 4, 100);
	label_max_length = MathHelpers::Clamp(label_max_length, 5, 600);
}

void Config::SaveConfigToDisk()
{
	nlohmann::json json;
	SerializeJSON(json);
	std::string serialized_json = json.dump();

	std::ofstream output_file(config_file_path);

	output_file << serialized_json;

	output_file.close();
}

bool Config::LoadConfigFromDisk() {
	std::string raw = FsHelpers::ReadFileContents(config_file_path);
	if (raw.size() == 0) return false;

	nlohmann::json json = nlohmann::json::parse(raw);

	DeserializeJSON(json);

	return true;
}
#include "pch.h"
#include "Config.h"
void Config::SerializeJSON(nlohmann::json& json)
{
	if (font_size > 100) font_size = 100;

	json["tmdb_key"] = tmdb_key;
	json["movie_folder"] = movie_folder;
	json["tv_folder"] = tv_folder;
	json["video_player_command"] = video_player_command;
	json["font_size"] = font_size;
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
}

void Config::Save()
{
	nlohmann::json json;
	SerializeJSON(json);
	std::string serialized_json = json.dump();

	std::ofstream output_file(config_file_path);

	output_file << serialized_json;

	output_file.close();
}

bool Config::Load() {
	std::string raw = Helpers::ReadFileContents(config_file_path);
	if (raw.size() == 0) return false;

	nlohmann::json json = nlohmann::json::parse(raw);

	DeserializeJSON(json);

	return true;
}
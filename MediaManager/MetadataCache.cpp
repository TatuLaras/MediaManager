#include "pch.h"
#include "MetadataCache.h"

namespace MetadataCache {
	void UpdateID(unsigned int new_id, std::string metadata_identifier, bool tv_show) {
		Metadata* metadata;
		TVShowMetadata show_data;
		MovieMetadata movie_data;

		TMDB tmdb(Config::tmdb_key.c_str());

		if (tv_show) {
			show_data = tmdb.GetTVShowDataById(std::to_string(new_id));
			metadata = &show_data;
		}
		else {
			movie_data = tmdb.GetMovieMetadataById(std::to_string(new_id));
			metadata = &movie_data;
		}


		Store(metadata_identifier, metadata);
	}


	bool Retrieve(std::string metadata_identifier, Metadata* out_metadata)
	{
		std::string raw = FsHelpers::ReadFileContents(GetFilePath(metadata_identifier));
		if (raw.size() == 0) return false;

		nlohmann::json json = nlohmann::json::parse(raw);

		out_metadata->DeserializeJSON(json);

		return true;
	}

	void Store(std::string metadata_identifier, Metadata* metadata)
	{
		nlohmann::json json;
		metadata->SerializeJSON(json);
		std::string serialized_json = json.dump();

		std::ofstream output_file(GetFilePath(metadata_identifier));

		output_file << serialized_json;

		output_file.close();
	}

	std::filesystem::path GetFilePath(std::string metadata_identifier)
	{
		return FsHelpers::ToPath(PathManager::GetBaseDataFolder(mm_SUBFOLDER_METADATA) + mm_SLASH + metadata_identifier + ".json");
	}

	void SetWatched(std::string metadata_identifier, bool watched) {
		if (watched)
			Watch(metadata_identifier);
		else
			UnWatch(metadata_identifier);
	}

	void Watch(std::string metadata_identifier) {
		std::string watched_list_file = PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "watched";
		AddLineToFile(metadata_identifier, watched_list_file);
	}

	void UnWatch(std::string metadata_identifier) {

		// Read watched list and filter

		std::ifstream input_file(PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "watched");
		if (input_file.fail()) return;

		std::string new_watched_list = "";
		std::string current_line = "";

		while (std::getline(input_file, current_line)) {
			if (current_line != metadata_identifier) new_watched_list += current_line + "\n";
		}

		input_file.close();

		// Write new watched list
		std::ofstream output_file(PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "watched");
		output_file << new_watched_list;
		output_file.close();
	}

	bool IsWatched(std::string metadata_identifier) {
		return FileContainsLine(metadata_identifier, PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "watched");
	}

	void Hide(std::string metadata_identifier) {
		std::string hidden_list_file = PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "hidden";
		AddLineToFile(metadata_identifier, hidden_list_file);
	}

	bool IsMarkedHidden(std::string metadata_identifier) {
		return FileContainsLine(metadata_identifier, PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "hidden");
	}

	void UnHideAll() {
		std::filesystem::path hidden_list_file = 
			FsHelpers::ToPath(PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "hidden");
		if (FsHelpers::PathExists(hidden_list_file))
			std::filesystem::remove(hidden_list_file);
	}

	bool FileContainsLine(std::string line, std::string haystack_filepath)
	{
		std::ifstream input_file(FsHelpers::ToPath(haystack_filepath));
		if (input_file.fail()) return false;

		std::string current_line = "";

		while (std::getline(input_file, current_line)) {
			if (current_line == line) {
				input_file.close();
				return true;
			}
		}

		input_file.close();
		return false;
	}

	void AddLineToFile(std::string line, std::string filepath) {
		std::ofstream output_file(FsHelpers::ToPath(filepath), std::ios_base::app);
		output_file << line << std::endl;
		output_file.close();
	}
}
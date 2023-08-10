#pragma once
#include "serialization.h"
#include "defines.h"

struct Genre : SubSerializable {
	int id = 0;
	std::string name = "";

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;
};

struct ProductionCompany : SubSerializable {
	int id = 0;
	std::string logo_path = "";
	std::string name = "";
	std::string origin_country = "";

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;
};

struct ProductionCountry : SubSerializable {
	std::string iso_3166_1 = "";
	std::string name = "";

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;
};

struct Language : SubSerializable {
	std::string english_name = "";
	std::string iso_639_1 = "";
	std::string name = "";

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;
};

struct Creator : SubSerializable {
	int id = 0;
	std::string credit_id = "";
	std::string name = "";
	int gender = 0;
	std::string profile_path = "";

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;
};

struct TVShowEpisode : SubSerializable {
	int id = 0;
	std::string name = "";
	std::string overview = "";
	float vote_average = 0;
	int vote_count = 0;
	std::string air_date = "";
	int episode_number = 0;
	std::string production_code = "";
	int runtime = 0;
	int season_number = 0;
	int show_id = 0;
	std::string still_path = "";
	std::string file_path = "";
	std::string filename = "";
	bool watched = false;

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;

	std::string GetFormattedVoteAverage() {
		return Helpers::FormatValue(vote_average) + " (TMDB)";
	}

	std::string GetFormattedRuntime() {
		if (!runtime) return "";
		return std::to_string(runtime) + "m";
	}

	std::string GetFormattedEpisodeNumber() {
		return std::to_string(episode_number);
	}
};

struct TVShowSeason : SubSerializable {
	std::string air_date = "";
	int episode_count = 0;
	int id = 0;
	std::string name = "";
	std::string overview = "";
	std::string poster_path = "";
	int season_number = 0;
	float vote_average = 0;
	std::vector<TVShowEpisode> episodes;
	bool should_render = false;

	virtual std::map<std::string, nlohmann::json> ToSerializableType() override;
	virtual void FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode = false) override;

	std::string GetFormattedVoteAverage() {
		return Helpers::FormatValue(vote_average) + " (TMDB)";
	}

	TVShowEpisode* GetEpisode(int episode_number) {
		for (uint32_t i = 0; i < episodes.size(); i++) {
			if (episodes[i].episode_number == episode_number) return &episodes[i];
		}

		return nullptr;
	}
};

/// <summary>
/// Parent data structure for generic metadata
/// </summary>
struct Metadata : Serializable {
	bool adult = false;
	std::string metadata_source = "";
	std::string backdrop_path = "";
	std::vector<Genre> genres;
	int id = 0;
	std::string original_language = "";
	std::string original_title = "";
	std::string overview = "";
	float popularity = 0;
	std::string poster_path = "";
	std::string title = "";
	float vote_average = 0;
	int vote_count = 0;
	std::string homepage = "";
	std::vector<ProductionCompany> production_companies;
	std::vector<ProductionCountry> production_countries;
	std::vector<Language> spoken_languages;
	std::string status = "";
	std::string tagline = "";

	void SerializeJSON(nlohmann::json& json) override;
	void DeserializeJSON(const nlohmann::json& json) override;

	virtual void ParseTMDB(const nlohmann::json& json) = 0;
	
	std::string GetFormattedVoteAverage() {
		return Helpers::FormatValue(vote_average) + " (TMDB)";
	}

	std::string GetFormattedGenres() {
		std::string str;

		for (uint32_t i = 0; i < genres.size(); i++) {
			if (i > 0) str += ", ";
			str += genres[i].name;
		}
		return str;
	}

	bool operator< (const Metadata& other) const {
		return title.compare(other.title) < 0;
	}

protected:
	void CommonParse(const nlohmann::json& json);
};

struct TVShowMetadata : Metadata
{
	std::string first_air_date = "";
	std::vector<std::string> origin_country;
	std::vector<int> episode_run_time;
	std::vector<Creator> created_by;
	bool in_production = false;
	std::vector<std::string> languages;
	std::string last_air_date = "";
	std::vector<ProductionCompany> networks;
	int number_of_episodes = 0;
	int number_of_seasons = 0;
	std::string type = "";
	std::string folder_name = "";
	std::vector<TVShowSeason> seasons;
	std::vector<TVShowEpisode> orphan_episodes;

	void SerializeJSON(nlohmann::json& json);
	void DeserializeJSON(const nlohmann::json& json);
	void ParseTMDB(const nlohmann::json& json) override;

	std::string GetFormattedRuntime() {
		if (episode_run_time.size() == 0) return "";

		std::string returnable = "";

		for (uint32_t i = 0; i < episode_run_time.size(); i++) {
			if (i > 0) returnable += ", ";
			returnable += std::to_string(episode_run_time[i]) + "m";
		}

		return returnable;
	}

	TVShowSeason* GetSeason(int season_number) {
		for (uint32_t i = 0; i < seasons.size(); i++) {
			if (seasons[i].season_number == season_number) return &seasons[i];
		}

		return nullptr;
	}

private:
	void CommonParse(const nlohmann::json& json);
};

struct MovieMetadata : Metadata
{
	std::string release_date = "";
	std::string imdb_id = "";
	int budget = 0;
	int revenue = 0;
	int runtime = 0;
	int year = 0;
	bool video = false;
	std::string belongs_to_collection = "";
	std::string file_path = "";
	std::string filename = "";
	bool watched = false;

	void SerializeJSON(nlohmann::json& json);
	void DeserializeJSON(const nlohmann::json& json);
	void ParseTMDB(const nlohmann::json& json);

	std::string GetFormattedRuntime() {
		if (!runtime) return "";
		return std::to_string(runtime / 60) + "h " + std::to_string(runtime % 60) + "m";
	}

private:
	void CommonParse(const nlohmann::json& json);
};

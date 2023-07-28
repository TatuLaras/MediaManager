#include "pch.h"
#include "Metadata.h"

void Metadata::SerializeJSON(nlohmann::json& json)
{
	json = nlohmann::json{
		{"adult", adult},
		{"metadata_source", metadata_source},
		{"backdrop_path", backdrop_path},
		{"id", id},
		{"original_language", original_language},
		{"original_title", original_title},
		{"overview", overview},
		{"popularity", popularity},
		{"poster_path", poster_path},
		{"title", title},
		{"vote_average", vote_average},
		{"vote_count", vote_count},
		{"homepage", homepage},
		{"production_companies", SubSerializable::ArrayToSerializable(production_companies)},
		{"genres", SubSerializable::ArrayToSerializable(genres)},
		{"production_countries", SubSerializable::ArrayToSerializable(production_countries)},
		{"spoken_languages", SubSerializable::ArrayToSerializable(spoken_languages)},
		{"status", status},
		{"tagline", tagline}
	};
}


void TVShowMetadata::SerializeJSON(nlohmann::json& json)
{
	Metadata::SerializeJSON(json);

	json["first_air_date"] = first_air_date;
	json["origin_country"] = origin_country;
	json["episode_run_time"] = episode_run_time;
	json["in_production"] = in_production;
	json["languages"] = languages;
	json["last_air_date"] = last_air_date;
	json["number_of_episodes"] = number_of_episodes;
	json["number_of_seasons"] = number_of_seasons;
	json["created_by"] = SubSerializable::ArrayToSerializable(created_by);
	json["networks"] = SubSerializable::ArrayToSerializable(networks);
	json["seasons"] = SubSerializable::ArrayToSerializable(seasons);
	json["type"] = type;
}

void MovieMetadata::SerializeJSON(nlohmann::json& json)
{
	Metadata::SerializeJSON(json);

	json["release_date"] = release_date;
	json["imdb_id"] = imdb_id;
	json["budget"] = budget;
	json["revenue"] = revenue;
	json["runtime"] = runtime;
	json["video"] = video;
	json["belongs_to_collection"] = belongs_to_collection;
	json["watched"] = watched;
}


std::map<std::string, nlohmann::json> ProductionCompany::ToSerializableType()
{
	return std::map<std::string, nlohmann::json>{
		{ "id", nlohmann::json(id) },
		{ "logo_path", nlohmann::json(logo_path) },
		{ "name", nlohmann::json(name) },
		{ "origin_country", nlohmann::json(origin_country) }
	};
}

void ProductionCompany::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["id"].is_number_integer())
		id = data["id"];

	if(data["logo_path"].is_string())
		logo_path = data["logo_path"];

	if (data["name"].is_string())
		name = data["name"];

	if (data["origin_country"].is_string())
		origin_country = data["origin_country"];
}

std::map<std::string, nlohmann::json> TVShowSeason::ToSerializableType()
{
	return std::map<std::string, nlohmann::json>{
		{ "air_date", nlohmann::json(air_date) },
		{ "episode_count", nlohmann::json(episode_count) },
		{ "id", nlohmann::json(id) },
		{ "name", nlohmann::json(name) },
		{ "overview", nlohmann::json(overview) },
		{ "poster_path", nlohmann::json(poster_path) },
		{ "season_number", nlohmann::json(season_number) },
		{ "vote_average", nlohmann::json(vote_average) },
		{ "episodes", nlohmann::json(SubSerializable::ArrayToSerializable(episodes)) }
	};
}

void TVShowSeason::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["air_date"].is_string())
		air_date = data["air_date"];

	if (data["episode_count"].is_number_integer())
		episode_count = data["episode_count"];

	if (data["id"].is_number_integer())
	id = data["id"];
	
	if (data["name"].is_string())
		name = data["name"];

	if (data["overview"].is_string())
		overview = data["overview"];

	if(data["poster_path"].is_string())
		poster_path = data["poster_path"];

	if (data["season_number"].is_number_integer())
		season_number = data["season_number"];
	
	if (data["vote_average"].is_number_float())
		vote_average = data["vote_average"];

	if (tmdb_mode) return;

	episodes = SubSerializable::ArrayFromSerializable<TVShowEpisode>(data["episodes"]);
}

std::map<std::string, nlohmann::json> TVShowEpisode::ToSerializableType()
{
	return std::map<std::string, nlohmann::json>{
		{ "id", nlohmann::json(id) },
		{ "name", nlohmann::json(name) },
		{ "overview", nlohmann::json(overview) },
		{ "vote_average", nlohmann::json(vote_average) },
		{ "vote_count", nlohmann::json(vote_count) },
		{ "air_date", nlohmann::json(air_date) },
		{ "episode_number", nlohmann::json(episode_number) },
		{ "production_code", nlohmann::json(production_code) },
		{ "runtime", nlohmann::json(runtime) },
		{ "season_number", nlohmann::json(season_number) },
		{ "show_id", nlohmann::json(show_id) },
		{ "still_path", nlohmann::json(still_path) },
		{ "watched", nlohmann::json(watched) },
	};
}

void TVShowEpisode::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["vote_average"].is_number_float())
		vote_average = data["vote_average"];


	if (data["id"].is_number_integer())
		id = data["id"];

	if (data["vote_count"].is_number_integer())
		vote_count = data["vote_count"];

	if (data["episode_number"].is_number_integer())
		episode_number = data["episode_number"];

	if (data["season_number"].is_number_integer())
		season_number = data["season_number"];

	if (data["show_id"].is_number_integer())
		show_id = data["show_id"];

	if (data["runtime"].is_number_integer())
		runtime = data["runtime"];


	if (data["name"].is_string())
		name = data["name"];

	if (data["overview"].is_string())
		overview = data["overview"];

	if (data["air_date"].is_string())
		air_date = data["air_date"];

	if (data["production_code"].is_string())
		production_code = data["production_code"];

	if(data["still_path"].is_string())
		still_path = data["still_path"];

}

std::map<std::string, nlohmann::json> Genre::ToSerializableType()
{
	return std::map<std::string, nlohmann::json>{
		{ "id", nlohmann::json(id) },
		{ "name", nlohmann::json(name) }
	};
}

void Genre::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["id"].is_number_integer())
		id = data["id"];

	if (data["name"].is_string())
		name = data["name"];
}

std::map<std::string, nlohmann::json> ProductionCountry::ToSerializableType()
{
	return std::map<std::string, nlohmann::json>{
		{ "iso_3166_1", nlohmann::json(iso_3166_1) },
		{ "name", nlohmann::json(name) }
	};
}

void ProductionCountry::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["iso_3166_1"].is_string())
		iso_3166_1 = data["iso_3166_1"];

	if (data["name"].is_string())
		name = data["name"];
}

std::map<std::string, nlohmann::json> Language::ToSerializableType()
{
	return std::map<std::string, nlohmann::json>{
		{ "english_name", nlohmann::json(english_name) },
		{ "iso_639_1", nlohmann::json(iso_639_1) },
		{ "name", nlohmann::json(name) }
	};
}

void Language::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["english_name"].is_string())
		english_name = data["english_name"];

	if (data["iso_639_1"].is_string())
		iso_639_1 = data["iso_639_1"];

	if (data["name"].is_string())
		name = data["name"];
}

std::map<std::string, nlohmann::json> Creator::ToSerializableType()
{
	return std::map<std::string, nlohmann::json> {
		{ "id", nlohmann::json(id) },
		{ "credit_id", nlohmann::json(credit_id) },
		{ "name", nlohmann::json(name) },
		{ "gender", nlohmann::json(gender) },
		{ "profile_path", nlohmann::json(profile_path) }
	};
}

void Creator::FromSerializableType(std::map<std::string, nlohmann::json> data, bool tmdb_mode)
{
	if (data["id"].is_number_integer())
		id = data["id"];

	if (data["gender"].is_number_integer())
		gender = data["gender"];

	if (data["credit_id"].is_string())
		credit_id = data["credit_id"];

	if (data["name"].is_string())
		name = data["name"];

	if (data["profile_path"].is_string())
		profile_path = data["profile_path"];

}

template<typename T>
std::vector<std::map<std::string, nlohmann::json>> SubSerializable::ArrayToSerializable(std::vector<T> data)
{
	static_assert(std::is_base_of<SubSerializable, T>::value, "Only use types inherited from SubSerializable.");

	std::vector<std::map<std::string, nlohmann::json>> serializable;

	for (int i = 0; i < data.size(); i++) {
		serializable.push_back(data[i].ToSerializableType());
	}

	return serializable;
}

template<typename T>
std::vector<T> SubSerializable::ArrayFromSerializable(std::vector<std::map<std::string, nlohmann::json>> data, bool tmdb_mode)
{
	static_assert(std::is_base_of<SubSerializable, T>::value, "Only use types inherited from SubSerializable.");

	std::vector<T> native;

	for (int i = 0; i < data.size(); i++) {
		T new_obj;
		new_obj.FromSerializableType(data[i], tmdb_mode);
		native.push_back(new_obj);
	}

	return native;
}



void Metadata::DeserializeJSON(const nlohmann::json& json)
{
	CommonParse(json);
}

void Metadata::CommonParse(const nlohmann::json& json)
{
	if (json.contains("adult") && json["adult"].is_boolean())
		adult = json["adult"].template get<bool>();
	
	if (json.contains("metadata_source") && json["metadata_source"].is_string())
		metadata_source = json["metadata_source"].template get<std::string>();

	if (json.contains("backdrop_path") && json["backdrop_path"].is_string())
		backdrop_path = json["backdrop_path"].template get<std::string>();

	if (json.contains("id") && json["id"].is_number_integer())
		id = json["id"].template get<int>();

	if (json.contains("original_language") && json["original_language"].is_string())
		original_language = json["original_language"].template get<std::string>();

	if (json.contains("original_title") && json["original_title"].is_string())
		original_title = json["original_title"].template get<std::string>();

	if (json.contains("overview") && json["overview"].is_string())
		overview = json["overview"].template get<std::string>();

	if (json.contains("popularity") && json["popularity"].is_number_float())
		popularity = json["popularity"].template get<float>();

	if (json.contains("poster_path") && json["poster_path"].is_string())
		poster_path = json["poster_path"].template get<std::string>();

	if (json.contains("vote_average") && json["vote_average"].is_number_float())
		vote_average = json["vote_average"].template get<float>();

	if (json.contains("vote_count") && json["vote_count"].is_number_integer())
		vote_count = json["vote_count"].template get<int>();

	if (json.contains("homepage") && json["homepage"].is_string())
		homepage = json["homepage"].template get<std::string>();

	if (json.contains("production_companies"))
		production_companies = SubSerializable::ArrayFromSerializable<ProductionCompany>(
			json["production_companies"].template get<std::vector<std::map<std::string, nlohmann::json>>>()
			);

	if (json.contains("genres"))
		genres = SubSerializable::ArrayFromSerializable<Genre>(
			json["genres"].template get<std::vector<std::map<std::string, nlohmann::json>>>()
			);

	if (json.contains("production_countries"))
		production_countries = SubSerializable::ArrayFromSerializable<ProductionCountry>(
			json["production_countries"].template get<std::vector<std::map<std::string, nlohmann::json>>>()
			);


	if (json.contains("spoken_languages"))
		spoken_languages = SubSerializable::ArrayFromSerializable<Language>(
			json["spoken_languages"].template get<std::vector<std::map<std::string, nlohmann::json>>>()
			);

	if (json.contains("status") && json["status"].is_string())
		status = json["status"].template get<std::string>();

	if (json.contains("tagline") && json["tagline"].is_string())
		tagline = json["tagline"].template get<std::string>();
}






void TVShowMetadata::DeserializeJSON(const nlohmann::json& json)
{
	Metadata::DeserializeJSON(json);
	CommonParse(json);

	if (json.contains("title") && json["title"].is_string())
		title = json["title"].template get<std::string>();

	if (json.contains("seasons"))
		seasons = SubSerializable::ArrayFromSerializable<TVShowSeason>(
			json["seasons"].template get<std::vector<std::map<std::string, nlohmann::json>>>(), false
			);
}

void TVShowMetadata::ParseTMDB(const nlohmann::json& json)
{
	Metadata::CommonParse(json);
	CommonParse(json);

	if (json.contains("name") && json["name"].is_string())
		title = json["name"].template get<std::string>();

	if (json.contains("seasons"))
		seasons = SubSerializable::ArrayFromSerializable<TVShowSeason>(
			json["seasons"].template get<std::vector<std::map<std::string, nlohmann::json>>>(), true
			);
}

void TVShowMetadata::CommonParse(const nlohmann::json& json)
{
	if (json.contains("first_air_date") && json["first_air_date"].is_string())
		first_air_date = json["first_air_date"].template get<std::string>();

	if (json.contains("origin_country") && json["origin_country"].is_array())
		origin_country = json["origin_country"].template get<std::vector<std::string>>();

	if (json.contains("episode_run_time") && json["episode_run_time"].is_array())
		episode_run_time = json["episode_run_time"].template get<std::vector<int>>();

	if (json.contains("in_production") && json["in_production"].is_boolean())
		in_production = json["in_production"].template get<bool>();

	if (json.contains("languages") && json["languages"].is_array())
		languages = json["languages"].template get<std::vector<std::string>>();

	if (json.contains("last_air_date") && json["last_air_date"].is_string())
		last_air_date = json["last_air_date"].template get<std::string>();

	if (json.contains("number_of_episodes") && json["number_of_episodes"].is_number_integer())
		number_of_episodes = json["number_of_episodes"].template get<int>();

	if (json.contains("number_of_seasons") && json["number_of_seasons"].is_number_integer())
		number_of_seasons = json["number_of_seasons"].template get<int>();

	if (json.contains("type") && json["type"].is_string())
		type = json["type"].template get<std::string>();

	if (json.contains("created_by"))
		created_by = SubSerializable::ArrayFromSerializable<Creator>(
			json["created_by"].template get<std::vector<std::map<std::string, nlohmann::json>>>()
			);

	if (json.contains("networks"))
		networks = SubSerializable::ArrayFromSerializable<ProductionCompany>(
			json["networks"].template get<std::vector<std::map<std::string, nlohmann::json>>>()
			);
}


void MovieMetadata::DeserializeJSON(const nlohmann::json& json)
{
	Metadata::DeserializeJSON(json);
	CommonParse(json);

	// .. Erikoistapaukset 
}

void MovieMetadata::ParseTMDB(const nlohmann::json& json)
{
	Metadata::CommonParse(json);
	CommonParse(json);
}

void MovieMetadata::CommonParse(const nlohmann::json& json)
{
	if (json.contains("release_date") && json["release_date"].is_string())
		release_date = json["release_date"].template get<std::string>();

	if (json.contains("imdb_id") && json["imdb_id"].is_string())
		imdb_id = json["imdb_id"].template get<std::string>();

	if (json.contains("budget") && json["budget"].is_number_integer())
		budget = json["budget"].template get<int>();

	if (json.contains("revenue") && json["revenue"].is_number_integer())
		revenue = json["revenue"].template get<int>();

	if (json.contains("runtime") && json["runtime"].is_number_integer())
		runtime = json["runtime"].template get<int>();

	if (json.contains("video") && json["video"].is_boolean())
		video = json["video"].template get<bool>();

	if (json.contains("watched") && json["watched"].is_boolean())
		watched = json["watched"].template get<bool>();

	if (json.contains("title") && json["title"].is_string())
		title = json["title"].template get<std::string>();

	if (json.contains("belongs_to_collection") && json["belongs_to_collection"].is_string())
		belongs_to_collection = json["belongs_to_collection"].template get<std::string>();
}

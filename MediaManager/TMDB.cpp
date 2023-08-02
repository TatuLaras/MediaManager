#include "pch.h"
#include "TMDB.h"

#define mm_TMDB__ENDPOINT__MOVIE_SEARCH "/search/movie"
#define mm_TMDB__ENDPOINT__TV_SEARCH "/search/tv"

/// <summary>
/// You can find your bearer auth token for TMDB at https://developer.themoviedb.org/reference/intro/authentication
/// </summary>
TMDB::TMDB(const char* auth_bearer_token)
{
    bearer_token = auth_bearer_token;
}

/// <summary>
/// Searches TMDB with a given title and returns the metadata
/// </summary>
MovieMetadata TMDB::GetMovieMetadataBySearchQuery(std::string search_query)
{
    std::string tmdb_id = GetIDBySearchQuery(search_query, mm_TMDB__ENDPOINT__MOVIE_SEARCH);
    if (tmdb_id.size() == 0) return MovieMetadata();

    return GetMovieMetadataById(tmdb_id);
}

MovieMetadata TMDB::GetMovieMetadataById(std::string id)
{
    MovieMetadata metadata;

    HTTP::Response res =
        HTTP::RestJson::Get(
            "https://api.themoviedb.org/3/movie/" + id,
            bearer_token
        );

    if (res.status_code == 401) {
        key_error_to_handle = true;
        return metadata;
    }
    
    if (res.status_code != 200) 
    {
        tmdb_general_error_to_handle = true;

        // Parse status message
        nlohmann::json error_json = nlohmann::json::parse(res.data);

        if (error_json.contains("status_message"))
            tmdb_general_error_reason = error_json["status_message"];

        return metadata;
    }

    nlohmann::json json = nlohmann::json::parse(res.data);

    metadata.ParseTMDB(json);

    return metadata;
}

/// <summary>
/// Finds TMDB id by using a given url-encoded search query
/// </summary>
std::string TMDB::GetIDBySearchQuery(std::string search_query, std::string endpoint)
{
    std::string url = mm_TMDB__BASE_URL + endpoint + "?" + search_query;
    HTTP::Response res =
        HTTP::RestJson::Get(
            url,
            bearer_token
        );

    if (res.status_code == 401) {
        key_error_to_handle = true;
        return "";
    }

    if (res.status_code != 200)
    {
        tmdb_general_error_to_handle = true;

        // Parse status message
        nlohmann::json error_json = nlohmann::json::parse(res.data);

        if (error_json.contains("status_message"))
            tmdb_general_error_reason = error_json["status_message"];

        return "";
    }

    nlohmann::json json = nlohmann::json::parse(res.data);

    // Check if not valid data or no matches found
    if (!json.contains("total_results") || json["total_results"].template get<int>() == 0) return "";

    // Return id of first result

    std::vector<std::map<std::string, nlohmann::json>> results = 
        json["results"].template get<std::vector<std::map<std::string, nlohmann::json>>>();

    return std::to_string(results[0]["id"].template get<int>());
}

void TMDB::DownloadImage(std::string image_path, std::string options)
{
    HTTP::DownloadFileTo((mm_TMDB__BASE_URL_IMAGE + options + image_path).c_str(),
        (PathManager::GetBaseDataFolder(mm_SUBFOLDER_IMAGE) + mm_SLASH + image_path.substr(1, image_path.size() - 1)).c_str());
}

/// <summary>
/// Parses an usable url search query from a pirated media's filename
/// </summary>
std::string TMDB::ParseQueryFromMovieFilename(std::string filename)
{
    std::string parsed = "query=";

    int last_cut = 0;
    for (int i = 0; i < filename.size(); i++) {
        // Break word at every special character
        if (!std::isalnum(filename[i], std::locale())) {
            // Empty segment
            if (last_cut == i) {
                last_cut++;
                continue;
            }

            // Check if current segment is a release year
            std::string segment = filename.substr(last_cut, i - last_cut);

            // Remove parentheses for parsing
            std::string segment_to_parse = segment;

            if (segment_to_parse[0] == '(' && segment_to_parse[segment_to_parse.size() - 1] == ')')
                segment_to_parse = segment_to_parse.substr(1, segment_to_parse.size() - 2);

            int year;
            sscanf_s(segment_to_parse.c_str(), "%d", &year);

            // If a release year is encountered in filename, stop parsing
            if (year >= 1888 && year <= 2100) {
                parsed += "&year=";
                parsed += std::to_string(year);
                break;
            }
               
            // Add to final string with url encoded space (%20) delimiter
            parsed += last_cut == 0 ? "" : "%20";
            parsed += segment;
            last_cut = i + 1;
        }
    }

    return parsed;

}
std::string TMDB::ParseQueryFromMovieFilename(const char* filename)
{
    return TMDB::ParseQueryFromMovieFilename(std::string(filename));
}

MovieMetadata TMDB::GetMovieMetadataByFilename(const char* filename)
{
    return GetMovieMetadataBySearchQuery(ParseQueryFromMovieFilename(filename));
}





std::string TMDB::ParseQueryFromTVShowFolderName(std::string folder_name)
{
    std::string parsed = "query=";

    int last_cut = 0;
    for (int i = 0; i < folder_name.size(); i++) {
        // Break word at every special character
        if (!std::isalnum(folder_name[i], std::locale())) {
            // Empty segment
            if (last_cut == i) {
                last_cut++;
                continue;
            }

            std::string segment = folder_name.substr(last_cut, i - last_cut);

            // Add to final string with url encoded space (%20) delimiter
            parsed += last_cut == 0 ? "" : "%20";
            parsed += segment;
            last_cut = i + 1;
        }

    }

    std::string segment = folder_name.substr(last_cut, folder_name.size() - last_cut);
    parsed += "%20";
    parsed += segment;


    return parsed;
}

std::string TMDB::ParseQueryFromTVShowFolderName(const char* folder_name)
{
    return ParseQueryFromTVShowFolderName(std::string(folder_name));
}

TVShowMetadata TMDB::GetTVShowDataBySearchQuery(std::string search_query)
{
    std::string tmdb_id = GetIDBySearchQuery(search_query, mm_TMDB__ENDPOINT__TV_SEARCH);
    if (tmdb_id.size() == 0) return TVShowMetadata();

    return GetTVShowDataById(tmdb_id);
}

TVShowMetadata TMDB::GetTVShowDataById(std::string id)
{
    // Get top level series details

    TVShowMetadata series_metadata;

    HTTP::Response res =
        HTTP::RestJson::Get(
            mm_TMDB__BASE_URL + std::string("/tv/") + id,
            bearer_token
        );

    if (res.status_code == 401) {
        key_error_to_handle = true;
        return series_metadata;
    }

    if (res.status_code != 200)
    {
        tmdb_general_error_to_handle = true;

        // Parse status message
        nlohmann::json error_json = nlohmann::json::parse(res.data);

        if (error_json.contains("status_message"))
            tmdb_general_error_reason = error_json["status_message"];

        return series_metadata;
    }

    nlohmann::json json = nlohmann::json::parse(res.data);

    series_metadata.ParseTMDB(json);

    if (series_metadata.seasons.size() == 0) return series_metadata;

    // Get episode metadata
    for (int i = 0; i < series_metadata.seasons.size(); i++) {
        TVShowSeason* season = &series_metadata.seasons[i];
        GetTVEpisodeData(id, std::to_string(season->season_number), &season->episodes);

    }

    return series_metadata;
}

TVShowMetadata TMDB::GetTVShowDataByFolderName(const char* filename)
{
    return GetTVShowDataBySearchQuery(ParseQueryFromTVShowFolderName(filename));
}



void TMDB::GetTVEpisodeData(std::string series_id, std::string season_number, std::vector<TVShowEpisode>* out_episodes)
{
    HTTP::Response res =
        HTTP::RestJson::Get(
            mm_TMDB__BASE_URL + std::string("/tv/") + series_id + std::string("/season/") + season_number,
            bearer_token
        );

    if (res.status_code == 401) {
        key_error_to_handle = true;
        return;
    }

    if (res.status_code != 200)
    {
        tmdb_general_error_to_handle = true;

        // Parse status message
        nlohmann::json error_json = nlohmann::json::parse(res.data);

        if (error_json.contains("status_message"))
            tmdb_general_error_reason = error_json["status_message"];

        return;
    }


    nlohmann::json json = nlohmann::json::parse(res.data);

    if (json.contains("episodes") && json["episodes"].is_array())
        *out_episodes= SubSerializable::ArrayFromSerializable<TVShowEpisode>(
            json["episodes"].template get<std::vector<std::map<std::string, nlohmann::json>>>(), 
            true
            );
}

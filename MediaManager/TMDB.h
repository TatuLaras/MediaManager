#pragma once
#include <string>
#include "HTTP.h"
#include "Metadata.h"
#include "PathManager.h"
#include "json.hpp"
#include <cctype>
#include "defines.h"
class TMDB
{
public:
    inline static bool key_error_to_handle;
    inline static bool tmdb_general_error_to_handle;
    inline static std::string tmdb_general_error_reason;

    static std::string ParseQueryFromMovieFilename(std::string filename);
    static std::string ParseQueryFromMovieFilename(const char* filename);

    TMDB(const char* auth_bearer_token);
    MovieMetadata GetMovieMetadataBySearchQuery(std::string search_query);
    MovieMetadata GetMovieMetadataById(std::string id);
    MovieMetadata GetMovieMetadataByFilename(const char* filename);
    std::string ParseQueryFromTVShowFolderName(std::string folder_name);
    std::string ParseQueryFromTVShowFolderName(const char* folder_name);
    TVShowMetadata GetTVShowDataBySearchQuery(std::string search_query);
    TVShowMetadata GetTVShowDataById(std::string id);
    TVShowMetadata GetTVShowDataByFolderName(const char* filename);
    void GetTVEpisodeData(std::string series_id, std::string season_number, std::vector<TVShowEpisode>* out_episodes);
    std::string GetIDBySearchQuery(std::string search_query, std::string endpoint);
    void DownloadImage(std::string image_path, std::string options = mm_TMDB__IMAGE_SIZE);
private:
    const char* bearer_token;
};


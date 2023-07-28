#pragma once
#include "MetadataCache.h"
#include "Metadata.h"
#include "MenuItem.h"
#include "TMDB.h"
#include "PlayFileAction.h"
#include "InfoPanel.h"
#include "Config.h"
#include "App.h"

/// <summary>
/// Handles scanning the media library
/// </summary>
class Library
{
public:
	inline static bool movie_thread_done = true;
	inline static bool tv_thread_done = true;

	static std::vector<MovieMetadata> ScanMovieLibrary();
	static std::vector<TVShowMetadata> ScanTVLibrary();
	static void GenerateMenuTree(MenuItem* root_item);
	static void GenerateMoviesMenuTree(MenuItem* parent);
	static void GenerateTVMenuTree(MenuItem* parent);
	static void ScanTVShow(std::filesystem::directory_entry folder_entry, 
		std::vector<TVShowMetadata>* metadata_list);
	static std::string GetPlaceHolderTitle(std::string filename);
	static void JoinThreads() {
		for (int i = 0; i < workers.size(); i++)
			if (workers[i].joinable()) workers[i].join();
	}
	static bool CanScan() { return movie_thread_done && tv_thread_done;  }
private:
	static void ParseSeasonEpisodeFromFilename(std::string filename, 
		int* out_season, int* out_episode);
	inline static std::vector<std::thread> workers;
};


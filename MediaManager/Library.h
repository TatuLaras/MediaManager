#pragma once
#include "MetadataCache.h"
#include "Metadata.h"
#include "MenuItem.h"
#include "TMDB.h"
#include "PlayFileAction.h"
#include "MetadataInfoPanel.h"
#include "PlaceholderInfoPanel.h"
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

	inline static bool ReadyForScanning() 
		{ return movie_thread_done && tv_thread_done;  }

	/// <summary>
	/// Joins Library classes' worker threads
	/// </summary>
	inline static void JoinThreads() {
		JoinThreads(workers);
	}
	
	/// <summary>
	/// Makes the program wait for all the threads to be finished
	/// </summary>
	inline static void JoinThreads(std::vector<std::thread>& workers_list) {
		for (int i = 0; i < workers_list.size(); i++)
			if (workers_list[i].joinable()) workers_list[i].join();
	}

	/// <summary>
	/// Generate whole menu tree based on library and fetched metadata. 
	/// Spawns threads.
	/// </summary>
	static void StartGeneratingMenuTree(MenuItem* root_item);


private:
	inline static std::vector<std::thread> workers;

	static void GenerateMoviesMenuTree(MenuItem* parent);
	static void GenerateTVMenuTree(MenuItem* parent);

	static std::vector<MovieMetadata> ScanMovieLibrary();
	static std::vector<TVShowMetadata> ScanTVLibrary();
	static void ScanTVShow(std::filesystem::directory_entry folder_entry, 
		std::vector<TVShowMetadata>* data, int index);

	static void ParseSeasonAndEpisodeFromFilename(std::string filename, 
		int* out_season, int* out_episode);
};


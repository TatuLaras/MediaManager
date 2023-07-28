#include "pch.h"
#include "Library.h"

#define tl_PLACE_HOLDER_TITLE_MAX_LENGTH 30

namespace fs = std::filesystem;


/// <summary>
/// Generate whole menu hierarchy tree based on library and fetched metadata. (multi-threaded)
/// </summary>
/// <returns></returns>
void Library::GenerateMenuTree(MenuItem* root_item)
{
	movie_thread_done = false;
	tv_thread_done = false;

	MenuItem* movies = root_item->AddItem("Movies");
	workers.push_back(std::thread(&Library::GenerateMoviesMenuTree, movies));
	MenuItem* shows = root_item->AddItem("TV Shows");
	workers.push_back(std::thread(&Library::GenerateTVMenuTree, shows));
	if (!Helpers::FilePathExists(Config::movie_folder)) {
		movies->infopanel = new PlaceholderInfoPanel("Go to Menu > Options to set a movie folder. \nThen you can go to Library > Refresh.");
	}
	else {
		movies->infopanel = new PlaceholderInfoPanel();
	}

	if (!Helpers::FilePathExists(Config::tv_folder)) {
		shows->infopanel = new PlaceholderInfoPanel("Go to Menu > Options to set a TV show folder. \nThen you can go to Library > Refresh.");
	}
	else {
		shows->infopanel = new PlaceholderInfoPanel();
	}

}






std::vector<MovieMetadata> Library::ScanMovieLibrary()
{
	std::vector<MovieMetadata> datas;
	TMDB tmdb = TMDB(Config::tmdb_key.c_str());

	// Iterate through all files in configured movie folder

	if (!Helpers::FilePathExists(Config::movie_folder))
		return datas;

	for (const auto& entry : fs::recursive_directory_iterator(Helpers::UTF8ToPath(Config::movie_folder))) {
		if (!entry.path().has_extension()) continue;
		std::string filename = Helpers::WStringToString(entry.path().filename().native());
		std::string file_path = Helpers::WStringToString(entry.path().native());
		std::string file_extension = Helpers::WStringToString(entry.path().extension());

		bool extension_supported = 
			std::count(
				Config::supported_file_formats.begin(), 
				Config::supported_file_formats.end(), 
				file_extension
			);

		if (!extension_supported) continue;
		if (MetadataCache::IsHidden(filename)) continue;

		MovieMetadata metadata;

		// Try getting from disk 
		if (!MetadataCache::TryGet(filename, &metadata) || metadata.id == 0) {
			// Fetch from tmdb
			metadata = tmdb.GetMovieMetadataByFilename(filename.c_str());
			MetadataCache::Cache(filename, &metadata);
		}

		metadata.file_path = file_path;
		metadata.filename = filename;
		metadata.watched = MetadataCache::IsWatched(filename);

		if (metadata.title.size() == 0) {
			metadata.title = GetPlaceHolderTitle(filename);
		}

		datas.push_back(metadata);
	}

	return datas;
}

std::vector<TVShowMetadata> Library::ScanTVLibrary()
{
	std::vector<TVShowMetadata> datas;
	std::vector<std::thread> show_workers;

	if (!Helpers::FilePathExists(Config::tv_folder))
		return datas;

	// Get all folders immediately under the tv show folder
	for (const auto& folder_entry : fs::directory_iterator(Helpers::UTF8ToPath(Config::tv_folder))) {
		if (folder_entry.path().has_extension()) continue;

		show_workers.push_back(std::thread(&Library::ScanTVShow, folder_entry, &datas));
	}

	for (int i = 0; i < show_workers.size(); i++)
		if (show_workers[i].joinable()) show_workers[i].join();

	return datas;
}



void Library::ScanTVShow(fs::directory_entry folder_entry, std::vector<TVShowMetadata>* metadata_list)
{
	TMDB tmdb = TMDB(Config::tmdb_key.c_str());

	std::string folder_name = Helpers::WStringToString(folder_entry.path().filename().native()).c_str();
	std::string folder_path = Helpers::WStringToString(folder_entry.path().native()).c_str();

	if (MetadataCache::IsHidden(folder_name)) return;


	TVShowMetadata metadata;

	// Try getting from disk 
	if (!MetadataCache::TryGet(folder_name, &metadata) || metadata.id == 0) {
		// Fetch from tmdb
		metadata = tmdb.GetTVShowDataByFolderName(folder_name.c_str());
		MetadataCache::Cache(folder_name, &metadata);
	}

	if (metadata.title.size() == 0) {
		metadata.title = GetPlaceHolderTitle(folder_name);
	}

	metadata.folder_name = folder_name;

	// Link file paths

	// Recursively get all files inside the folder
	for (const auto& file_entry : fs::recursive_directory_iterator(Helpers::UTF8ToPath(folder_path))) {
		bool is_folder = !file_entry.path().has_extension();
		if (is_folder) continue;

		std::string filename = Helpers::WStringToString(file_entry.path().filename().native()).c_str();
		std::string file_path = Helpers::WStringToString(file_entry.path().native()).c_str();
		std::string file_extension = Helpers::WStringToString(file_entry.path().extension());

		bool extension_supported =
			std::count(
				Config::supported_file_formats.begin(),
				Config::supported_file_formats.end(),
				file_extension
			);

		if (!extension_supported) continue;

		int season = 1;
		int episode = 0;
		ParseSeasonEpisodeFromFilename(filename, &season, &episode);

		TVShowEpisode* episode_data = nullptr;
		TVShowSeason* season_data = nullptr;

		// Try to get episode
		season_data = metadata.GetSeason(season);
		if (season_data && episode) {
			season_data->should_render = true;
			episode_data = season_data->GetEpisode(episode);
		}

		// Orphan episode
		if (episode_data == nullptr) {
			TVShowEpisode orphan;
			orphan.name = GetPlaceHolderTitle(filename);
			orphan.filename = filename;
			orphan.file_path = file_path;
			orphan.watched = MetadataCache::IsWatched(filename);
			metadata.orphan_episodes.push_back(orphan);
		}
		else {
			episode_data->filename = filename;
			episode_data->file_path = file_path;
			episode_data->watched = MetadataCache::IsWatched(filename);
		}
	}

	metadata_list->push_back(metadata);
}

void Library::GenerateMoviesMenuTree(MenuItem* parent) {
	// Get data
	std::vector<MovieMetadata> movie_metadatas = ScanMovieLibrary();

	for (int i = 0; i < movie_metadatas.size(); i++) {
		std::string label = movie_metadatas[i].title;
		parent->AddItem(label);
		MenuItem* movie_menu_item = parent->Last();

		// Play file action
		PlayFileAction* action = new PlayFileAction(movie_metadatas[i].file_path);
		movie_menu_item->action = action;

		// Infopanel
		movie_menu_item->infopanel = new PosterInfoPanel(movie_metadatas[i]);
		movie_menu_item->infopanel->SetPanelImage(movie_metadatas[i].poster_path);


		movie_menu_item->role = MenuItemRole::Watchable;
		movie_menu_item->watched = movie_metadatas[i].watched;
		movie_menu_item->filename = movie_metadatas[i].filename;
		movie_menu_item->metadata_identifier = movie_metadatas[i].filename;
	}

	movie_thread_done = true;
}

void Library::GenerateTVMenuTree(MenuItem* parent) {
	// Get data
	std::vector<TVShowMetadata> series_metadatas = ScanTVLibrary();

	// Series
	for (int series = 0; series < series_metadatas.size(); series++) {

		TVShowMetadata* show = &series_metadatas[series];

		parent->AddItem(show->title);
		MenuItem* show_menu_item = parent->Last();

		// Infopanel
		show_menu_item->infopanel = new PosterInfoPanel(*show);
		show_menu_item->infopanel->SetPanelImage(show->poster_path);

		show_menu_item->role = MenuItemRole::TVShow;
		show_menu_item->metadata_identifier = show->folder_name;
		// Seasons
		for (int s = 0; s < show->seasons.size(); s++) {

			TVShowSeason* show_season = &show->seasons[s];
			if (!show_season->should_render) continue;

			show_menu_item->AddItem(show_season->name);
			MenuItem* season_menu_item = show_menu_item->Last();

			// Infopanel
			season_menu_item->infopanel = new PosterInfoPanel(*show_season);
			season_menu_item->infopanel->SetPanelImage(show_season->poster_path);

			season_menu_item->role = MenuItemRole::TVSeason;

			// Episodes
			for (int e = 0; e < show_season->episodes.size(); e++) {

				TVShowEpisode* show_episode = &show_season->episodes[e];
				if (show_episode->file_path.size() == 0) continue;

				season_menu_item->AddItem(show_episode->name);
				MenuItem* episode_menu_item = season_menu_item->Last();

				// Infopanel
				episode_menu_item->infopanel = new PosterInfoPanel(*show_episode);
				episode_menu_item->infopanel->SetPanelImage(show_episode->still_path);

				// Play action
				PlayFileAction* action = new PlayFileAction(show_episode->file_path);
				episode_menu_item->action = action;


				episode_menu_item->role = MenuItemRole::Watchable;
				episode_menu_item->filename = show_episode->filename;
				episode_menu_item->watched = show_episode->watched;
			}
		}

		if (show->orphan_episodes.size() == 0) continue;

		// Orphan episodes
		show_menu_item->AddItem("Unknown");
		MenuItem* orphans_menu_item = show_menu_item->Last();
		orphans_menu_item->infopanel = new PlaceholderInfoPanel();

		orphans_menu_item->role = MenuItemRole::TVSeason;

		for (int e = 0; e < show->orphan_episodes.size(); e++) {

			TVShowEpisode* show_episode = &show->orphan_episodes[e];
			if (show_episode->file_path.size() == 0) continue;

			orphans_menu_item->AddItem(show_episode->name);
			MenuItem* episode_menu_item = orphans_menu_item->Last();

			// Infopanel
			episode_menu_item->infopanel = new PosterInfoPanel(*show_episode);

			// Play action
			PlayFileAction* action = new PlayFileAction(show_episode->file_path);
			episode_menu_item->action = action;


			episode_menu_item->role = MenuItemRole::Watchable;
			episode_menu_item->filename = show_episode->filename;
			episode_menu_item->watched = show_episode->watched;
		}

	}

	tv_thread_done = true;
}

std::string Library::GetPlaceHolderTitle(std::string filename)
{
	if (filename.size() > tl_PLACE_HOLDER_TITLE_MAX_LENGTH)
		return filename.substr(0, tl_PLACE_HOLDER_TITLE_MAX_LENGTH) + "...";

	return filename;
}

void Library::ParseSeasonEpisodeFromFilename(std::string filename, int* out_season, int* out_episode) {
	int s_encountered_at = 0;
	int e_encountered_at = 0;

	// Try parse season and episode from filename
	for (int i = 0; i < filename.size(); i++) {
		bool digits_ahead = isdigit(filename[i + 1]) && isdigit(filename[i + 2]);
		if ((filename[i] == 's' || filename[i] == 'S') && digits_ahead) s_encountered_at = i;
		if ((filename[i] == 'e' || filename[i] == 'E')
			&& s_encountered_at != -1 && digits_ahead)
			e_encountered_at = i;
	}

	if (s_encountered_at) {
		std::string season = filename.substr(s_encountered_at + 1, 2);
		sscanf_s(season.c_str(), "%d", out_season);
	}

	if (e_encountered_at) {
		std::string episode = filename.substr(e_encountered_at + 1, 2);
		sscanf_s(episode.c_str(), "%d", out_episode);
	}
}

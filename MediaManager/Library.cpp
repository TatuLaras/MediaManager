#include "pch.h"
#include "Library.h"

namespace fs = std::filesystem;



void Library::StartGeneratingMenuTree(MenuItem* root_item)
{
	movie_thread_done = false;
	tv_thread_done = false;

	MenuItem* movies = root_item->AddItem("Movies");
	workers.push_back(std::thread(&Library::GenerateMoviesMenuTree, movies));

	MenuItem* shows = root_item->AddItem("TV Shows");
	workers.push_back(std::thread(&Library::GenerateTVMenuTree, shows));


	if (!FsHelpers::PathExists(Config::movie_folder))
		movies->infopanel = new PlaceholderInfoPanel("Go to Menu > Options to set a movie folder. \nThen you can go to Library > Refresh.");
	else
		movies->infopanel = new PlaceholderInfoPanel();


	if (!FsHelpers::PathExists(Config::tv_folder))
		shows->infopanel = new PlaceholderInfoPanel("Go to Menu > Options to set a TV show folder. \nThen you can go to Library > Refresh.");
	else
		shows->infopanel = new PlaceholderInfoPanel();

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
		MetadataInfoPanel* infopanel = new MetadataInfoPanel(movie_metadatas[i]);
		infopanel->SetPanelImage(movie_metadatas[i].poster_path, ImageType::Poster);
		movie_menu_item->infopanel = infopanel;


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

		// So many indentations...
		// Kinda messy but can't bother coming up with anything more elegant

		// Infopanel
		MetadataInfoPanel* infopanel = new MetadataInfoPanel(*show);
		infopanel->SetPanelImage(show->poster_path, ImageType::Poster);
		show_menu_item->infopanel = infopanel;

		show_menu_item->role = MenuItemRole::TVShow;
		show_menu_item->metadata_identifier = show->folder_name;
		// Seasons
		for (int s = 0; s < show->seasons.size(); s++) {

			TVShowSeason* show_season = &show->seasons[s];
			if (!show_season->should_render) continue;

			show_menu_item->AddItem(show_season->name);
			MenuItem* season_menu_item = show_menu_item->Last();

			// Infopanel
			MetadataInfoPanel* infopanel = new MetadataInfoPanel(*show_season);
			infopanel->SetPanelImage(show_season->poster_path, ImageType::Poster);
			season_menu_item->infopanel = infopanel;

			season_menu_item->role = MenuItemRole::TVSeason;

			// Episodes
			for (int e = 0; e < show_season->episodes.size(); e++) {

				TVShowEpisode* show_episode = &show_season->episodes[e];
				if (show_episode->file_path.size() == 0) continue;

				season_menu_item->AddItem(show_episode->name, std::to_string(show_episode->episode_number));
				MenuItem* episode_menu_item = season_menu_item->Last();

				// Infopanel
				MetadataInfoPanel* infopanel = new MetadataInfoPanel(*show_season);
				infopanel->SetPanelImage(show_episode->still_path, ImageType::Still);
				episode_menu_item->infopanel = infopanel;

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
			episode_menu_item->infopanel = new MetadataInfoPanel(*show_episode);

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


std::vector<MovieMetadata> Library::ScanMovieLibrary()
{
	std::vector<MovieMetadata> data_list;
	TMDB tmdb = TMDB(Config::tmdb_key.c_str());

	if (!FsHelpers::PathExists(Config::movie_folder))
		return data_list;

	// Iterate through all files in configured movie folder
	for (const auto& entry : fs::recursive_directory_iterator(FsHelpers::ToPath(Config::movie_folder))) {

		if (!entry.path().has_extension()) continue;

		std::string entry_filename = FsHelpers::WideToMultibyte(entry.path().filename().native());
		std::string entry_file_path = FsHelpers::WideToMultibyte(entry.path().native());
		std::string entry_file_extension = FsHelpers::WideToMultibyte(entry.path().extension());

		bool extension_supported =
			std::count(
				Config::supported_file_formats.begin(),
				Config::supported_file_formats.end(),
				entry_file_extension
			);

		if (!extension_supported) continue;
		if (MetadataCache::IsMarkedHidden(entry_filename)) continue;

		MovieMetadata metadata;

		// Try getting from disk 
		if (!MetadataCache::Retrieve(entry_filename, &metadata) || metadata.id == 0) {
			// Fetch from tmdb
			metadata = tmdb.GetMovieMetadataByFilename(entry_filename.c_str());

			MetadataCache::Store(entry_filename, &metadata);
		}

		metadata.file_path = entry_file_path;
		metadata.filename = entry_filename;
		metadata.watched = MetadataCache::IsWatched(entry_filename);

		if (metadata.title.size() == 0)
			metadata.title = entry_filename;
		

		data_list.push_back(metadata);
	}

	return data_list;
}

std::vector<TVShowMetadata> Library::ScanTVLibrary()
{
	std::vector<TVShowMetadata> data_list;
	std::vector<std::thread> tv_show_workers;

	if (!FsHelpers::PathExists(Config::tv_folder))
		return data_list;

	// Get all folders immediately under the configured tv show folder
	for (const auto& folder_entry : fs::directory_iterator(FsHelpers::ToPath(Config::tv_folder))) {

		if (folder_entry.path().has_extension()) continue;

		// New thread for this tv show
		tv_show_workers.push_back(std::thread(&Library::ScanTVShow, folder_entry, &data_list));
	}

	JoinThreads(tv_show_workers);

	return data_list;
}

void Library::ScanTVShow(fs::directory_entry folder_entry, std::vector<TVShowMetadata>* metadata_list)
{
	TMDB tmdb = TMDB(Config::tmdb_key.c_str());

	std::string entry_folder_name = FsHelpers::WideToMultibyte(folder_entry.path().filename().native()).c_str();
	std::string entry_folder_path = FsHelpers::WideToMultibyte(folder_entry.path().native()).c_str();

	if (MetadataCache::IsMarkedHidden(entry_folder_name)) return;


	TVShowMetadata metadata;

	// Try getting from disk 
	if (!MetadataCache::Retrieve(entry_folder_name, &metadata) || metadata.id == 0) {
		// Fetch from tmdb
		metadata = tmdb.GetTVShowDataByFolderName(entry_folder_name.c_str());

		MetadataCache::Store(entry_folder_name, &metadata);
	}

	if (metadata.title.size() == 0) {
		metadata.title = entry_folder_name;
	}

	metadata.folder_name = entry_folder_name;

	// Link file paths

	// Recursively get all files inside the folder
	for (const auto& file_entry : fs::recursive_directory_iterator(FsHelpers::ToPath(entry_folder_path))) {
		bool is_folder = !file_entry.path().has_extension();
		if (is_folder) continue;

		std::string entry_filename = FsHelpers::WideToMultibyte(file_entry.path().filename().native()).c_str();
		std::string entry_file_path = FsHelpers::WideToMultibyte(file_entry.path().native()).c_str();
		std::string entry_file_extension = FsHelpers::WideToMultibyte(file_entry.path().extension());

		bool extension_supported =
			std::count(
				Config::supported_file_formats.begin(),
				Config::supported_file_formats.end(),
				entry_file_extension
			);

		if (!extension_supported) continue;

		int season = 1;
		int episode = 0;
		ParseSeasonAndEpisodeFromFilename(entry_filename, &season, &episode);

		TVShowEpisode* episode_data = nullptr;
		TVShowSeason* season_data = nullptr;

		season_data = metadata.GetSeason(season);

		if (season_data != nullptr && episode > 0) {
			season_data->should_render = true;
			episode_data = season_data->GetEpisode(episode);
		}

		// Orphan episode
		if (episode_data == nullptr) {
			TVShowEpisode orphan;
			orphan.name = entry_filename;
			orphan.filename = entry_filename;
			orphan.file_path = entry_file_path;
			orphan.watched = MetadataCache::IsWatched(entry_filename);
			metadata.orphan_episodes.push_back(orphan);
		}
		else {
			episode_data->filename = entry_filename;
			episode_data->file_path = entry_file_path;
			episode_data->watched = MetadataCache::IsWatched(entry_filename);
		}
	}

	metadata_list->push_back(metadata);
}


void Library::ParseSeasonAndEpisodeFromFilename(std::string filename, int* out_season, int* out_episode) {
	int s_encountered_at = 0;
	int e_encountered_at = 0;

	for (int i = 0; i < filename.size(); i++) {
		bool exists_digits_ahead = isdigit(filename[i + 1]) && isdigit(filename[i + 2]);

		if ((filename[i] == 's' || filename[i] == 'S') && exists_digits_ahead) 
			s_encountered_at = i;

		if ((filename[i] == 'e' || filename[i] == 'E') && exists_digits_ahead)
			e_encountered_at = i;
	}

	if (s_encountered_at > 0) {
		std::string season = filename.substr(s_encountered_at + 1, 2);
		sscanf_s(season.c_str(), "%d", out_season);
	}

	if (e_encountered_at > 0) {
		std::string episode = filename.substr(e_encountered_at + 1, 2);
		sscanf_s(episode.c_str(), "%d", out_episode);
	}
}

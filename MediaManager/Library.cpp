#include "pch.h"
#include "Library.h"

namespace fs = std::filesystem;

void Library::StartGeneratingMenuTree(MenuItem *root_item)
{
	movie_thread_done = false;
	tv_thread_done = false;

	MenuItem *movies = root_item->AddItem("Movies");
	workers.push_back(std::thread(&Library::GenerateMoviesMenuTree, movies));
	MenuItem *shows = root_item->AddItem("TV Shows");
	workers.push_back(std::thread(&Library::GenerateTVMenuTree, shows));

	if (!FsHelpers::PathExists(Config::movie_folders))
		movies->infopanel = new PlaceholderInfoPanel("Go to Menu > Options to set a movie folder. \nThen you can go to Library > Refresh.");
	else
		movies->infopanel = new PlaceholderInfoPanel();

	if (!FsHelpers::PathExists(Config::tv_folders))
		shows->infopanel = new PlaceholderInfoPanel("Go to Menu > Options to set a TV show folder. \nThen you can go to Library > Refresh.");
	else
		shows->infopanel = new PlaceholderInfoPanel();
}

void Library::GenerateMoviesMenuTree(MenuItem *parent)
{
	// Get data
	std::vector<MovieMetadata> movie_metadatas = ScanMovieLibrary();

	if (movie_metadatas.size() == 0)
	{
		movie_thread_done = true;
		return;
	}

	std::sort(movie_metadatas.begin(), movie_metadatas.end());

	for (uint32_t i = 0; i < movie_metadatas.size(); i++)
	{
		std::string label = movie_metadatas[i].title;
		parent->AddItem(label);
		MenuItem *movie_menu_item = parent->Last();

		// Play file action
		PlayFileAction *action = new PlayFileAction(movie_metadatas[i].file_path);
		movie_menu_item->action = action;

		// Infopanel
		MetadataInfoPanel *infopanel = new MetadataInfoPanel(movie_metadatas[i]);
		infopanel->SetPanelImage(movie_metadatas[i].poster_path, ImageType::Poster);
		movie_menu_item->infopanel = infopanel;

		movie_menu_item->role = MenuItemRole::Watchable;
		movie_menu_item->watched = movie_metadatas[i].watched;
		movie_menu_item->filename = movie_metadatas[i].filename;
		movie_menu_item->metadata_identifier = movie_metadatas[i].filename;
	}

	movie_thread_done = true;
}

void Library::GenerateTVMenuTree(MenuItem *parent)
{
	// Get data
	std::vector<TVShowMetadata> series_metadatas = ScanTVLibrary();

	if (series_metadatas.size() == 0)
	{
		tv_thread_done = true;
		return;
	}

	std::sort(series_metadatas.begin(), series_metadatas.end());

	// Series
	for (uint32_t series = 0; series < series_metadatas.size(); series++)
	{

		TVShowMetadata *show = &series_metadatas[series];

		parent->AddItem(show->title);
		MenuItem *show_menu_item = parent->Last();

		// So many indentations...
		// Kinda messy but can't bother coming up with anything more elegant

		// Infopanel
		MetadataInfoPanel *infopanel = new MetadataInfoPanel(*show);
		infopanel->SetPanelImage(show->poster_path, ImageType::Poster);
		show_menu_item->infopanel = infopanel;

		show_menu_item->role = MenuItemRole::TVShow;
		show_menu_item->metadata_identifier = show->folder_name;
		// Seasons
		for (uint32_t s = 0; s < show->seasons.size(); s++)
		{

			TVShowSeason *show_season = &show->seasons[s];
			if (!show_season->should_render)
				continue;

			show_menu_item->AddItem(show_season->name);
			MenuItem *season_menu_item = show_menu_item->Last();

			// Infopanel
			MetadataInfoPanel *infopanel = new MetadataInfoPanel(*show_season);
			infopanel->SetPanelImage(show_season->poster_path, ImageType::Poster);
			season_menu_item->infopanel = infopanel;

			season_menu_item->role = MenuItemRole::TVSeason;

			// Episodes
			for (uint32_t e = 0; e < show_season->episodes.size(); e++)
			{

				TVShowEpisode *show_episode = &show_season->episodes[e];
				if (show_episode->file_path.size() == 0)
					continue;

				season_menu_item->AddItem(show_episode->name, show_episode->GetFormattedEpisodeNumber());
				MenuItem *episode_menu_item = season_menu_item->Last();

				// Infopanel
				MetadataInfoPanel *infopanel = new MetadataInfoPanel(*show_episode);
				infopanel->SetPanelImage(show_episode->still_path, ImageType::Still);
				episode_menu_item->infopanel = infopanel;

				// Play action
				PlayFileAction *action = new PlayFileAction(show_episode->file_path);
				episode_menu_item->action = action;

				episode_menu_item->role = MenuItemRole::Watchable;
				episode_menu_item->filename = show_episode->filename;
				episode_menu_item->watched = show_episode->watched;
			}
		}

		if (show->orphan_episodes.size() == 0)
			continue;

		// Orphan episodes
		show_menu_item->AddItem("Unknown");
		MenuItem *orphans_menu_item = show_menu_item->Last();
		orphans_menu_item->infopanel = new PlaceholderInfoPanel();

		orphans_menu_item->role = MenuItemRole::TVSeason;

		for (uint32_t e = 0; e < show->orphan_episodes.size(); e++)
		{

			TVShowEpisode *show_episode = &show->orphan_episodes[e];
			if (show_episode->file_path.size() == 0)
				continue;

			orphans_menu_item->AddItem(show_episode->name);
			MenuItem *episode_menu_item = orphans_menu_item->Last();

			// Infopanel
			episode_menu_item->infopanel = new MetadataInfoPanel(*show_episode);

			// Play action
			PlayFileAction *action = new PlayFileAction(show_episode->file_path);
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

	std::vector<std::string> paths = Config::GetMoviePaths();

	if (paths.size() == 0)
	{
		std::vector<MovieMetadata> empty;
		return empty;
	}

	// Iterate through all files in configured movie folders
	for (uint32_t i = 0; i < paths.size(); i++)
		for (const auto &entry : fs::recursive_directory_iterator(FsHelpers::ToPath(paths[i])))
		{

			if (!entry.path().has_extension())
				continue;

			std::string entry_filename = FsHelpers::WideToMultibyte(entry.path().filename().native());
			std::string entry_file_path = FsHelpers::WideToMultibyte(entry.path().native());
			std::string entry_file_extension = FsHelpers::WideToMultibyte(entry.path().extension());

			bool extension_supported =
				std::count(
					Config::supported_file_formats.begin(),
					Config::supported_file_formats.end(),
					entry_file_extension);

			if (!extension_supported)
				continue;
			if (MetadataCache::IsMarkedHidden(entry_filename))
				continue;

			MovieMetadata metadata;

			// Try getting from disk
			if (!MetadataCache::Retrieve(entry_filename, &metadata) || metadata.id == 0)
			{
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
	std::vector<std::string> paths = Config::GetTVPaths();

	if (paths.size() == 0)
	{
		std::vector<TVShowMetadata> empty;
		return empty;
	}

	std::vector<TVShowMetadata> data;
	std::vector<std::thread> tv_show_workers;
	int index = 0;

	// Get all folders immediately under the configured tv show folders
	for (uint32_t i = 0; i < paths.size(); i++)
		for (const auto &folder_entry : fs::directory_iterator(FsHelpers::ToPath(paths[i])))
		{
			if (folder_entry.path().has_extension())
				continue;

			std::string entry_folder_name = FsHelpers::WideToMultibyte(folder_entry.path().filename().native()).c_str();
			std::string entry_folder_path = FsHelpers::WideToMultibyte(folder_entry.path().native()).c_str();

			if (MetadataCache::IsMarkedHidden(entry_folder_name))
				continue;

			// Placeholder
			data.push_back(TVShowMetadata());

			// New thread for this tv show
			tv_show_workers.push_back(std::thread(&Library::ScanTVShow, entry_folder_name, entry_folder_path, &data, index));

			index++;
		}

	JoinThreads(tv_show_workers);

	return data;
}

void Library::ScanTVShow(std::string entry_folder_name, std::string entry_folder_path, std::vector<TVShowMetadata> *data, int data_index)
{
	TMDB tmdb = TMDB(Config::tmdb_key.c_str());

	TVShowMetadata metadata;

	// Try getting from disk
	if (!MetadataCache::Retrieve(entry_folder_name, &metadata) || metadata.id == 0)
	{
		// Fetch from tmdb
		metadata = tmdb.GetTVShowDataByFolderName(entry_folder_name.c_str());

		MetadataCache::Store(entry_folder_name, &metadata);
	}

	if (metadata.title.size() == 0)
	{
		metadata.title = entry_folder_name;
	}

	metadata.folder_name = entry_folder_name;

	// Link file paths

	// Recursively get all files inside the folder
	for (const auto &file_entry : fs::recursive_directory_iterator(FsHelpers::ToPath(entry_folder_path)))
	{
		bool is_folder = !file_entry.path().has_extension();
		if (is_folder)
			continue;

		std::string entry_filename = FsHelpers::WideToMultibyte(file_entry.path().filename().native()).c_str();
		std::string entry_file_path = FsHelpers::WideToMultibyte(file_entry.path().native()).c_str();
		std::string entry_file_extension = FsHelpers::WideToMultibyte(file_entry.path().extension());

		bool extension_supported =
			std::count(
				Config::supported_file_formats.begin(),
				Config::supported_file_formats.end(),
				entry_file_extension);

		if (!extension_supported)
			continue;

		int season = 1;
		int episode = 0;
		ParseSeasonAndEpisodeFromFilename(entry_filename, &season, &episode);

		TVShowEpisode *episode_data = nullptr;
		TVShowSeason *season_data = nullptr;

		season_data = metadata.GetSeason(season);

		if (season_data != nullptr && episode > 0)
		{
			season_data->should_render = true;
			episode_data = season_data->GetEpisode(episode);
		}

		// Orphan episode
		if (episode_data == nullptr)
		{
			TVShowEpisode orphan;
			orphan.name = entry_filename;
			orphan.filename = entry_filename;
			orphan.file_path = entry_file_path;
			orphan.watched = MetadataCache::IsWatched(entry_filename);
			metadata.orphan_episodes.push_back(orphan);
		}
		else
		{
			episode_data->filename = entry_filename;
			episode_data->file_path = entry_file_path;
			episode_data->watched = MetadataCache::IsWatched(entry_filename);
		}
	}

	(*data)[data_index] = metadata;
}

void Library::ParseSeasonAndEpisodeFromFilename(std::string filename, int *out_season, int *out_episode)
{
	std::string episode_string;
	std::string season_string;

	int episode_start_index = -1;
	int episode_end_index = -1;
	int season_start_index = -1;

	bool in_episode_number = false;

	for (uint32_t i = 0; i < filename.size() - 2; i++)
	{
		bool exists_digits_ahead =
			filename[i + 1] >= 0 && filename[i + 2] >= 0 &&
			isdigit(filename[i + 1]) && isdigit(filename[i + 2]);

		if (in_episode_number && (filename[i] == ' ' || filename[i] == '.'))
		{
			episode_end_index = i;
			break;
		}

		if ((filename[i] == 's' || filename[i] == 'S') && exists_digits_ahead)
		{
			season_start_index = i;
			continue;
		}

		if ((filename[i] == 'e' || filename[i] == 'E') && exists_digits_ahead)
		{
			episode_start_index = i;
			in_episode_number = true;
			continue;
		}
	}

	if (episode_start_index == 0 || episode_end_index == 0)
		return;

	if (season_start_index > 0)
	{
		std::string season = filename.substr(season_start_index + 1, episode_start_index - season_start_index - 1);

		sscanf_s(season.c_str(), "%d", out_season);
	}

	std::string episode = filename.substr(episode_start_index + 1, episode_end_index - episode_start_index - 1);
	sscanf_s(episode.c_str(), "%d", out_episode);
}

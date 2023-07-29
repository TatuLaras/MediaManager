#pragma once
#include "Metadata.h"
#include "FsHelpers.h"
#include "Config.h"
#include "TMDB.h"
#include "PathManager.h"

namespace MetadataCache
{
	void UpdateID(unsigned int new_id, std::string metadata_identifier, bool tv_show = false);
	bool TryGet(std::string metadata_identifier, Metadata* out_metadata);
	void Cache(std::string metadata_identifier, Metadata* metadata);
	std::filesystem::path GetFilePath(std::string metadata_identifier);
	void SetWatched(std::string metadata_identifier, bool watched);
	void Watch(std::string metadata_identifier);
	void UnWatch(std::string metadata_identifier);
	bool IsWatched(std::string metadata_identifier);
	void Hide(std::string metadata_identifier);
	bool IsHidden(std::string metadata_identifier);

	void UnHideAll();
	
	bool FileContainsLine(std::string line, std::string haystack_filepath);
	void AddLineToFile(std::string line, std::string filepath);
}

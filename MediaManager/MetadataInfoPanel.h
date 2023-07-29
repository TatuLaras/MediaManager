#pragma once
#include "ImageInfoPanel.h"

class MetadataInfoPanel :
    public ImageInfoPanel
{
public:
    MetadataInfoPanel(MovieMetadata metadata);
    MetadataInfoPanel(TVShowMetadata metadata);
    MetadataInfoPanel(TVShowSeason metadata);
    MetadataInfoPanel(TVShowEpisode metadata);
    virtual void Render() override;

private:
    std::string title = "";
    std::string under_title_info = "";
    std::string formatted_vote_average = "";
    std::string overview = "";
};


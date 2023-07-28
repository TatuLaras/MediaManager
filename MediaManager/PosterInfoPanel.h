#pragma once
#include "InfoPanel.h"
class PosterInfoPanel :
    public InfoPanel
{
public:
    PosterInfoPanel(MovieMetadata metadata);
    PosterInfoPanel(TVShowMetadata metadata);
    PosterInfoPanel(TVShowSeason metadata);
    PosterInfoPanel(TVShowEpisode metadata);
    virtual void Render() override;

private:
    std::string title = "";
    std::string under_title_info = "";
    std::string formatted_vote_average = "";
    std::string overview = "";
};


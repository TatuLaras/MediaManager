#include "pch.h"
#include "PosterInfoPanel.h"
#define mm_SPACE "  -  "

PosterInfoPanel::PosterInfoPanel(MovieMetadata metadata)
{
    title = metadata.title;
    overview = metadata.overview;
    under_title_info = metadata.GetFormattedRuntime() + mm_SPACE + metadata.release_date + mm_SPACE + metadata.GetFormattedGenres();
    formatted_vote_average = metadata.GetFormattedVoteAverage();
}

PosterInfoPanel::PosterInfoPanel(TVShowMetadata metadata)
{
    title = metadata.title;
    overview = metadata.overview;
    under_title_info = metadata.GetFormattedRuntime() + mm_SPACE + metadata.GetFormattedGenres();
    formatted_vote_average = metadata.GetFormattedVoteAverage();
}

PosterInfoPanel::PosterInfoPanel(TVShowSeason metadata)
{
    title = metadata.name;
    overview = metadata.overview;
    under_title_info = std::to_string(metadata.episode_count) + " episodes";
    formatted_vote_average = metadata.GetFormattedVoteAverage();
}

PosterInfoPanel::PosterInfoPanel(TVShowEpisode metadata)
{
    title = metadata.name;
    overview = metadata.overview;
    under_title_info = std::string("Season ") + std::to_string(metadata.season_number) + std::string(" episode ") + std::to_string(metadata.episode_number) + mm_SPACE + metadata.GetFormattedRuntime() + mm_SPACE + metadata.air_date;
    formatted_vote_average = metadata.GetFormattedVoteAverage();
}

void PosterInfoPanel::Render()
{
    ImGui::BeginChild("info_panel");
    
    ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + tl_InfoPanel_WRAP_WIDTH);
    ImGui::SeparatorText(title.c_str());

    ImGui::TextDisabled(under_title_info.c_str());
    ImGui::Text(formatted_vote_average.c_str());


    ImGui::Dummy(ImVec2(0, 10));

    RenderImage();

    ImGui::Dummy(ImVec2(0, 10));
    ImGui::Text(overview.c_str());
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::PopTextWrapPos();

    ImGui::EndChild();
}



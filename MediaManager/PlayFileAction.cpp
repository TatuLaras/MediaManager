#include "pch.h"
#include "PlayFileAction.h"

void PlayFileAction::Execute(std::wstring command) {
    command = L"\"" + command + L"\"";

#ifdef mm_def_PLATFORM_WINDOWS
    _wsystem(command.c_str());
#else
    system(command.c_str());
#endif
}


void PlayFileAction::Perform()
{
    // Runs a video player in a separate detached thread

    std::filesystem::path file_path_to_play = FsHelpers::ToPath(file_path);

    std::wstring base_command = std::wstring(Config::video_player_command.begin(), Config::video_player_command.end());
    std::wstring full_command = base_command + L" \"" + file_path_to_play.native() + L"\"";

    std::thread worker(&PlayFileAction::Execute, full_command);
    worker.detach();
}

#include "pch.h"
#include "PlayFileAction.h"

void PlayFileAction::Execute(std::wstring command) {
    // Wrap in extra quotes, just a quirk of the system() function
    command = L"\"" + command + L"\"";

    _wsystem(command.c_str());
}

/// <summary>
/// Runs a video player in a separate detached thread
/// </summary>
void PlayFileAction::Perform()
{
    std::filesystem::path file_path_to_play = FsHelpers::ToPath(file_path);

    std::wstring base_command = std::wstring(Config::video_player_command.begin(), Config::video_player_command.end());
    std::wstring full_command = base_command + L" \"" + file_path_to_play.native() + L"\"";

    std::thread worker(&PlayFileAction::Execute, full_command);
    worker.detach();
}

#include "pch.h"
#include "PlayFileAction.h"


#ifdef mm_def_PLATFORM_WINDOWS
void PlayFileAction::Execute(std::wstring command) {
    command = L"\"" + command + L"\"";
    _wsystem(command.c_str());
}
#else
void PlayFileAction::Execute(std::string command) {
    // command = "\"" + command + "\"";
    system(command.c_str());
}
#endif


void PlayFileAction::Perform()
{
    // Runs a video player in a separate detached thread

    std::filesystem::path file_path_to_play = FsHelpers::ToPath(file_path);
#ifdef mm_def_PLATFORM_WINDOWS
    std::wstring base_command = std::wstring(Config::video_player_command.begin(), Config::video_player_command.end());
    std::wstring full_command = base_command + L" \"" + file_path_to_play.native() + L"\"";
#else 
    std::string full_command = Config::video_player_command + " \"" + file_path_to_play.native() + "\""; 
#endif
    std::thread worker(&PlayFileAction::Execute, full_command);
    worker.detach();
}

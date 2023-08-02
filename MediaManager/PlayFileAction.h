#pragma once
#include "MenuAction.h"
#include "FsHelpers.h"
#include "Config.h"

class PlayFileAction :
    public MenuAction
{
public:
    PlayFileAction(std::string playable_file_path): file_path(playable_file_path) {}

#ifdef mm_def_PLATFORM_WINDOWS
    static void Execute(std::wstring command);
#else
    static void Execute(std::string command);
#endif

private:
    virtual void Perform() override;
    std::string file_path = "";
};


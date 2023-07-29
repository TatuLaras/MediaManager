#pragma once
#include "MenuAction.h"
#include "FsHelpers.h"
#include "Config.h"

class PlayFileAction :
    public MenuAction
{
public:
    PlayFileAction(std::string playable_file_path): file_path(playable_file_path) {}
    static void Execute(std::wstring command);


private:
    virtual void Perform() override;
    std::string file_path = "";
};


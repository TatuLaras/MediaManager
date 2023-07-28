#pragma once
#include "InfoPanel.h"

class PlaceholderInfoPanel :
    public InfoPanel
{
public:
    PlaceholderInfoPanel(std::string info_text = "No info to display.") 
        : label(info_text) {}

private:
    virtual void Render() override;
    std::string label;
};


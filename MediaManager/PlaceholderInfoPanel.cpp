#include "pch.h"
#include "PlaceholderInfoPanel.h"

void PlaceholderInfoPanel::Render()
{
    ImGui::BeginChild("info_panel");

    ImGui::TextDisabled(label.c_str());
    
    ImGui::EndChild();
}

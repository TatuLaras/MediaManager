#pragma once
#include "MenuAction.h"
#include "imgui.h"
#include "MathHelper.h"
#include "Metadata.h"
#include "MetadataCache.h"
#include "InfoPanel.h"

enum class MenuItemRole {
    Navigation, 
    TVSeason,
    TVShow,
    Watchable
};

/// <summary>
/// Data structure for one UI menu item
/// </summary>
class MenuItem {
public:
    std::string label = "";
    std::string sublabel = "";
    /// <summary>
    /// All menu items "under" this one in the menu tree
    /// </summary>
    std::vector<MenuItem*> subitems;
    int selected_subitem_index = 0;
    MenuAction* action = nullptr;
    bool selected;
    bool watched;
    bool actively_selected;
    MenuItemRole role = MenuItemRole::Navigation;
    std::string filename = "";
    std::string metadata_identifier = "";
    InfoPanel* infopanel = nullptr;

    MenuItem(std::string label);
    MenuItem(std::string label, std::string sublabel);
    ~MenuItem();
    MenuItem* AddItem(MenuItem* item);
    MenuItem* AddItem(std::string item_label);
    MenuItem* AddItem(std::string item_label, std::string sublabel);
    void IndexToBounds();
    void NextSubitem();
    void PreviousSubitem();
    /// <summary>
    /// Render the table cell for this MenuItem
    /// </summary>
    void Render();
    void RenderCheckmark(int spacing_top = 0);
    void RenderInfoPanel() {
        if(infopanel != nullptr)
            infopanel->Render();
    }

    bool PerformAction() {
        if (action != nullptr) {
            action->Perform();
            return true;
        }

        return false;
    }

    bool GetWatched() {
        if (role == MenuItemRole::Watchable) return watched;
        if (role == MenuItemRole::Navigation) return false;

        int sum = 0;
        for (int i = 0; i < subitems.size(); i++) 
            sum += (int)subitems[i]->GetWatched();
        
        return sum == subitems.size();
    }

    void SetWatched(bool b) {
        if (role == MenuItemRole::Navigation) return;
        if (role == MenuItemRole::Watchable) {
            if(watched != b)
                MetadataCache::SetWatched(filename, b);

            watched = b;
            return;
        }

        // Else set all subitems
        for (int i = 0; i < subitems.size(); i++)
            subitems[i]->SetWatched(b);
    }

    void ToggleWatched() {
        SetWatched(!GetWatched());
    }

    void Hide() {
        if (metadata_identifier.size() > 0)
            MetadataCache::Hide(metadata_identifier);
    }

    MenuItem* SelectedSubitem();
    void DeleteSelectedSubitem();

    MenuItem* SelectedSubitem(int depth);

    MenuItem* Last();

    int MaxTreeWidth();

    std::string GetRowLabel(int index);

    MenuItem* GetRowItem(int index);
    int MaxTreeDepth();

private:
    std::string Truncate(std::string label);
};


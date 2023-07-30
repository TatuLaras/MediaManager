#pragma once
#include "MenuAction.h"
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

    MenuItemRole role = MenuItemRole::Navigation;
    bool selected;
    bool watched;
    bool actively_selected;

    std::string filename = "";
    std::string metadata_identifier = "";
    
    InfoPanel* infopanel = nullptr;
    MenuAction* action = nullptr;


    MenuItem(std::string label);
    MenuItem(std::string label, std::string sublabel);
    ~MenuItem();
    MenuItem* AddItem(std::string item_label);
    MenuItem* AddItem(std::string item_label, std::string sublabel);
    
    /// <summary>
    /// Navigates to the next subitem inside this one in the menu tree
    /// </summary>
    void NextSubitem();
    /// <summary>
    /// Navigates to the previous subitem inside this one in the menu tree
    /// </summary>
    void PreviousSubitem();

    /// <summary>
    /// Render the table cell for this MenuItem
    /// </summary>
    void Render();


    void RenderInfoPanel() {
        if(infopanel) infopanel->Render();
    }

    bool PerformMenuAction() {
        if (action) {
            action->Perform();
            return true;
        }

        return false;
    }

    bool IsWatched();


    void SetWatched(bool b);

    void ToggleWatched() {
        SetWatched(!IsWatched());
    }

    void Hide() {
        if (metadata_identifier.size() > 0)
            MetadataCache::Hide(metadata_identifier);
    }

    MenuItem* SelectedSubitem();
    MenuItem* SelectedSubitem(int depth);
    /// <summary>
    /// Returns a pointer to the last added subitem. 
    /// Only call immediately after AddItem as a shortcut.
    /// Not thread-safe.
    /// </summary>
    MenuItem* Last();
    void DeleteSelectedSubitem();

    int MaxTreeWidth();
    int MaxTreeDepth();

    MenuItem* GetRowItem(int index);

private:
    std::string Truncate(std::string label);
    void IndexToBounds();
    bool SubItemsAreWatched();

    void RenderCheckmark(float* out_checkmark_width = nullptr);
    
    void RenderBackground();
    void RenderLabels(float margin_left);
};


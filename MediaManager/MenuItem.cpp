#include "pch.h"
#include "MenuItem.h"

MenuItem::MenuItem(std::string item_label) {
    label = item_label;
}
MenuItem::~MenuItem()
{
    for (int i = 0; i < subitems.size(); i++) 
        delete subitems[i];

    delete action;
    delete infopanel;
}

MenuItem* MenuItem::AddItem(MenuItem* item)
{
    subitems.push_back(item);
    return item;
}

MenuItem* MenuItem::AddItem(const char* item_label)
{
    return AddItem(std::string(item_label));
}

MenuItem* MenuItem::AddItem(std::string item_label)
{
    MenuItem* menuItem = new MenuItem(item_label);
    subitems.push_back(menuItem);
    return menuItem;
}

void MenuItem::IndexToBounds() {
    selected_subitem_index = 
        MathHelpers::Wrap(selected_subitem_index, 0, subitems.size() - 1);
}

void MenuItem::NextSubitem()
{
    selected_subitem_index++;
    IndexToBounds();
}

void MenuItem::PreviousSubitem()
{
    selected_subitem_index--;
    IndexToBounds();
}

void MenuItem::Render()
{
    if(GetWatched())
        RenderCheckmark(4);

    if (role != MenuItemRole::Navigation)
        ImGui::Indent();

    ImGui::Text(((role != MenuItemRole::Navigation ? "" : "") + label).c_str());

    if (role != MenuItemRole::Navigation)
        ImGui::Unindent();

    ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0, 0, 0, 0));

    if (selected) cell_bg_color = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.6f, 0.65f));
    if (actively_selected) cell_bg_color = ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.8f, 0.65f));
    ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
}

void MenuItem::RenderCheckmark(int spacing_top)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    const ImVec2 cur = ImGui::GetCursorScreenPos();

    const ImVec2 p1 = ImVec2(0, 6 + spacing_top);
    const ImVec2 p2 = ImVec2(4, 10 + spacing_top);
    const ImVec2 p3 = ImVec2(12, 2 + spacing_top);

    draw_list->AddLine(ImVec2(cur.x + p1.x, cur.y + p1.y), ImVec2(cur.x + p2.x, cur.y + p2.y), ImColor(ImVec4(1, 1, 1, 1)), 2.0f);
    draw_list->AddLine(ImVec2(cur.x + p2.x, cur.y + p2.y), ImVec2(cur.x + p3.x, cur.y + p3.y), ImColor(ImVec4(1, 1, 1, 1)), 2.0f);
}

MenuItem* MenuItem::SelectedSubitem() {

    IndexToBounds();
    if (subitems.size() <= selected_subitem_index) return nullptr;
    return subitems[selected_subitem_index];
}

void MenuItem::DeleteSelectedSubitem()
{
    IndexToBounds();
    if (subitems.size() <= selected_subitem_index) return;
    MenuItem* current = subitems[selected_subitem_index];
    subitems.erase(subitems.begin() + selected_subitem_index);
    delete current;

}

MenuItem* MenuItem::SelectedSubitem(int depth) {
    if (subitems.size() == 0) return nullptr;

    MenuItem* currentItem = this;

    for (int i = 0; i < depth; i++) {
        MenuItem* candidate = currentItem->SelectedSubitem();
        if (candidate != nullptr) currentItem = candidate;
    }

    return currentItem;
}

/// <summary>
/// Returns a pointer to the last added subitem. Only call immediately after AddItem as a shortcut.
/// </summary>
MenuItem* MenuItem::Last() {
    if (subitems.size() == 0) return nullptr;
    return subitems[subitems.size() - 1];
}

int MenuItem::MaxTreeWidth() {
    int maxWidth = subitems.size();

    MenuItem* currentItem = SelectedSubitem();

    while (currentItem != nullptr)
    {
        maxWidth = MathHelpers::Max(currentItem->subitems.size(), maxWidth);
        currentItem = currentItem->SelectedSubitem();
    }

    return maxWidth;
}

int MenuItem::MaxTreeDepth() {
    int maxDepth = 0;

    MenuItem* currentItem = SelectedSubitem();

    while (currentItem != nullptr)
    {
        maxDepth++;
        currentItem = currentItem->SelectedSubitem();
    }

    return maxDepth;
}

std::string MenuItem::GetRowLabel(int index) {
    if (index >= subitems.size() || index < 0) return "";
    return subitems[index]->label;
}

MenuItem* MenuItem::GetRowItem(int index) {
    if (index >= subitems.size() || index < 0) return nullptr;
    return subitems[index];
}


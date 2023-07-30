#include "pch.h"
#include "MenuItem.h"

MenuItem::MenuItem(std::string item_label) {
	label = item_label;
}

MenuItem::MenuItem(std::string item_label, std::string item_sublabel)
{
	label = item_label;
	sublabel = item_sublabel;
}

MenuItem::~MenuItem()
{
	for (int i = 0; i < subitems.size(); i++)
		delete subitems[i];

	delete action;
	delete infopanel;
}



MenuItem* MenuItem::AddItem(std::string item_label)
{
	MenuItem* menuItem = new MenuItem(item_label);
	subitems.push_back(menuItem);
	return menuItem;
}

MenuItem* MenuItem::AddItem(std::string item_label, std::string sublabel)
{
	MenuItem* menuItem = new MenuItem(item_label, sublabel);
	subitems.push_back(menuItem);
	return menuItem;
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

	bool indent = role != MenuItemRole::Navigation;
	float space_for_checkmark = 0;

	if (indent)
		RenderCheckmark(&space_for_checkmark);

	float extra_padding = ImGui::CalcTextSize("_.").x;
	float label_margin_left = space_for_checkmark + extra_padding;

	RenderLabels(label_margin_left);

	RenderBackground();
}




bool MenuItem::IsWatched()
{
	if (role == MenuItemRole::Watchable) return watched;
	if (role == MenuItemRole::Navigation) return false;

	return SubItemsAreWatched();
}



void MenuItem::SetWatched(bool b)
{
	if (role == MenuItemRole::Navigation) return;
	if (role == MenuItemRole::Watchable) {
		if (watched != b)
			MetadataCache::SetWatched(filename, b);

		watched = b;
		return;
	}

	// Else set all subitems
	for (int i = 0; i < subitems.size(); i++)
		subitems[i]->SetWatched(b);
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

MenuItem* MenuItem::GetRowItem(int index) {
	if (index >= subitems.size() || index < 0) return nullptr;
	return subitems[index];
}







std::string MenuItem::Truncate(std::string label)
{
	if (label.size() > Config::label_max_length)
		return label.substr(0, Config::label_max_length) + "...";

	return label;
}

void MenuItem::IndexToBounds() {
	selected_subitem_index =
		MathHelpers::Repeat(selected_subitem_index, 0, subitems.size() - 1);
}

bool MenuItem::SubItemsAreWatched()
{
	int sum = 0;
	for (int i = 0; i < subitems.size(); i++)
		sum += (int)subitems[i]->IsWatched();

	return sum == subitems.size();
}

void MenuItem::RenderCheckmark(float* out_checkmark_width)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const ImVec2 cur = ImGui::GetCursorScreenPos();

	// Units
	float line_height_with_spacing = ImGui::GetTextLineHeightWithSpacing();
	float line_height_inner = ImGui::GetTextLineHeight();

	float unit = line_height_inner * 0.1;
	float midpoint = line_height_with_spacing * 0.5;
	float baseline = midpoint + unit * 2;
	float top_line = midpoint - unit * 3;

	float x_start = 0;
	float x_mid = x_start + baseline - midpoint;
	float x_end = x_mid + baseline - top_line;

	if (out_checkmark_width) *out_checkmark_width = x_end;

	if (!IsWatched()) return;

	const ImVec2 p1 = ImVec2(x_start, midpoint);
	const ImVec2 p2 = ImVec2(x_mid, baseline);
	const ImVec2 p3 = ImVec2(x_end, top_line);

	draw_list->AddLine(ImVec2(cur.x + p1.x, cur.y + p1.y), ImVec2(cur.x + p2.x, cur.y + p2.y), ImColor(ImVec4(1, 1, 1, 1)), 2.0f);
	draw_list->AddLine(ImVec2(cur.x + p2.x, cur.y + p2.y), ImVec2(cur.x + p3.x, cur.y + p3.y), ImColor(ImVec4(1, 1, 1, 1)), 2.0f);

}


void MenuItem::RenderBackground()
{
	ImU32 cell_bg_color = ImGui::GetColorU32(ImVec4(0, 0, 0, 0));

	if (selected) cell_bg_color = ImGui::GetColorU32(ImVec4(0.2f, 0.2f, 0.6f, 0.65f));
	if (actively_selected) cell_bg_color = ImGui::GetColorU32(ImVec4(0.4f, 0.4f, 0.8f, 0.65f));

	ImGui::TableSetBgColor(ImGuiTableBgTarget_CellBg, cell_bg_color);
}

void MenuItem::RenderLabels(float margin_left)
{
	// Margin
	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + margin_left);

	if (sublabel.size() > 0 && Config::show_sublabels) {
		ImGui::TextDisabled(sublabel.c_str());
		ImGui::SameLine();
	}

	ImGui::Text(Truncate(label).c_str());

}

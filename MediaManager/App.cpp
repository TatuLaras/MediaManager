#include "pch.h"
#include "App.h"

namespace MediaManager
{
	// App state
	MenuItem* root_item;
	static int tree_depth = 0;
	static int maxDepth;
	static bool main_window_focused = true;

	// Input field buffers
	static char tmdb_key[256];
	static char movie_folder[256];
	static char tv_folder[256];
	static char video_player_command[256];
	static unsigned int tmdb_id;

	// Window and modal is open states
	static bool show_options = false;
	static bool show_options_last_frame = false;
	static bool open_custom_search = false;
	static bool open_no_player_modal = false;

	// Quirks of imgui
	const ImU8 u8_one = 1;
	const ImU16 u16_one = 1;


	// ----- These are called from main.cpp -----

	void Init() {
		ScanLibrary();
		LoadFonts();

		open_no_player_modal = Config::video_player_command.size() == 0;

		if (_heapchk() != _HEAPOK)
			DebugBreak();
	}

	void Terminate() {
		Library::JoinThreads();
		Config::SaveConfigToDisk();
		if(root_item)
			delete root_item;

		if (_heapchk() != _HEAPOK)
			DebugBreak();
	}

	void Update() {

		maxDepth = root_item->MaxTreeDepth();

		RenderUI();

		if (main_window_focused) HandleKeyboardInput();

		if (show_options_last_frame && !show_options) ImGui::SetWindowFocus("main");
		show_options_last_frame = show_options;
	}

	// --------------------------






	void ScanLibrary() {
		if (!Config::IsInitialized()) InitConfig();
		root_item = new MenuItem("Root");
		Library::StartGeneratingMenuTree(root_item);
	}

	void LoadFonts() {
		if (!Config::IsInitialized()) InitConfig();
		ImGuiIO& io = ImGui::GetIO();
		std::string font_path = PathManager::GetBaseResourceFolder() + mm_SLASH + mm_FONT_USED;

		if (FsHelpers::PathExists(font_path))
			io.Fonts->AddFontFromFileTTF(font_path.c_str(), Config::font_size, nullptr, io.Fonts->GetGlyphRangesJapanese());
	}

	void InitConfig() {
		Config::Init(PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + mm_SLASH + "config");
		Config::LoadConfigFromDisk();

		// Config values to text fields
		strcpy_s(tmdb_key, Config::tmdb_key.c_str());
		strcpy_s(movie_folder, Config::movie_folder.c_str());
		strcpy_s(tv_folder, Config::tv_folder.c_str());
		strcpy_s(video_player_command, Config::video_player_command.c_str());
	}






	void RenderUI()
	{
		RenderMainMenuBar();

		static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoBringToFrontOnFocus;

		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);

		if (ImGui::Begin("main", nullptr, flags))
		{
			main_window_focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);

			RenderMenuTable();
			ImGui::SameLine();
			RenderInfoPanel();
		}

		ImGui::End();

		RenderPopups();
		RenderCustomIdBindingModal();

		if (!Library::movie_thread_done || !Library::tv_thread_done)
			RenderLoadingOverlay();

		if (show_options) RenderOptionsWindow(&show_options);

	}



	void RenderMainMenuBar() {
		MenuItem* current = GetCurrent();
		if (!current) return;

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				ImGui::MenuItem("Options", "F1", &show_options);
				ImGui::EndMenu();
			}


			if (ImGui::BeginMenu("Library"))
			{
				if (ImGui::MenuItem("Refresh", "F5"))
					RefreshLibrary();

				if (ImGui::MenuItem("Unhide all hidden items", "Alt+H"))
					UnHideAll();

				if (ImGui::MenuItem("Empty metadata cache"))
					EmptyMetadataCache();

				if (ImGui::MenuItem("Empty image cache"))
					EmptyImageCache();

				ImGui::EndMenu();
			}

			if (current->role != MenuItemRole::Navigation && ImGui::BeginMenu("Item")) {
				const char* watched_label = current->IsWatched() ? "Mark as unwatched" : "Mark as watched";
				if (ImGui::MenuItem(watched_label, "Ctrl+W"))
					current->ToggleWatched();

				if ((CurrentIsMovie() || CurrentIsTVShow())) {
					if (ImGui::MenuItem("Search metadata by TMDB ID", "Ctrl+B"))
						open_custom_search = true;

					if (ImGui::MenuItem("Hide", "Ctrl+H"))
						HideCurrent();
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}



	void RenderMenuTable() {
		// Style settings
		ImGuiTableFlags table_flags =
			ImGuiTableFlags_BordersOuter |
			ImGuiTableFlags_BordersV |
			ImGuiTableFlags_ContextMenuInBody |
			ImGuiTableFlags_NoHostExtendX |
			ImGuiTableFlags_SizingFixedFit
			;

		ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10, 3));
		// New table
		if (!ImGui::BeginTable("table", maxDepth, table_flags, ImVec2(-FLT_MIN, -FLT_MIN))) {
			ImGui::PopStyleVar();
			return;
		}

		// Loop over rows and columns
		for (int row = 0; row < root_item->MaxTreeWidth(); row++)
		{
			ImGui::TableNextRow();

			for (int col = 0; col < maxDepth; col++) {
				ImGui::TableSetColumnIndex(col);

				// Corresponding menu item to the cell we're currently defining
				MenuItem* current = root_item->SelectedSubitem(col)->GetRowItem(row);

				// Empty cell
				if (current == nullptr) continue;

				// Set params for cell rendering
				current->selected =
					root_item->SelectedSubitem(col)->selected_subitem_index == row;

				current->actively_selected =
					col == tree_depth && current->selected;

				// Render cell
				current->Render();

			}

		}

		ImGui::EndTable();
		ImGui::PopStyleVar();
	}



	void RenderInfoPanel() {

		MenuItem* current = GetCurrent();
		if (current == nullptr) return;

		current->RenderInfoPanel();

	}



	void RenderPopups() {
		if (TMDB::tmdb_general_error_to_handle) {
			TMDB::tmdb_general_error_to_handle = false;
			ImGui::OpenPopup("TMDB error");
		}

		if (TMDB::key_error_to_handle) {
			TMDB::key_error_to_handle = false;
			ImGui::OpenPopup("TMDB key invalid");
		}

		if (open_no_player_modal) {
			open_no_player_modal = false;
			ImGui::OpenPopup("Video player command missing");
		}

		if (ImGui::BeginPopupModal("TMDB error", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			std::string label = TMDB::tmdb_general_error_reason.size() > 0 ?
				TMDB::tmdb_general_error_reason : "Check that you have a valid internet connection and try again.";

			ImGui::Text(label.c_str());
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("TMDB key invalid", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("You have an invalid TMDB API key.\nGo to Menu > Options to change it.\nAfter that, go to Library > Refresh.");
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		if (ImGui::BeginPopupModal("Video player command missing", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("You haven't set a video player command.\nGo to Menu > Options to change it.");
			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}



	void RenderOptionsWindow(bool* open) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking |
			ImGuiWindowFlags_NoCollapse;

		ImGui::SetNextWindowSize(ImVec2(500, 440), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("MediaManager options", open, window_flags))
		{
			ImGui::InputText("TMDB API key", tmdb_key, 256, ImGuiInputTextFlags_AutoSelectAll);

			ImGui::InputText("Movies folder", movie_folder, 256);

			ImGui::InputText("TV show folder", tv_folder, 256);

			ImGui::InputText("Video player command", video_player_command, 256);
			ImGui::TextDisabled("(will be executed as [command] \"[file path]\")");

			ImGui::Text("");

			ImGui::InputScalar("Font size", ImGuiDataType_U8, &Config::font_size, &u8_one);
			ImGui::TextDisabled("(requires restart)");

			ImGui::InputScalar("Menu item label max length", ImGuiDataType_U16, &Config::label_max_length, &u8_one);

			ImGui::Checkbox("Download larger images", &Config::use_large_images);
			ImGui::TextDisabled("(takes up more disk space)");

			ImGui::Checkbox("Show episode numbers", &Config::show_sublabels);

			Config::tmdb_key = std::string(tmdb_key);
			Config::movie_folder = std::string(movie_folder);
			Config::tv_folder = std::string(tv_folder);
			Config::video_player_command = std::string(video_player_command);

		}
		ImGui::End();
	}



	void RenderCustomIdBindingModal() {
		if (open_custom_search) {
			open_custom_search = false;
			if ((CurrentIsMovie() || CurrentIsTVShow()) && GetCurrent()->metadata_identifier.size() > 0)
				ImGui::OpenPopup("Search metadata by TMDB ID");
		}

		if (ImGui::BeginPopupModal("Search metadata by TMDB ID", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputScalar("TMDB ID", ImGuiDataType_U32, &tmdb_id);

			ImGui::SetItemDefaultFocus();
			if (ImGui::Button("OK", ImVec2(120, 0))) {
				MetadataCache::UpdateID(tmdb_id, GetCurrent()->metadata_identifier, CurrentIsTVShow());
				RefreshLibrary();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
	}



	void RenderLoadingOverlay()
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos;
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = work_pos.x + work_size.x - PAD;
		window_pos.y = work_pos.y + PAD;
		window_pos_pivot.x = 1.0f;
		window_pos_pivot.y = 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		ImGui::SetNextWindowViewport(viewport->ID);


		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
			ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
			ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

		if (ImGui::Begin("Loading overlay", nullptr, window_flags))
		{
			ImGui::Text("Loading metadata...");
		}
		ImGui::End();
	}








	void HandleKeyboardInput() {
		MenuItem* inside_menu_item = root_item->SelectedSubitem(tree_depth);
		MenuItem* current = GetCurrent();

		// Hotkeys
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W))
			&& ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
			current->ToggleWatched();
			return;
		}

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F5))) {
			RefreshLibrary();
			return;
		}

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_B))
			&& ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
			open_custom_search = true;
			return;
		}

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_F1)))
			show_options = true;

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_H))
			&& ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftCtrl))) {
			HideCurrent();
			return;
		}

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_H))
			&& ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_LeftAlt))) {
			UnHideAll();
			return;
		}

		// Left / right
		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_RightArrow)) ||
			ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_D)))
			tree_depth++;

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_LeftArrow)) ||
			ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_A)))
			tree_depth--;

		if (ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_Enter))) {
			if (GetCurrent()->PerformMenuAction()) {
				current->SetWatched(true);
				// Move to next item on tv shows
				if (inside_menu_item != nullptr
					&& inside_menu_item->label != "Movies") inside_menu_item->NextSubitem();
			}
			else
				tree_depth++;
		}

		tree_depth = MathHelpers::Clamp(tree_depth, 0, maxDepth - 1);

		// Up / down
		if (inside_menu_item != nullptr) {

			if (inside_menu_item != nullptr && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_UpArrow)) ||
				ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_W)))
				inside_menu_item->PreviousSubitem();

			if (inside_menu_item != nullptr && ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_DownArrow)) ||
				ImGui::IsKeyPressed(ImGui::GetKeyIndex(ImGuiKey_S)))
				inside_menu_item->NextSubitem();
		}

	}







	void HideCurrent() {
		MenuItem* inside_menu_item = root_item->SelectedSubitem(tree_depth);
		MenuItem* current = inside_menu_item->SelectedSubitem();

		current->Hide();
		inside_menu_item->DeleteSelectedSubitem();
	}


	void UnHideAll() {
		MetadataCache::UnHideAll();
		RefreshLibrary();
	}


	// TODO come up with something better
	bool CurrentIsMovie() {
		MenuItem* inside_menu_item = root_item->SelectedSubitem(tree_depth);
		return inside_menu_item->label == "Movies";
	}


	bool CurrentIsTVShow() {
		MenuItem* inside_menu_item = root_item->SelectedSubitem(tree_depth);
		return inside_menu_item->label == "TV Shows";
	}


	MenuItem* GetCurrent() {
		return root_item->SelectedSubitem(tree_depth)->SelectedSubitem();
	}


	void RefreshLibrary() {
		if (!Library::ReadyForScanning()) return;

		MenuItem* old = root_item;
		root_item = new MenuItem("root");
		if(old)
			delete old;
		ScanLibrary();
	}


	void EmptyMetadataCache() {
		FsHelpers::WipeFolder(PathManager::GetBaseDataFolder(mm_SUBFOLDER_METADATA));
		RefreshLibrary();
	}


	void EmptyImageCache() {
		FsHelpers::WipeFolder(PathManager::GetBaseDataFolder(mm_SUBFOLDER_IMAGE));
	}


}

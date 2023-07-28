#include "pch.h"
#include "App.h"

namespace MediaManager
{
    MenuItem* root_item;
    static int tree_depth = 0;
    static int maxDepth;
    
    static char tmdb_key[256];
    static char movie_folder[256];
    static char tv_folder[256];
    static char video_player_command[256];
    static UINT8 font_size;

    static unsigned int tmdb_id;

    static bool show_options = false;
    static bool show_options_last_frame = false;
    static bool open_custom_search = false;

    const ImU8 u8_one = 1;

    void Init() {
        InitConfig();
        ScanLibrary();
        LoadFonts();
    }

    void ScanLibrary() {
        root_item = new MenuItem("Root");
        Library::GenerateMenuTree(root_item);
    }

    void InitConfig() {
        Config::Init(PathManager::GetBaseDataFolder(mm_SUBFOLDER_OTHER) + "\\config");
        Config::Load();

        // Config values to text fields
        strcpy_s(tmdb_key, Config::tmdb_key.c_str());
        strcpy_s(movie_folder, Config::movie_folder.c_str());
        strcpy_s(tv_folder, Config::tv_folder.c_str());
        strcpy_s(video_player_command, Config::video_player_command.c_str());
    }

    void LoadFonts() {
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(mm_FONT_USED, Config::font_size, nullptr, io.Fonts->GetGlyphRangesJapanese());
    }

    void Terminate() {
        Library::JoinThreads();
        Config::Save();
        delete root_item;
    }

    void Update() {

        maxDepth = root_item->MaxTreeDepth();

        RenderMainMenuBar();
        RenderMainLayout();
        RenderPopup();
        RenderCustomIdBindingModal();
        
        if(!Library::movie_thread_done || !Library::tv_thread_done)
            ShowLoadingOverlay();

        if (show_options) RenderOptionsWindow(&show_options);

        if (show_options_last_frame && !show_options) ImGui::SetWindowFocus("main");
        show_options_last_frame = show_options;



        if (_heapchk() != _HEAPOK)
            DebugBreak();
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
                const char* watched_label = current->GetWatched() ? "Mark as unwatched" : "Mark as watched";
                if (ImGui::MenuItem(watched_label, "Ctrl+W"))
                    current->ToggleWatched();
                    
                if ((CurrentIsMovie() || CurrentIsTVShow())) {
                    if(ImGui::MenuItem("Search metadata by TMDB ID", "Ctrl+B"))
                        open_custom_search = true;

                    if (ImGui::MenuItem("Hide", "Ctrl+H"))
                        HideCurrent();
                }

                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
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

    void RenderMainLayout()
    {
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
            
            if(ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
                HandleKeyboardInput();

            RenderMenuTable();
            ImGui::SameLine();
            RenderInfoPanel();
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
            // Perform menu item action if there is one, else navigate the tree as normal
            if (GetCurrent()->PerformAction()) {
                current->SetWatched(true);
                if (inside_menu_item != nullptr) inside_menu_item->NextSubitem();
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
        if (ImGui::BeginTable("table", maxDepth, table_flags, ImVec2(-FLT_MIN, -FLT_MIN)))
        {
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
        }
        ImGui::PopStyleVar();
    }

    void RenderInfoPanel() {

        MenuItem* current = GetCurrent();
        if (current == nullptr) return;

        current->RenderInfoPanel();

    }

    MenuItem* GetCurrent() {
        return root_item->SelectedSubitem(tree_depth)->SelectedSubitem();
    }

    void RefreshLibrary() {
        if (!Library::CanScan()) return;

        MenuItem* old = root_item;
        root_item = new MenuItem("root");
        delete old;
        ScanLibrary();
    }

    void EmptyMetadataCache() {
        Helpers::EmptyFolder(PathManager::GetBaseDataFolder(mm_SUBFOLDER_METADATA));
        RefreshLibrary();
    }
    
    void EmptyImageCache() {
        Helpers::EmptyFolder(PathManager::GetBaseDataFolder(mm_SUBFOLDER_IMAGE));
    }

    void RenderPopup() {
        if (TMDB::tmdb_general_error_to_handle) {
            TMDB::tmdb_general_error_to_handle = false;
            ImGui::OpenPopup("TMDB error");
        }

        if (TMDB::key_error_to_handle) {
            TMDB::key_error_to_handle = false;
            ImGui::OpenPopup("TMDB key invalid");
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

            ImGui::Text("");

            ImGui::InputText("Video player command", video_player_command, 256);
            ImGui::TextDisabled("(will be executed as [command] \"[file path]\")");

            ImGui::Text("");

            ImGui::InputScalar("Font size", ImGuiDataType_U8, &Config::font_size, &u8_one);
            ImGui::TextDisabled("(requires restart)");

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
            if((CurrentIsMovie() || CurrentIsTVShow()) && GetCurrent()->metadata_identifier.size() > 0)
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

    void ShowLoadingOverlay()
    {
        const float PAD = 10.0f;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
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

        ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
        if (ImGui::Begin("Loading overlay", nullptr, window_flags))
        {
            ImGui::Text("Loading metadata...");
        }
        ImGui::End();
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

}

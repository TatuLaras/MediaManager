#pragma once

#include "imgui.h"
#include "MathHelper.h"
#include "MenuItem.h"
#include "TMDB.h"
#include "Library.h"
#include "Config.h"

namespace MediaManager
{
    void Init();
    void InitConfig();
    void ScanLibrary();
    void Terminate();
    void Update();
    void RenderMainMenuBar();
    void LoadFonts();
    void RenderMainLayout();
    void RenderMenuTable();
    void RenderInfoPanel();
    void RenderCustomIdBindingModal();
    void ShowLoadingOverlay();
    void HideCurrent();
    void UnHideAll();
    MenuItem* GetCurrent();
    void RefreshLibrary();
    void RenderOptionsWindow(bool* open);
    void RenderPopup();
    void HandleKeyboardInput();
    bool CurrentIsMovie();
    bool CurrentIsTVShow();
    void EmptyMetadataCache();
    void EmptyImageCache();
}

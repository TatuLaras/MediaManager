#pragma once

#include "imgui.h"
#include "MathHelper.h"
#include "MenuItem.h"
#include "TMDB.h"
#include "Library.h"
#include "Config.h"

/// <summary>
/// Main app
/// </summary>
namespace MediaManager
{
    // Called from main.cpp
    void Init();
    void Update();
    void Terminate();
    //

    void ScanLibrary();
    void LoadFonts();
    void InitConfig();

    void RenderMainMenuBar();
    void RenderUI();
    void RenderMenuTable();
    void RenderInfoPanel();
    void RenderCustomIdBindingModal();
    void RenderLoadingOverlay();
    void RenderOptionsWindow(bool* open);
    void RenderPopup();

    void HandleKeyboardInput();

    bool CurrentIsMovie();
    bool CurrentIsTVShow();
    void EmptyMetadataCache();
    void EmptyImageCache();
    void HideCurrent();
    void UnHideAll();
    MenuItem* GetCurrent();
    void RefreshLibrary();
}

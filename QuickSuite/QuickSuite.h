#pragma once

#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "MapList.h" 

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);

class QuickSuite : public BakkesMod::Plugin::BakkesModPlugin,
    public SettingsWindowBase
{
public:
    bool enabled = false;

    int currentIndex = 0;             // Freeplay
    int currentTrainingIndex = 0;     // Training
    int currentWorkshopIndex = 0;     // Workshop

    bool loadTraining = false;
    bool loadWorkshop = false;
    bool loadFreeplay = false;
    bool returnToPreviousMode = false;
    void LoadWorkshopMaps();

    std::string lastGameMode = "";

    void RenderSettings() override;
    void onLoad() override;
    void onUnload() override;

    void LoadHooks();
    void GameEndedEvent(std::string name);

    void Log(std::string msg);
};

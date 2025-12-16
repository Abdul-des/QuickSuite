#pragma once
#include "GuiBase.h"
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include "bakkesmod/plugin/PluginSettingsWindow.h"
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"
#include "MapList.h"

#include "version.h"
constexpr auto plugin_version = stringify(VERSION_MAJOR) "." stringify(VERSION_MINOR) "." stringify(VERSION_PATCH) "." stringify(VERSION_BUILD);



void SaveTrainingMaps(std::shared_ptr<CVarManagerWrapper> cvarManager, const std::vector<TrainingEntry>& RLTraining);
//void LoadTrainingMaps(std::shared_ptr<CVarManagerWrapper> cvarManager, std::vector<TrainingEntry>& RLTraining);

class QuickSuite : public BakkesMod::Plugin::BakkesModPlugin,
    public SettingsWindowBase

{
public:
  
    struct StatTickerParams {
        // person who got a stat
        uintptr_t Receiver;
        // person who is victim of a stat (only exists for demos afaik)
        uintptr_t Victim;
        // wrapper for the stat event
        uintptr_t StatEvent;
    };

    bool enabled = false;

    //Indexes for current selection
    int currentIndex = 0;
    int currentTrainingIndex = 0;
    int currentWorkshopIndex = 0;

    bool skipPostMatchLoads = false;
    bool stayInLobby = false;
    bool gameWon = false;
    bool loadTraining = false;
    bool loadWorkshop = false;
    bool loadFreeplay = false;
    bool returnToPreviousMode = false;


    void SaveSettings();
    void LoadSettings();

    std::string lastGameMode = "";

    void RenderSettings() override;
    void onLoad() override;
    void onUnload() override;

    void LoadHooks();
    void GameEndedEvent(std::string name);
    void onStatTickerMessage(void* params);
};
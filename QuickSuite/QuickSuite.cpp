#include "pch.h"
#include "QuickSuite.h"
#include "MapList.h"
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>


using namespace std;
using namespace filesystem;

BAKKESMOD_PLUGIN(QuickSuite, "QuickSuite", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool quicksuite_enabled = false;



void QuickSuite::SaveSettings() {
    std::ofstream file("quicksuite_settings.cfg");
    if (!file.is_open()) {
        return;
    }    

    file << (returnToPreviousMode ? 1 : 0) << "\n";
    file << (loadFreeplay ? 1 : 0) << " "
        << (loadTraining ? 1 : 0) << " "
        << (loadWorkshop ? 1 : 0) << "\n";
    file << currentIndex << " "
        << currentTrainingIndex << " "
        << currentWorkshopIndex << "\n";

    file.close();
}

void QuickSuite::LoadSettings() {
    std::ifstream file("quicksuite_settings.cfg");
    if (!file.is_open()) {
        return;
    }

    int wonGame = 0;
    int requeue = 0;
    int freeplay = 0, training = 0, workshop = 0;

    file >> requeue;
    returnToPreviousMode = requeue;
    stayInLobby = wonGame;

    file >> freeplay >> training >> workshop;
    loadFreeplay = freeplay;
    loadTraining = training;
    loadWorkshop = workshop;

    file >> currentIndex >> currentTrainingIndex >> currentWorkshopIndex;

    file.close();
}

void LoadWorkshopMaps(std::vector<WorkshopEntry>& RLWorkshop) {
    RLWorkshop.clear();

    std::filesystem::path modsFolder = "C:\\Program Files\\Epic Games\\rocketleague\\TAGame\\CookedPCConsole\\mods";

    if (!std::filesystem::exists(modsFolder) || !std::filesystem::is_directory(modsFolder)) {
        LOG("Mods folder not found");
        return;
    }

    for (const auto& p : std::filesystem::recursive_directory_iterator(modsFolder)) {
        if (p.is_regular_file() && p.path().extension() == ".upk") {
            WorkshopEntry entry;
            entry.filePath = p.path().string();
            entry.name = p.path().stem().string();
            RLWorkshop.push_back(entry);
        }
    }
}



void SaveTrainingMaps(std::shared_ptr<CVarManagerWrapper> cvarManager, const std::vector<TrainingEntry>& RLTraining) {
    std::string serialized;
    for (const auto& entry : RLTraining) {
        serialized += entry.code + ":" + entry.name + "|";
    }
    cvarManager->getCvar("qs_training_maps").setValue(serialized);
}

void LoadTrainingMapsFromFile(std::vector<TrainingEntry>& RLTraining) {
    RLTraining.clear();
    std::ifstream file("training_maps.txt");
    if (!file.is_open()) {
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t sep = line.find(',');
        if (sep != std::string::npos) {
            RLTraining.push_back({
                line.substr(0, sep),         
                line.substr(sep + 1)          
                });
        }
    }

    file.close();
}


void QuickSuite::onLoad()
{
    _globalCvarManager = cvarManager;
    LOG("QuickSuite loaded!");
	
    cvarManager->registerCvar("quicksuite_enabled", "0", "Enable or disable QuickSuite plugin", true, true, 0, true, 1)
        .addOnValueChanged([this](string oldValue, CVarWrapper cvar) {
        enabled = cvar.getBoolValue();
            });
    cvarManager->registerCvar("qs_training_maps", "", "Stored training maps", true, false, 0, false, 0);

    LoadWorkshopMaps(RLWorkshop);
    LoadTrainingMapsFromFile(RLTraining);
    LoadSettings();

    this->LoadHooks();
}


void QuickSuite::onUnload()
{
	LOG("gtfo");
}


void QuickSuite::LoadHooks()
{
    // Fires at end of match
    gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded",std::bind(&QuickSuite::GameEndedEvent, this, std::placeholders::_1));

    // Detect win via stat ticker 
    gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage",[this](ServerWrapper caller, void* params, std::string eventName) {onStatTickerMessage(params);});

    // Reset flags at start of the next match
    gameWrapper->HookEvent(
        "Function TAGame.GFxHUD_Soccar_TA.HandleReadyToStartGame",[this](std::string) {
            LOG("new match is about to start!");
            skipPostMatchLoads = false;
            gameWon = false;
        }
    );
}



void QuickSuite::onStatTickerMessage(void* params)
{
    StatTickerParams* pStruct = (StatTickerParams*)params;

    StatEventWrapper statEvent = StatEventWrapper(pStruct->StatEvent);
    PriWrapper receiver = PriWrapper(pStruct->Receiver);

    PlayerControllerWrapper pc = gameWrapper->GetPlayerController();
    if (pc.IsNull()) return;

    PriWrapper localPRI = pc.GetPRI();
    if (localPRI.IsNull()) return;

    if (statEvent.GetEventName() == "Win" && receiver.GetUniqueIdWrapper() == localPRI.GetUniqueIdWrapper())
    {
        LOG("PLAYER WON");
        gameWon = true;
        skipPostMatchLoads = true;
    }
}


void QuickSuite::GameEndedEvent(std::string name)
{
    if (!enabled) return;

    if (stayInLobby && skipPostMatchLoads && gameWon) {
        LOG("Skipping all post-match loads, staying in lobby");

        // Optional requeue
        if (returnToPreviousMode) {
            cvarManager->executeCommand("queue");
        }

        return;
    }   
   
    if (loadTraining && !RLTraining.empty()) {
        cvarManager->executeCommand(
            "load_training " + RLTraining[currentTrainingIndex].code);
    }
    
    if (loadWorkshop && !RLWorkshop.empty()) {
        cvarManager->executeCommand("load_workshop \"" + RLWorkshop[currentWorkshopIndex].filePath + "\"");
    }
    
    if (loadFreeplay) {
        cvarManager->executeCommand("load_freeplay " + RLMaps[currentIndex].code);
    }
    
    if (returnToPreviousMode) {
        cvarManager->executeCommand("queue");
    }
}









//{
//	_globalCvarManager = cvarManager;
//	//LOG("Plugin loaded!");
//	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
//	//DEBUGLOG("QuickSuite debug mode enabled");
//
//	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
//	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);
//
//	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
//	//	LOG("Hello notifier!");
//	//}, "", 0);
//
//	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
//	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );
//
//	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
//	//	LOG("the cvar with name: {} changed", cvarName);
//	//	LOG("the new value is: {}", newCvar.getStringValue());
//	//});
//
//	//cvar2.addOnValueChanged(std::bind(&QuickSuite::YourPluginMethod, this, _1, _2));
//
//	// enabled decleared in the header
//	//enabled = std::make_shared<bool>(false);
//	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);
//
//	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
//	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
//	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
//	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&QuickSuite::FUNCTION, this, _1, _2, _3));
//	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));
//
//
//	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
//	//	LOG("Your hook got called and the ball went POOF");
//	//});
//	// You could also use std::bind here
//	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&QuickSuite::YourPluginMethod, this);
//}

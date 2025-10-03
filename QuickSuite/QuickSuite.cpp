#include "pch.h"
#include "QuickSuite.h"
#include "MapList.h"
#include <iostream>
#include <string>
#include <filesystem>

using namespace std;
using namespace filesystem;

BAKKESMOD_PLUGIN(QuickSuite, "QuickSuite", plugin_version, PLUGINTYPE_FREEPLAY)

shared_ptr<CVarManagerWrapper> _globalCvarManager;
bool quicksuite_enabled = false;

void QuickSuite::LoadWorkshopMaps() {
    RLWorkshop.clear();
    string modsFolder = "C:\\Program Files\\Epic Games\\rocketleague\\TAGame\\CookedPCConsole\\mods";

    for (auto& p : filesystem::recursive_directory_iterator(modsFolder)) {
        if (p.path().extension() == ".upk") {
            WorkshopEntry entry;
            entry.filePath = p.path().string();
            entry.name = p.path().stem().string(); // file name without extension
            RLWorkshop.push_back(entry);
        }
    }
}


void QuickSuite::onLoad()
{
    _globalCvarManager = cvarManager;
    LOG("QuickSuite loaded!");
    LoadWorkshopMaps();
    cvarManager->registerCvar("quicksuite_enabled", "0", "Enable or disable QuickSuite plugin", true, true, 0, true, 1)
        .addOnValueChanged([this](string oldValue, CVarWrapper cvar) {
        enabled = cvar.getBoolValue();
            });

    this->LoadHooks();
}


void QuickSuite::onUnload()
{
	LOG("gtfo");
}


void QuickSuite::LoadHooks()
{
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", bind(&QuickSuite::GameEndedEvent, this, placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.AchievementManager_TA.HandleMatchEnded", bind(&QuickSuite::GameEndedEvent, this,placeholders::_1));
}

void QuickSuite::GameEndedEvent(string name)
{
    if (!enabled) return;

    // Load Training
    if (loadTraining && !RLTraining.empty()) {
        cvarManager->executeCommand("load_training " + RLTraining[currentTrainingIndex].code);
        LOG("Loading training map: " + RLTraining[currentTrainingIndex].name);
    }
    else {
        LOG("No Training Maps Detected");
    }

    // Load Workshop
    if (loadWorkshop && !RLWorkshop.empty()) {
        cvarManager->executeCommand("load_workshop \"" + RLWorkshop[currentWorkshopIndex].filePath+ "\"");
        LOG("Loading workshop map: " + RLWorkshop[currentWorkshopIndex].name);
    }
    else {
        LOG("No Training Maps Detected");
    }

    // Load Freeplay

    if(loadFreeplay) {
        cvarManager->executeCommand("load_freeplay " + RLMaps[currentIndex].code);
        LOG("Loading freeplay map: " + RLMaps[currentIndex].name);
    }

    // ReQueue
    if (returnToPreviousMode) {
        cvarManager->executeCommand("queue");
        LOG("Re-queueing into previous playlist.");
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

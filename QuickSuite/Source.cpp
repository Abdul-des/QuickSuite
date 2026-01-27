#include "pch.h"
#include "QuickSuite.h"
#include "MapList.h"
#include <fstream>
#include <sstream>

void QuickSuite::RenderSettings() {
    ImGui::TextUnformatted("QuickSuite Settings");

    // Enable/disable
    CVarWrapper enableCvar = cvarManager->getCvar("quicksuite_enabled");
    if (!enableCvar) return;
    enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable QuickSuite", &enabled)) {
        enableCvar.setValue(enabled);
        SaveSettings();
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();


    if (ImGui::Checkbox("Reque", &returnToPreviousMode)) {
   
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("After loading a Freeplay/Training/Workshop map, you'll be queued back into the mode you were in before.");
    }


    if (ImGui::Checkbox("Win = Stay in Lobby", &stayInLobby)) {
      
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("After winning a game, it keeps you in the lobby, allowing you to team up with you team mate");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Map type selector
    ImGui::TextUnformatted("Select Map Type:");
    if (ImGui::RadioButton("Freeplay", loadFreeplay)) {
        loadTraining = false;
        loadWorkshop = false;
        loadFreeplay = true;
        SaveSettings();
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Training", loadTraining)) {
        loadTraining = true;
        loadWorkshop = false;
        loadFreeplay = false;
        SaveSettings();
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Workshop", loadWorkshop)) {
        loadWorkshop = true;
        loadTraining = false;
        loadFreeplay = false;
        SaveSettings();
    }

    ImGui::Spacing();

    // Freeplay map combo
    if (!loadTraining && !loadWorkshop && ImGui::BeginCombo("Maps", RLMaps[currentIndex].name.c_str())) {
        for (int i = 0; i < RLMaps.size(); i++) {
            bool isSelected = (currentIndex == i);
            if (ImGui::Selectable(RLMaps[i].name.c_str(), isSelected))
                currentIndex = i;
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
        SaveSettings();
    }

    // Training map combo
    if (loadTraining && !RLTraining.empty() && ImGui::BeginCombo("Training Packs", RLTraining[currentTrainingIndex].name.c_str())) {
        for (int i = 0; i < RLTraining.size(); i++) {
            bool isSelected = (currentTrainingIndex == i);
            if (ImGui::Selectable(RLTraining[i].name.c_str(), isSelected))
                currentTrainingIndex = i;
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
        SaveSettings();
    }

    // Workshop map combo
    if (loadWorkshop && !RLWorkshop.empty() &&
        ImGui::BeginCombo("Workshop Maps", RLWorkshop[currentWorkshopIndex].name.c_str()))
    {
        for (int i = 0; i < RLWorkshop.size(); i++) {
            bool isSelected = (currentWorkshopIndex == i);
            if (ImGui::Selectable(RLWorkshop[i].name.c_str(), isSelected))
                currentWorkshopIndex = i;
            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
        SaveSettings();
    }


    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();


    // Add training map
    ImGui::TextUnformatted("Add a Training Map");
    static char newMapCode[32] = "";
    static char newMapName[64] = "";

    ImGui::InputText("Training Map Code", newMapCode, sizeof(newMapCode));
    ImGui::InputText("Training Map Name", newMapName, sizeof(newMapName));

    if (ImGui::Button("Add Training Map")) {
        if (strlen(newMapCode) > 0 && strlen(newMapName) > 0) {
            RLTraining.push_back({ std::string(newMapCode), std::string(newMapName) });

            // Save after adding
            std::ofstream file("training_maps.txt");
            if (file.is_open()) {
                for (const auto& entry : RLTraining) {
                    file << entry.code << "," << entry.name << "\n";
                }
                file.close();
            }

            // Reset input fields
            newMapCode[0] = '\0';
            newMapName[0] = '\0';
        }
        SaveSettings();
    }


    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Adds a custom training map to the list.");
    }

}


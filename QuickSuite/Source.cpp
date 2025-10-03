#include "pch.h"
#include "QuickSuite.h"
#include "MapList.h"

void QuickSuite::RenderSettings() {
    ImGui::TextUnformatted("QuickSuite Settings");

    // Enable/disable
    CVarWrapper enableCvar = cvarManager->getCvar("quicksuite_enabled");
    if (!enableCvar) return;
    enabled = enableCvar.getBoolValue();
    if (ImGui::Checkbox("Enable QuickSuite", &enabled)) {
        enableCvar.setValue(enabled);
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Return to previous mode checkbox
    if (ImGui::Checkbox("Reque", &returnToPreviousMode)) {
   
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("If checked, after loading a Freeplay/Training/Workshop map, you'll be queued back into the mode you were in before.");
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    // Map type selector
    ImGui::TextUnformatted("Select Map Type:");
    if (ImGui::RadioButton("Freeplay", !loadTraining && !loadWorkshop)) {
        loadTraining = false;
        loadWorkshop = false;
        loadFreeplay = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Training", loadTraining)) {
        loadTraining = true;
        loadWorkshop = false;
        loadFreeplay = false;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Workshop", loadWorkshop)) {
        loadWorkshop = true;
        loadTraining = false;
        loadFreeplay = false;
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
            newMapCode[0] = '\0';
            newMapName[0] = '\0';
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::SetTooltip("Adds a custom training map to the list.");
    }
}


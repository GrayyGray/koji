// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "tab.h"
#include "imgui.h"

bool beginTab(const char *label)
{
    if (!ImGui::BeginTabItem(label, nullptr, ImGuiTabItemFlags_NoArrowNav))
        return false;

    ImGui::Spacing();
    ImGui::BeginChild("tabFrame", ImVec2(0, -100), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::Separator();

    return true;
}

void endTab()
{
    ImGui::PopItemFlag();
    ImGui::EndChild();
    ImGui::EndTabItem();
}
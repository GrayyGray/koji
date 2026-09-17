// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "imgui.h"
#include "tabs.h"

namespace koji::frontend::tabs::internal
{
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
} // namespace koji::frontend::tabs::internal

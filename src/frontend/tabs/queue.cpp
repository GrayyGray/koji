// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "tabs.h"
#include "frontend/ui.h"
#include "backend/library.h"
#include "backend/utils.h"
#include "imgui.h"
#include "tab.h"


using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

namespace koji_ui
{

void queueTab(koji_app::AppState &state, koji_player::PlayerStatus &status)
{
    const ImVec4 selected_background_color = koji_utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);
    if (beginTab("Queue"))
    {
        ImGui::BeginTable("queueTable", 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Album");
        ImGui::TableSetupColumn("Duration");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();


        for (int index = 0; index < status.queue.size(); index++)
        {
            ImGui::PushID(index);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(status.queue[index].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                status.current_song = status.queue[index];
                updateCurrentSong(status);
            }

            if (ImGui::BeginPopupContextItem())
            {
                float avalible_width = ImGui::GetContentRegionAvail().x;
                
                if (ImGui::Button("Remove from queue", ImVec2(avalible_width, 0)))
                    status.queue.erase(status.queue.begin() + index);
                
                ImGui::EndPopup();
            }

            if (index < status.queue.size() && status.queue[index] == status.current_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue[index].title.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue[index].album.c_str());
            ImGui::TableNextColumn();
            ImGui::Text("%s", koji_utils::formatTime(status.queue[index].duration).c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
        endTab();
    }
}
} // namespace koji_ui

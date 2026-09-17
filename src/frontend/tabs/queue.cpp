// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "../../backend/app.h"
#include "../../backend/utils/color.h"
#include "../../backend/utils/time.h"
#include "imgui.h"
#include "tabs.h"

using namespace std;
using namespace koji::backend::app;
using namespace koji::backend::library;

namespace koji::frontend::tabs
{
void queueTab(AppState &state)
{
    const ImVec4 selected_background_color = backend::utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);
    if (internal::beginTab("Queue"))
    {
        ImGui::BeginTable("queueTable", 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Album");
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Duration");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int index = 0; index < state.player_context.queue.size(); index++)
        {
            ImGui::PushID(index);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(state.player_context.queue[index].title.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                state.player_context.current_song = state.player_context.queue[index];
                updateCurrentSong(state);
            }

            if (ImGui::BeginPopupContextItem())
            {
                float avalible_width = ImGui::GetContentRegionAvail().x;

                if (ImGui::Button("Remove from queue", ImVec2(avalible_width, 0)))
                    state.player_context.queue.erase(state.player_context.queue.begin() + index);
                if (ImGui::Button("Add to playlist", ImVec2(avalible_width, 0)))
                {
                    state.editor_context.edit_window        = true;
                    state.editor_context.playlist_container = {};
                    state.editor_context.basket_container   = {state.player_context.queue[index]};
                }

                ImGui::EndPopup();
            }

            if (index < state.player_context.queue.size() && state.player_context.queue[index] == state.player_context.current_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));

            ImGui::TableNextColumn();
            ImGui::Text("%s", state.player_context.queue[index].album.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", state.player_context.queue[index].artist.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", backend::utils::formatTime(state.player_context.queue[index].duration).c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
        internal::endTab();
    }
}
} // namespace koji::frontend::tabs

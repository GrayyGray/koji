// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "tabs.h"
#include "frontend/ui.h"
#include "backend/library.h"
#include "backend/utils.h"
#include "imgui.h"


using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

namespace koji_ui
{

void appendToQueueButton(PlayerStatus &status, int index, float button_width)
{
    if (ImGui::Button("Append to queue", ImVec2(button_width, 0)))
    {
        vector<SongEntry> songs;
        songs = getAlbumSongs(status.albums[index]);
        addSongsToQueue(status, songs);
    }
}

void albumTab(AppState &state, PlayerStatus &status)
{
    ImGui::BeginTabItem("Albums", nullptr, ImGuiTabItemFlags_NoArrowNav);
    ImGui::Spacing();
    ImGui::BeginChild("tabFrame", ImVec2(0, -100), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::Separator();

    ImGui::BeginTable("albumTable", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

    ImGui::TableSetupColumn("Artist");
    ImGui::TableSetupColumn("Album");

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
    ImGui::TableHeadersRow();
    ImGui::PopStyleColor();
    ImGui::PopItemFlag();


    for (int index = 0; index < status.albums.size(); index++)
    {
        const ImVec4 selected_background_color = koji_utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);

        ImGui::PushID(index);
        ImGui::TableNextRow();
        ImGui::TableNextColumn();

        if (ImGui::Selectable(status.queue[index].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
        {
            if (!state.io->KeyShift && !status.queue.empty())
                status.queue.clear();
            vector<SongEntry> songs;
            songs = getAlbumSongs(status.albums[index]);
            addSongsToQueue(status, songs);
        }

        if (ImGui::BeginPopupContextItem())
        {
            float avalible_width = ImGui::GetContentRegionAvail().x;
            
            appendToQueueButton(status, index, avalible_width);
            
            ImGui::EndPopup();
        }

        if (index < status.queue.size() && status.queue[index] == status.current_song)
            ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));

        ImGui::TableNextColumn();
        ImGui::Text("%s", status.queue[index].album.c_str());

        ImGui::PopID();
    }

    ImGui::EndTable();
    ImGui::PopItemFlag();
    ImGui::EndChild();
    ImGui::EndTabItem();
}
} // namespace koji_ui

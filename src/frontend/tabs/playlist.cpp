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

namespace koji_frontend
{
void playlistTab(AppState &state, PlayerStatus &status)
{
    if (beginTab("Playlists"))
    {
        ImGui::BeginTable("playlistTable", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

        ImGui::TableSetupColumn("Playlist");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();


        for (int index = 0; index < status.playlists.size(); index++)
        {
            ImGui::PushID(index);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(status.playlists[index].title.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (!state.io->KeyShift && !status.queue.empty())
                    status.queue.clear();
                vector<SongEntry> songs;
                songs = getPlaylistSongs(status.playlists[index]);
                addSongsToQueue(status, songs);
            }

            if (ImGui::BeginPopupContextItem())
            {
                float avalible_width = ImGui::GetContentRegionAvail().x;

                if (ImGui::Button("Append to queue", ImVec2(avalible_width, 0)))
                {
                    vector<SongEntry> songs;
                    songs = getPlaylistSongs(status.playlists[index]);
                    addSongsToQueue(status, songs);
                }
            
                if (ImGui::Button("Edit Playlist", ImVec2(avalible_width, 0)))
                {
                    state.edit_window = true;
                }
                        
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
        endTab();
    }
}
} // namespace koji_frontend

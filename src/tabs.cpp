// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "tabs.h"

#include "imgui.h"
#include "imgui_utils.h"

void songQueueTab(const AppState &state, std::vector<SongEntry> &queue, int &now_playing_index)
{
    const ImVec4 selected_background_color = darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);

    if (ImGui::BeginTable("songQueueTab", 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Album");
        ImGui::TableSetupColumn("Duration");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < queue.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(queue[i].album.artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (queue.size() > 0)
                    now_playing_index = i;
            }

            if (i != -1 && i == now_playing_index)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", queue[i].title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", queue[i].album.album_title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", queue[i].duration.c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void albumSelectionTab(const AppState &state, std::vector<SongEntry> &queue, int &now_playing_index, const std::vector<AlbumEntry> &albums)
{
    if (ImGui::BeginTable("albumSelectionTab", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Album");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < albums.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(albums[i].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (queue.size() == 0 || !state.io->KeyShift)
                {
                    now_playing_index = 0;
                    queue.clear();
                }

                std::vector<SongEntry> album_songs = getAlbumSongs({albums[i]});
                queue.insert(queue.end(), album_songs.begin(), album_songs.end());
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", albums[i].album_title.c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "tabs.h"
#include <algorithm>
#include <format>
#include "imgui.h"
#include "player.h"

ImVec4 darkenColor(const ImVec4 &color, float amount)
{
    float red   = std::max(0.0f, color.x - 0.2f);
    float green = std::max(0.0f, color.y - 0.2f);
    float blue  = std::max(0.0f, color.z - 0.2f);
    return ImVec4(red, green, blue, color.w);
}

void songQueueTab(const AppState &state, koji_player::PlayerStatus &status)
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

        for (int i = 0; i < status.queue.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(status.queue[i].album.artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                status.paused       = false;
                status.current_song = status.queue[i];
            }
            if (status.queue[i] == status.current_song)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue[i].title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue[i].album.album_title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", koji_player::formatTime(status.queue[i].duration).c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void albumSelectionTab(const AppState &state, koji_player::PlayerStatus &status)
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

        for (int i = 0; i < status.albums.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(status.albums[i].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (!state.io->KeyShift && (status.queue.size() > 0))
                {
                    status.paused = false;
                    status.queue.clear();
                }

                std::vector<koji_player::SongEntry> album_songs = koji_player::getAlbumSongs(status.albums[i]);
                if (status.queue.size() == 0)
                {
                    status.queue.insert(status.queue.end(), album_songs.begin(), album_songs.end());
                    status.current_song = status.queue[0];
                }
                else
                    status.queue.insert(status.queue.end(), album_songs.begin(), album_songs.end());
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", status.albums[i].album_title.c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

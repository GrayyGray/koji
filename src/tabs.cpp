// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "tabs.h"

#include "imgui.h"
#include "imgui_utils.h"

void songQueueTab(const AppState &state, PlayerStatus &status)
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

        for (int i = 0; status.queue && i < status.queue->size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(status.queue->at(i).album.artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                status.current_song = status.queue->at(i);
            }
            if (status.queue->at(i) == status.current_song)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue->at(i).title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue->at(i).album.album_title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", formatTime(status.queue->at(i).duration).c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void albumSelectionTab(const AppState &state, PlayerStatus &status)
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

        for (int i = 0; i < status.albums->size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(status.albums->at(i).artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (!state.io->KeyShift && (status.queue && status.queue->size() > 0))
                {
                    status.queue->clear();
                }

                std::optional<std::vector<SongEntry>> album_songs = getAlbumSongs({status.albums->at(i)});
                if (!status.queue)
                    status.queue = std::vector<SongEntry>{};

                if (status.queue->size() == 0)
                {
                    status.queue->insert(status.queue->end(), album_songs->begin(), album_songs->end());
                    status.current_song = status.queue->at(0);
                }
                else if (album_songs)
                    status.queue->insert(status.queue->end(), album_songs->begin(), album_songs->end());
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", status.albums->at(i).album_title.c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

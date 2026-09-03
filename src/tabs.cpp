// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "tabs.h"

#include "imgui.h"

void albumSelectionTab(std::vector<SongEntry>        &queue,
                       const std::vector<AlbumEntry> &albums)
{
    if (ImGui::BeginTable("albumSelectionTab", 2,
                          ImGuiTableFlags_NoSavedSettings |
                              ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Album");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                              ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < albums.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(albums[i].artist.c_str(), false,
                                  ImGuiSelectableFlags_SpanAllColumns))
            {
                printf("%d\n", i);

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

void songQueueTab(std::vector<SongEntry> &queue)
{
    if (ImGui::BeginTable("songQueueTab", 4,
                          ImGuiTableFlags_NoSavedSettings |
                              ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Album");
        ImGui::TableSetupColumn("Length");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                              ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < queue.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(queue[i].album.artist.c_str(), false,
                                  ImGuiSelectableFlags_SpanAllColumns))
            {
                printf("%d\n", i);
                // OnSongClicked(i);
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
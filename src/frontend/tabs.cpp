// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "tabs.h"
#include "../backend/library.h"
#include "../backend/utils.h"
#include "imgui.h"

namespace koji_ui
{
void songQueueTab(const koji_app::AppState &state, koji_player::PlayerStatus &status)
{
    const ImVec4 selected_background_color = koji_utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);

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

            if (ImGui::Selectable(status.queue[i].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                status.paused       = false;
                status.current_song = status.queue[i];
                koji_player::updateCurrentSong(status);
            }

            if (ImGui::BeginPopupContextItem())
            {
                float width = ImGui::GetContentRegionAvail().x;
                if (ImGui::Button("Remove from queue", ImVec2(width, 0)))
                {
                    if (status.queue.size() == i)
                        status.queue.pop_back();
                    else
                        status.queue.erase(status.queue.begin() + i);
                }
                if (ImGui::Button("Add to Playlist", ImVec2(width, 0)))
                {
                    // addToPlaylistPopup()
                }
                ImGui::EndPopup();
            }

            if (status.queue[i] == status.current_song)
            {
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue[i].title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.queue[i].album.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", koji_utils::formatTime(status.queue[i].duration).c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
    }
}

void albumSelectionTab(const koji_app::AppState &state, koji_player::PlayerStatus &status)
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

                std::vector<koji_player::SongEntry> album_songs = koji_library::getAlbumSongs(status.albums[i]);
                koji_player::addSongsToQueue(status, album_songs);
            }

            if (ImGui::BeginPopupContextItem())
            {
                float width = ImGui::GetContentRegionAvail().x;
                if (ImGui::Button("Append to queue", ImVec2(width, 0)))
                {
                    std::vector<koji_player::SongEntry> album_songs = koji_library::getAlbumSongs(status.albums[i]);
                    koji_player::addSongsToQueue(status, album_songs);
                }
                ImGui::EndPopup();
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", status.albums[i].title.c_str());

            ImGui::PopID();

        }
        ImGui::EndTable();
    }
}

void playlistSelectionTab(const koji_app::AppState &state, koji_player::PlayerStatus &status) 
{
    if (ImGui::BeginTable("playlistSelectionTab", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Playlist");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < status.playlists.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            if (ImGui::Selectable(status.playlists[i].title.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (!state.io->KeyShift && (status.queue.size() > 0))
                {
                    status.paused = false;
                    status.queue.clear();
                }

                std::vector<koji_player::SongEntry> playlist_songs = koji_library::getPlaylistSongs(status.playlists[i]);
                koji_player::addSongsToQueue(status, playlist_songs);
            }
            if (ImGui::BeginPopupContextItem())
            {
                float width = ImGui::GetContentRegionAvail().x;
                if (ImGui::Button("Append to queue", ImVec2(width, 0)))
                {
                    std::vector<koji_player::SongEntry> playlist_songs = koji_library::getPlaylistSongs(status.playlists[i]);
                    koji_player::addSongsToQueue(status, playlist_songs);
                }
                if (ImGui::Button("Edit Playlist", ImVec2(width, 0)))
                {
                    // editPlaylistPopup()
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}
} // namespace koji_ui

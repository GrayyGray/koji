// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "../../backend/app.h"
#include "../../backend/library/playlists.h"
#include "../../frontend/compontents/notification.h"
#include "imgui.h"
#include "tabs.h"

using namespace std;
using namespace koji::backend::app;
using namespace koji::backend::library;
using namespace koji::frontend::components::notification;

namespace koji::frontend::tabs
{

void playlistTab(AppState &state)
{
    if (internal::beginTab("Playlists"))
    {
        ImGui::BeginTable("playlistTable", 1, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

        ImGui::TableSetupColumn("Playlist");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int index = 0; index < state.player_context.playlists.size(); index++)
        {
            ImGui::PushID(index);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(state.player_context.playlists[index].title.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (!state.io->KeyShift && !state.player_context.queue.empty())
                    state.player_context.queue.clear();
                vector<SongEntry> songs;
                songs = getPlaylistSongs(state.player_context.playlists[index]);
                addSongsToQueue(state, songs);
            }

            if (ImGui::BeginPopupContextItem())
            {
                const float avalible_width = ImGui::GetContentRegionAvail().x;

                if (ImGui::Button("Append to queue", ImVec2(avalible_width, 0)))
                {
                    vector<SongEntry> songs;
                    songs = getPlaylistSongs(state.player_context.playlists[index]);
                    addSongsToQueue(state, songs);
                }

                if (ImGui::Button("Rename Playlist", ImVec2(avalible_width, 0)))
                {
                    state.editor_context.mode        = EditorMode::Rename;
                    state.editor_context.edit_window = true;
                    state.editor_context.rename      = state.editor_context.playlist.title;
                }

                if (ImGui::Button("Edit Playlist Order", ImVec2(avalible_width, 0)))
                {
                    state.editor_context.mode               = EditorMode::Edit;
                    state.editor_context.playlist           = {state.player_context.playlists[index]};
                    state.editor_context.edit_window        = true;
                    state.editor_context.basket_container   = {};
                    state.editor_context.playlist_container = getPlaylistSongs(state.player_context.playlists[index]);
                }

                if (ImGui::Button("Copy Playlist", ImVec2(avalible_width, 0)))
                {
                    if (!duplicatePlaylist(state.editor_context.playlist))
                        setNotification(state, "Error Copied Playlist Already Exists");
                    else
                        state.player_context.playlists = getPlaylists();

                    state.editor_context.edit_window = false;
                }
                ImGui::EndPopup();
            }

            ImGui::PopID();
        }

        ImGui::EndTable();
        internal::endTab();
    }
}
} // namespace koji::frontend::tabs
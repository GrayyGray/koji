// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "../../backend/app.h"
#include "../../backend/library/albums.h"
#include "imgui.h"
#include "tabs.h"

using namespace std;
using namespace koji::backend::app;
using namespace koji::backend::library;

namespace koji::frontend::tabs
{

void albumTab(AppState &state)
{
    if (internal::beginTab("Albums"))
    {
        ImGui::BeginTable("albumTable", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Album");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int index = 0; index < state.player_context.albums.size(); index++)
        {
            ImGui::PushID(index);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            if (ImGui::Selectable(state.player_context.albums[index].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                if (!state.io->KeyShift && !state.player_context.queue.empty())
                    state.player_context.queue.clear();
                vector<SongEntry> songs;
                songs = getAlbumSongs(state.player_context.albums[index]);
                addSongsToQueue(state, songs);
            }

            if (ImGui::BeginPopupContextItem())
            {
                float avalible_width = ImGui::GetContentRegionAvail().x;

                if (ImGui::Button("Append to queue", ImVec2(avalible_width, 0)))
                {
                    vector<SongEntry> songs;
                    songs = getAlbumSongs(state.player_context.albums[index]);
                    addSongsToQueue(state, songs);
                }
                if (ImGui::Button("Add to playlist", ImVec2(avalible_width, 0)))
                {
                    state.editor_context.edit_window        = true;
                    state.editor_context.playlist = {};
                    state.editor_context.playlist_container = {};
                    state.editor_context.basket_container   = getAlbumSongs(state.player_context.albums[index]);
                }

                ImGui::EndPopup();
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", state.player_context.albums[index].title.c_str());

            ImGui::PopID();
        }

        ImGui::EndTable();
        internal::endTab();
    }
}
} // namespace koji::frontend::tabs

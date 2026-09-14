// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "windows.h"
#include "imgui.h"
#include "backend/library.h"
#include "backend/player.h"
#include "backend/app.h"
#include <stdlib.h>
#include <vector>

using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;



void playlistEditorMenu(AppState &state, PlayerStatus &status)
{
    float horizontal_padding = 3.0f;
    float button_size = 6.0f;

    float table_size_x = ImGui::GetContentRegionAvail().x / 2 - (button_size + horizontal_padding);
    float table_size_y = ImGui::GetContentRegionAvail().y / 2 - (button_size + horizontal_padding);

    if (ImGui::BeginTable("playlist", 1, ImGuiTableFlags_None, ImVec2(table_size_x, table_size_y)))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); 
        for (const SongEntry &song : getPlaylistSongs(status.editor_context.playlist_to_edit))
        {
            ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None);
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("ButtonUp", ImGuiDir_Up)) {}
    if (ImGui::ArrowButton("ButtonDown", ImGuiDir_Down)) {}
    if (ImGui::ArrowButton("ButtonLeft", ImGuiDir_Left)) {}
    if (ImGui::ArrowButton("ButtonRight", ImGuiDir_Right)) {}

}

void selectPlaylistMenu(AppState &state, PlayerStatus &status)
{
    ImGui::Text("Select a playlist to add to:");
    
    ImGui::BeginTable("playlistSelector", 1, ImGuiTableFlags_Borders);
    
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);


    for (int index = 0; index < status.playlists.size(); index++)
    {
        ImGui::PushID(index);
        ImGui::TableNextColumn();

        if (ImGui::Selectable(status.playlists[index].title.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
        {
            status.editor_context.playlist_to_edit = status.playlists[index];
        }

        ImGui::PopID();
    }

    ImGui::EndTable();
}


namespace koji_frontend
{

void editorWindow(AppState &state, PlayerStatus &status)
{
    ImGui::SetNextWindowSize(ImVec2(550, 680));
    ImGui::Begin("Playlist Editor", &state.edit_window, ImGuiWindowFlags_NoCollapse); // ImGuiChildFlags_NavFlattened,
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

    if (status.editor_context.playlist_to_edit == PlaylistEntry{})
    {
        selectPlaylistMenu(state, status);
    }
    else
    {
        playlistEditorMenu(state, status);
    }

    ImGui::PopItemFlag();
    ImGui::PopItemFlag();
    ImGui::End();
}

} // namespace koji_frontend

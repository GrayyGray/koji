// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "windows.h"
#include "imgui.h"
#include "backend/library.h"
#include "backend/player.h"
#include "backend/app.h"
#include <stdlib.h>
#include <vector>
#include <tuple>
using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

void playlistEditorMenu(AppState &state, PlayerStatus &status)
{   
    float button_size = 43.0f;

    float table_size_x = ImGui::GetContentRegionAvail().x / 2 - (button_size);
    float table_size_y = ImGui::GetContentRegionAvail().y;

    ImGui::BeginChild("playlistChild", ImVec2(table_size_x, table_size_y));
    if (ImGui::BeginTable("playlistTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); 
        for (const SongEntry &song : status.editor_context.playlist)
        {
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
                status.editor_context.playlist_selected = song;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("buttons", ImVec2(button_size, table_size_y));
    if (ImGui::ArrowButton("ButtonUp", ImGuiDir_Up)) {}
    if (ImGui::ArrowButton("ButtonDown", ImGuiDir_Down)) {}
    if (ImGui::ArrowButton("ButtonLeft", ImGuiDir_Left)) {}
    if (ImGui::ArrowButton("ButtonRight", ImGuiDir_Right)) {}
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("basketChild", ImVec2(table_size_x, table_size_y));
    if (ImGui::BeginTable("basketTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch); 
        for (const SongEntry &song :status.editor_context.basket)
        {
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
                status.editor_context.basket_selected = song;

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
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
            status.editor_context.playlist_to_edit = status.playlists[index];

        ImGui::PopID();
    }
    ImGui::EndTable();
}


namespace koji_frontend
{

void editorWindow(AppState &state, PlayerStatus &status)
{
    ImGui::SetNextWindowSize(ImVec2(550, 680));
    ImGui::Begin("Playlist Editor", &state.edit_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize); // ImGuiChildFlags_NavFlattened,
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

    if (status.editor_context.playlist_to_edit == PlaylistEntry{})
    {
        selectPlaylistMenu(state, status);
        status.editor_context.playlist = getPlaylistSongs(status.editor_context.playlist_to_edit);
    }
    else
    {
        if (status.editor_context.playlist == vector<SongEntry>{})
            status.editor_context.playlist = getPlaylistSongs(status.editor_context.playlist_to_edit);
        playlistEditorMenu(state, status);
    }
        

    ImGui::PopItemFlag();
    ImGui::PopItemFlag();
    ImGui::End();
}

} // namespace koji_frontend

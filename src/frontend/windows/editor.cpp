// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include <tuple>
#include <vector>
#include <stdlib.h>
#include "backend/app.h"
#include "backend/library.h"
#include "backend/player.h"
#include "backend/utils.h"
#include "imgui.h"
#include "windows.h"

using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

void buttonUp(EditorContext &context)
{
    if (ImGui::ArrowButton("ButtonUp", ImGuiDir_Up))
    {
        vector<SongEntry>::iterator selected;

        if (context.playlist_selected_song != SongEntry{})
        {
            selected = find(context.playlist_container.begin(), context.playlist_container.end(), context.playlist_selected_song);
            if (selected != context.playlist_container.end() && selected != context.playlist_container.begin())
            {
                context.playlist_container.insert(selected - 1, context.playlist_selected_song);
                context.playlist_container.erase(selected);
            }
        }

        if (context.basket_selected_song != SongEntry{})
        {
            selected = find(context.basket_container.begin(), context.basket_container.end(), context.basket_selected_song);

            if (selected != context.basket_container.end() && selected != context.basket_container.begin())
            {
                context.basket_container.insert(selected - 1, context.basket_selected_song);
                context.basket_container.erase(selected);
            }
        }
    }
}

void buttonDown(EditorContext &context)
{
    if (ImGui::ArrowButton("ButtonDown", ImGuiDir_Down))
    {
        vector<SongEntry>::iterator selected;

        if (context.playlist_selected_song != SongEntry{})
        {
            selected = find(context.playlist_container.begin(), context.playlist_container.end(), context.playlist_selected_song);
            if (selected != context.playlist_container.end() && selected + 1 != context.playlist_container.end())
            {
                context.playlist_container.insert(selected + 1, context.playlist_selected_song);
                context.playlist_container.erase(selected);
            }
        }

        if (context.basket_selected_song != SongEntry{})
        {
            selected = find(context.basket_container.begin(), context.basket_container.end(), context.basket_selected_song);

            if (selected != context.basket_container.end() && selected + 1 != context.basket_container.end())
            {
                context.basket_container.insert(selected + 1, context.basket_selected_song);
                context.basket_container.erase(selected);
            }
        }
    }
}

void buttonLeft(EditorContext &context)
{
    if (ImGui::ArrowButton("ButtonLeft", ImGuiDir_Left))
    {
        if (context.playlist_selected_song == SongEntry{} && context.basket_selected_song != SongEntry{})
        {
            vector<SongEntry>::iterator selected = find(context.basket_container.begin(), context.basket_container.end(), context.basket_selected_song);
            if (selected != context.basket_container.end())
            {
                context.playlist_container.push_back(context.basket_selected_song);
                context.basket_container.erase(selected);
                context.basket_selected_song = {};
            }
        }
    }
}

void buttonRight(EditorContext &context)
{
    if (ImGui::ArrowButton("ButtonRight", ImGuiDir_Right))
    {
        if (context.playlist_selected_song != SongEntry{} && context.basket_selected_song == SongEntry{})
        {
            vector<SongEntry>::iterator selected = find(context.playlist_container.begin(), context.playlist_container.end(), context.playlist_selected_song);
            if (context.playlist_container.size() == 0 || selected != context.playlist_container.end())
            {
                context.basket_container.push_back(context.playlist_selected_song);
                context.playlist_selected_song = {};
                context.basket_selected_song = context.basket_container[context.basket_container.size() - 1];
            }
            if (selected != context.playlist_container.end())
                context.playlist_container.erase(selected);
        }
    }
}

void playlistEditorMenu(AppState &state, PlayerStatus &status)
{
    const ImVec4 selected_background_color = koji_utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);
    float        button_size               = 43.0f;

    float table_size_x = ImGui::GetContentRegionAvail().x / 2 - (button_size);
    float table_size_y = ImGui::GetContentRegionAvail().y;

    ImGui::BeginChild("playlistChild", ImVec2(table_size_x, table_size_y));
    if (ImGui::BeginTable("playlistTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const SongEntry &song : status.editor_context.playlist_container)
        {
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
            {
                status.editor_context.basket_selected_song   = {};
                status.editor_context.playlist_selected_song = song;
            }

            if (song == status.editor_context.playlist_selected_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("buttons", ImVec2(button_size, table_size_y));

    buttonUp(status.editor_context);
    buttonDown(status.editor_context);
    buttonLeft(status.editor_context);
    buttonRight(status.editor_context);

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("basketChild", ImVec2(table_size_x, table_size_y));
    if (ImGui::BeginTable("basketTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const SongEntry &song : status.editor_context.basket_container)
        {
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
            {
                status.editor_context.playlist_selected_song = {};
                status.editor_context.basket_selected_song   = song;
            }

            if (song == status.editor_context.basket_selected_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));

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
        status.editor_context.playlist_container = getPlaylistSongs(status.editor_context.playlist_to_edit);
    }
    else
    {
        if (status.editor_context.playlist_container == vector<SongEntry>{})
            status.editor_context.playlist_container = getPlaylistSongs(status.editor_context.playlist_to_edit);
        playlistEditorMenu(state, status);
    }

    ImGui::PopItemFlag();
    ImGui::PopItemFlag();
    ImGui::End();
}

} // namespace koji_frontend

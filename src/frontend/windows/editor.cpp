// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include <algorithm>
#include <tuple>
#include <vector>
#include <stdlib.h>
#include "../../backend/app.h"
#include "../../backend/library.h"
#include "../../backend/player.h"
#include "../../backend/utils/color.h"
#include "imgui.h"
#include "windows.h"

using namespace std;
using namespace koji;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

void buttonUp(EditorContext &context, float button_size)
{
    if (!ImGui::Button("⮝", ImVec2(button_size, button_size)))
        return;

    vector<SongEntry>::iterator selected;

    if (context.playlist_selected_song != SongEntry{})
    {
        selected = find(context.playlist_container.begin(), context.playlist_container.end(), context.playlist_selected_song);
        if (selected != context.playlist_container.end() && selected != context.playlist_container.begin())
        {
            context.playlist_container.erase(selected);
            context.playlist_container.insert(selected - 1, context.playlist_selected_song);
        }
    }

    else if (context.basket_selected_song != SongEntry{})
    {
        selected = find(context.basket_container.begin(), context.basket_container.end(), context.basket_selected_song);

        if (selected != context.basket_container.end() && selected != context.basket_container.begin())
        {
            context.basket_container.erase(selected);
            context.basket_container.insert(selected - 1, context.basket_selected_song);
        }
    }
}

void buttonDown(EditorContext &context, float button_size)
{
    if (!ImGui::Button("⮟", ImVec2(button_size, button_size)))
        return;

    vector<SongEntry>::iterator selected;

    if (context.playlist_selected_song != SongEntry{})
    {
        selected = find(context.playlist_container.begin(), context.playlist_container.end(), context.playlist_selected_song);
        if (selected != context.playlist_container.end() && selected + 1 != context.playlist_container.end())
        {
            context.playlist_container.erase(selected);
            context.playlist_container.insert(selected + 1, context.playlist_selected_song);
        }
    }

    else if (context.basket_selected_song != SongEntry{})
    {
        selected = find(context.basket_container.begin(), context.basket_container.end(), context.basket_selected_song);

        if (selected != context.basket_container.end() && selected + 1 != context.basket_container.end())
        {
            context.basket_container.erase(selected);
            context.basket_container.insert(selected + 1, context.basket_selected_song);
        }
    }
}

void buttonLeft(EditorContext &context, float button_size)
{
    if (!ImGui::Button("⮜", ImVec2(button_size, button_size)))
        return;

    if (context.playlist_selected_song != SongEntry{} || context.basket_selected_song == SongEntry{})
        return;

    vector<SongEntry>::iterator selected = find(context.basket_container.begin(), context.basket_container.end(), context.basket_selected_song);

    if (selected == context.playlist_container.end())
        return;

    context.basket_container.erase(selected);
    context.playlist_container.push_back(context.basket_selected_song);
    context.basket_selected_song = {};

    context.playlist_selected_song = context.playlist_container[context.playlist_container.size() - 1];
}

void buttonRight(EditorContext &context, float button_size)
{
    if (!ImGui::Button("⮞", ImVec2(button_size, button_size)))
        return;

    if (context.playlist_selected_song == SongEntry{} || context.basket_selected_song != SongEntry{})
        return;

    vector<SongEntry>::iterator selected = find(context.playlist_container.begin(), context.playlist_container.end(), context.playlist_selected_song);

    if (selected == context.playlist_container.end())
        return;

    context.playlist_container.erase(selected);
    context.basket_container.push_back(context.playlist_selected_song);
    context.playlist_selected_song = {};

    context.basket_selected_song = context.basket_container[context.basket_container.size() - 1];
}

void buttonSave(AppState &state, PlayerStatus &status, float button_size)
{
    if (!ImGui::Button("Save", ImVec2(button_size, button_size)))
        return;
}

void buttonCancel(AppState &state, PlayerStatus &status, float button_size)
{
    if (!ImGui::Button("Cancel", ImVec2(button_size, button_size)))
        return;

    state.edit_window     = false;
    status.editor_context = {};
}

void playlistEditorMenu(AppState &state, PlayerStatus &status)
{
    const float  margin                    = 4.0f;
    const float  button_size               = 48.0f;
    const float  table_size_y              = ImGui::GetContentRegionAvail().y;
    const float  table_size_x              = ImGui::GetContentRegionAvail().x / 2 - (button_size);
    const ImVec4 selected_background_color = backend::utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);

    ImGui::BeginChild("playlistChild", ImVec2(table_size_x, table_size_y));
    if (ImGui::BeginTable("playlistTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        int i = 0;
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const SongEntry &song : status.editor_context.playlist_container)
        {
            ImGui::TableNextColumn();

            ImGui::PushID(i);
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
            {
                status.editor_context.basket_selected_song   = {};
                status.editor_context.playlist_selected_song = song;
            }
            ImGui::PopID();
            ++i;

            if (song == status.editor_context.playlist_selected_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("buttons", ImVec2(button_size, table_size_y), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    buttonUp(status.editor_context, button_size);
    buttonDown(status.editor_context, button_size);
    buttonLeft(status.editor_context, button_size);
    buttonRight(status.editor_context, button_size);

    const float original_font_scale = ImGui::GetFont()->Scale;

    // For saving and canceling buttons the font scaling is off and needs to be calculated
    ImGui::SetWindowFontScale(std::min((button_size - margin * 2.0f) / 36.0f, (button_size - margin * 2.0f) / 83.0f));

    // To set the buttons at the bottom of the editor
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - (button_size * 2 + ImGui::GetStyle().FramePadding.y));

    buttonSave(state, status, button_size);
    buttonCancel(state, status, button_size);

    ImGui::SetWindowFontScale(original_font_scale);

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::BeginChild("basketChild", ImVec2(table_size_x, table_size_y));
    if (ImGui::BeginTable("basketTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        int i = 0;
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const SongEntry &song : status.editor_context.basket_container)
        {
            ImGui::TableNextColumn();

            ImGui::PushID(i);
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
            {
                status.editor_context.playlist_selected_song = {};
                status.editor_context.basket_selected_song   = song;
            }
            ImGui::PopID();
            ++i;

            if (song == status.editor_context.basket_selected_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
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
        {
            status.editor_context.playlist_container = getPlaylistSongs(status.playlists[index]);
            status.editor_context.playlist_container.reserve(status.editor_context.playlist_container.size() + status.editor_context.songs_to_append.size());
            for (const SongEntry &song : status.editor_context.songs_to_append)
                status.editor_context.playlist_container.push_back(song);
        }
        ImGui::PopID();
    }
    ImGui::EndTable();
}

namespace koji_frontend
{

void editorWindow(AppState &state, PlayerStatus &status)
{
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionMax().x / 2, ImGui::GetContentRegionMax().y / 2));
    ImGui::Begin("Playlist Editor", &state.edit_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize); // ImGuiChildFlags_NavFlattened,
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

    if (status.editor_context.playlist_container == vector<SongEntry>{})
        selectPlaylistMenu(state, status);
    else
        playlistEditorMenu(state, status);

    ImGui::PopItemFlag();
    ImGui::PopItemFlag();
    ImGui::End();
}

} // namespace koji_frontend

// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "menus.h"
#include "../../../backend/app.h"
#include "../../../backend/library/entries.h"
#include "../../../backend/library/playlists.h"
#include "../../../backend/utils/color.h"
#include "buttons.h"
#include "editor.h"
#include "imgui.h"
#include "imgui_internal.h"

using namespace std;
using namespace koji::backend::app;
using namespace koji::backend::library;

namespace koji::frontend::windows::editor
{
void selectPlaylistMenu(AppState &state)
{
    ImGui::Text("Select a playlist to add to:");
    ImGui::BeginTable("playlistSelector", 1, ImGuiTableFlags_Borders);
    ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);

    for (int index = 0; index < state.player_context.playlists.size(); index++)
    {
        ImGui::PushID(index);
        ImGui::TableNextColumn();

        if (ImGui::Selectable(state.player_context.playlists[index].title.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
        {
            state.editor_context.playlist           = state.player_context.playlists[index];
            state.editor_context.playlist_container = getPlaylistSongs(state.player_context.playlists[index]);
        }
        ImGui::PopID();
    }
    ImGui::EndTable();
}

void playlistEditorMenu(AppState &state)
{
    const float  margin                    = 4.0f;
    const float  button_size               = 48.0f;
    const float  table_size_y              = ImGui::GetContentRegionAvail().y;
    const float  table_size_x              = ImGui::GetWindowSize().x / 2 - (button_size);
    const ImVec4 selected_background_color = koji::backend::utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);

    ImGui::BeginChild("playlistChild", ImVec2(table_size_x, table_size_y));
    ImGui::Text("Playlist");
    ImGui::Separator();
    if (ImGui::BeginTable("playlistTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        int i = 0;
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const SongEntry &song : state.editor_context.playlist_container)
        {
            ImGui::TableNextColumn();

            ImGui::PushID(i);
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
            {
                state.editor_context.basket_selected_song   = {};
                state.editor_context.playlist_selected_song = song;
            }
            ImGui::PopID();
            ++i;

            if (song == state.editor_context.playlist_selected_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
    ImGui::BeginChild("buttons", ImVec2(button_size, table_size_y), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    buttonUp(state.editor_context, button_size);
    buttonDown(state.editor_context, button_size);
    buttonLeft(state.editor_context, button_size);
    buttonRight(state.editor_context, button_size);

    const float original_font_scale = ImGui::GetFont()->Scale;

    // For saving and canceling buttons the font scaling is off and needs to be calculated
    ImGui::SetWindowFontScale(std::min((button_size - margin * 2.0f) / 36.0f, (button_size - margin * 2.0f) / 83.0f));

    // To set the buttons at the bottom of the editor
    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + ImGui::GetContentRegionAvail().y - (button_size * 2 + ImGui::GetStyle().FramePadding.y));

    buttonSave(state, button_size);
    buttonCancel(state, button_size);

    ImGui::SetWindowFontScale(original_font_scale);

    ImGui::EndChild();
    ImGui::SameLine();
    ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
    ImGui::SameLine();
    ImGui::BeginChild("basketChild", ImVec2(table_size_x, table_size_y));
    ImGui::Text("Removal Basket");
    ImGui::Separator();
    if (ImGui::BeginTable("basketTable", 1, ImGuiTableFlags_None, ImVec2(0, 0)))
    {
        int i = 0;
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthStretch);
        for (const SongEntry &song : state.editor_context.basket_container)
        {
            ImGui::TableNextColumn();

            ImGui::PushID(i);
            if (ImGui::Selectable(song.title.c_str(), true, ImGuiSelectableFlags_None))
            {
                state.editor_context.playlist_selected_song = {};
                state.editor_context.basket_selected_song   = song;
            }
            ImGui::PopID();
            ++i;

            if (song == state.editor_context.basket_selected_song)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
}
} // namespace koji::frontend::windows::editor

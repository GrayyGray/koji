// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "buttons.h"
#include "../../../backend/library/entries.h"
#include "../../../backend/app.h"
#include <algorithm>
#include <vector>
#include "imgui.h"

using namespace std;
using namespace koji::backend::app;
using namespace koji::backend::library;

namespace koji::frontend::windows::editor
{
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

void buttonSave(AppState &state, float button_size)
{
    if (!ImGui::Button("Save", ImVec2(button_size, button_size)))
        return;
}

void buttonCancel(AppState &state, float button_size)
{
    if (!ImGui::Button("Cancel", ImVec2(button_size, button_size)))
        return;

    state.editor_context.edit_window     = false;
}
} // namespace koji::frontend::windows::editor

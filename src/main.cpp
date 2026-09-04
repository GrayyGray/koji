// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include <filesystem>
#include <optional>
#include <tuple>
#include <vector>

#include <SDL3/SDL.h>
#include <stdio.h>

#include "app.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "library.h"
#include "tabs.h"

int main(int, char **)
{

    AppState state;
    state.window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    state.width        = 1280;
    state.height       = 720;
    state.title        = "koji";

    if (!initialize(state))
    {
        return 1;
    }

    std::vector<AlbumEntry> albums = getAlbums();
    std::vector<SongEntry>  queue;
    int                     now_playing_index = -1;

    bool done = false;
    while (!done)
    {
        if (!pollEvents(state))
            done = true;

        beginMainWindow(state);
        ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None);
        if (beginTab("Queue"))
        {
            songQueueTab(state, queue, now_playing_index);
            endTab();
        }
        if (beginTab("Albums"))
        {
            albumSelectionTab(state, queue, now_playing_index, albums);
            endTab();
        }
        if (beginTab("Playlists"))
        {
            endTab();
        }
        ImGui::EndTabBar();
        ImGui::Separator();
        ImGui::BeginChild("footer", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_None);
        ImGui::ProgressBar(0.5f, ImVec2(200, 0), "");
        ImGui::EndChild();

        endMainWindow(state);
    }

    cleanup(state);
    return 0;
}

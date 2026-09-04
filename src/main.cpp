// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include <filesystem>
#include <optional>
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
    std::vector<SongEntry>  songs  = getAlbumSongs(albums);
    std::vector<SongEntry>  queue;

    bool done = false;
    while (!done)
    {
        if (!pollEvents(state))
            done = true;

        beginMainWindow(state);
        if (ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None))
        {
            if (beginTab("Queue"))
            {
                songQueueTab(state, queue);
                endTab();
            }
            if (beginTab("Albums"))
            {
                albumSelectionTab(state, queue, albums);
                endTab();
            }
            if (beginTab("Playlists"))
            {
                endTab();
            }
            ImGui::EndTabBar();
        }
        endMainWindow(state);
    }

    cleanup(state);
    return 0;
}

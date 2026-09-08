// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "backend/app.h"
#include "backend/library.h"
#include "backend/player.h"
#include "frontend/tabs.h"
#include "frontend/ui.h"
#include "imgui.h"

int main(int, char **)
{
    koji_app::AppState state;
    state.window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    state.width        = 1280;
    state.height       = 720;
    state.title        = "koji";

    if (!koji_app::initialize(state))
    {
        return 1;
    }

    koji_player::PlayerStatus status;

    if (!koji_player::initialize(state, status))
    {
        koji_app::cleanup(state);
        return 1;
    }

    if (!koji_library::initialize(status))
    {
        koji_app::cleanup(state);
        koji_player::cleanup(status);
        return 1;
    }

    bool done = false;
    while (!done)
    {
        if (!koji_app::pollEvents(state))
            done = true;
        if (!koji_player::pollEvents(status))
            done = true;

        koji_ui::beginMainWindow(state);
        ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None);
        if (koji_ui::beginTab("Queue"))
        {
            ImGui::Separator();
            koji_ui::songQueueTab(state, status);
            koji_ui::endTab();
        }
        if (koji_ui::beginTab("Albums"))
        {
            ImGui::Separator();
            koji_ui::albumSelectionTab(state, status);
            koji_ui::endTab();
        }
        if (koji_ui::beginTab("Playlists"))
        {
            ImGui::Separator();
            koji_ui::playlistSelectionTab(state, status);
            koji_ui::endTab();
        }
        ImGui::EndTabBar();

        koji_ui::renderPlayer(status);

        koji_ui::endMainWindow(state);
    }

    koji_app::cleanup(state);
    koji_player::cleanup(status);
    return 0;
}

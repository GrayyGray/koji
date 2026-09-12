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
            koji_ui::tab(state, status, "queue");
            koji_ui::endTab();
        }
        if (koji_ui::beginTab("Albums"))
        {
            koji_ui::tab(state, status, "album");
            koji_ui::endTab();
        }
        if (koji_ui::beginTab("Playlists"))
        {
            koji_ui::tab(state, status, "playlist");
            koji_ui::endTab();
        }
        ImGui::EndTabBar();

        koji_ui::renderPlayer(status);


        if (state.edit_playlist_window)
        {
            ImGui::SetNextWindowSize(ImVec2(550, 680));
            ImGui::Begin("Playlist Editor", &state.edit_playlist_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus); // ImGuiChildFlags_NavFlattened, 

            ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
            
            ImGui::Text("this is a test");

            ImGui::PopItemFlag();
            ImGui::End();
        }

        koji_ui::endMainWindow(state);
    }

    koji_app::cleanup(state);
    koji_player::cleanup(status);
    return 0;
}

// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include <filesystem>
#include <optional>
#include <vector>
#include "app.h"
#include "imgui.h"
#include "player.h"
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

    koji_player::PlayerStatus player_status;

    bool done = false;
    while (!done)
    {
        if (!pollEvents(state))
            done = true;

        beginMainWindow(state);
        ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None);
        if (beginTab("Queue"))
        {
            ImGui::Separator();
            songQueueTab(state, player_status);
            endTab();
        }
        if (beginTab("Albums"))
        {
            ImGui::Separator();
            albumSelectionTab(state, player_status);
            endTab();
        }
        if (beginTab("Playlists"))
        {
            ImGui::Separator();
            endTab();
        }
        ImGui::EndTabBar();

        ImGui::Separator();

        ImGui::BeginChild("footer", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        renderPlayer(player_status);
        if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X))
        {
            player_status.current_song = {};
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space))
        {
            player_status.paused = !player_status.paused;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        {
            player_status.shuffle = !player_status.shuffle;
        }
        else if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        {
            
        }
        ImGui::EndChild();

        endMainWindow(state);
    }

    cleanup(state);
    return 0;
}

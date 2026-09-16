// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "backend/app.h"
#include "backend/library.h"
#include "backend/player.h"
#include "frontend/tabs/tabs.h"
#include "frontend/ui.h"
#include "frontend/windows/windows.h"
#include "imgui.h"

int main(int, char **)
{
    koji_app::AppState state;
    state.width  = 1280;
    state.height = 720;
    state.title  = "koji";

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

        koji_frontend::beginMainWindow(state);

        ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None);
        koji_frontend::queueTab(state, status);
        koji_frontend::albumTab(state, status);
        koji_frontend::playlistTab(state, status);

        ImGui::EndTabBar();

        koji_frontend::renderPlayer(status);

        if (state.edit_window)
        {
            koji_frontend::editorWindow(state, status);
        }

        koji_frontend::endMainWindow(state);
    }

    koji_app::cleanup(state);
    koji_player::cleanup(status);
    return 0;
}

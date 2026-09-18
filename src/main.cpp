// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "frontend/windows/main/main.h"
#include "backend/app.h"
#include "frontend/compontents/player.h"
#include "frontend/tabs/tabs.h"
#include "frontend/windows/editor/editor.h"
#include "frontend/compontents/notification.h"
#include "imgui.h"

using namespace koji;
using namespace backend;
using namespace frontend;

int main(int, char **)
{
    app::AppState state;
    state.width  = 1280;
    state.height = 720;
    state.title  = "koji";

    if (!app::initialize(state))
    {
        return 1;
    }

    bool done = false;
    while (!done)
    {
        if (!app::pollEvents(state))
            done = true;

        windows::main::beginMainWindow(state);

        ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None);

        tabs::queueTab(state);
        tabs::albumTab(state);
        tabs::playlistTab(state);

        ImGui::EndTabBar();

        components::player::renderPlayer(state.player_context);

        if (state.editor_context.edit_window)
            windows::editor::editorWindow(state);

        components::notification::drawNotification(state);

        windows::main::endMainWindow(state);
    }

    app::cleanup(state);
    return 0;
}

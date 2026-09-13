// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "windows.h"
#include "imgui.h"
#include "backend/library.h"
#include "backend/player.h"
#include "backend/app.h"
#include <stdlib.h>
#include <vector>

using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

namespace koji_ui
{

void editorWindow(koji_app::AppState &state, koji_player::PlayerStatus &status)
{
    ImGui::SetNextWindowSize(ImVec2(550, 680));
    ImGui::Begin("Playlist Editor", &state.edit_window, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoNavFocus); // ImGuiChildFlags_NavFlattened,
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

    ImGui::Text("this is a editor it is in testing");

    ImGui::PopItemFlag();
    ImGui::PopItemFlag();
    ImGui::End();
}

} // namespace koji_ui

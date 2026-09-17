// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "editor.h"
#include "../../../backend/library/entries.h"
#include "../../../backend/app.h"
#include <algorithm>
#include <tuple>
#include <vector>
#include <stdlib.h>
#include "imgui.h"
#include "menus.h"

using namespace std;
using namespace koji::backend::app;
using namespace koji::backend::library;


namespace koji::frontend::windows::editor
{
void editorWindow(AppState &state)
{
    ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionMax().x / 2, ImGui::GetContentRegionMax().y / 2));
    ImGui::Begin("Playlist Editor", &state.editor_context.edit_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize); // ImGuiChildFlags_NavFlattened,
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);

    if (state.editor_context.playlist_container == vector<SongEntry>{})
        selectPlaylistMenu(state);
    else
        playlistEditorMenu(state);

    ImGui::PopItemFlag();
    ImGui::PopItemFlag();
    ImGui::End();
}
} // namespace koji::frontend::windows::editor


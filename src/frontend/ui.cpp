// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include <SDL3/SDL.h>
#include "../backend/app.h"
#include "../backend/player.h"
#include "../backend/utils.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

namespace koji_ui
{
void beginMainWindow(const koji_app::AppState &state)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(state.io->DisplaySize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Begin("mainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings);
}

void endMainWindow(const koji_app::AppState &state)
{
    ImGui::PopStyleColor();
    ImGui::End();
    ImGui::Render();
    SDL_SetRenderScale(state.renderer, state.io->DisplayFramebufferScale.x, state.io->DisplayFramebufferScale.y);
    SDL_RenderClear(state.renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer);
    SDL_RenderPresent(state.renderer);
}

bool beginTab(const char *label)
{
    if (!ImGui::BeginTabItem(label, nullptr, ImGuiTabItemFlags_NoArrowNav))
        return false;

    ImGui::Spacing();
    ImGui::BeginChild("mainBrowser", ImVec2(0, -65), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

    return true;
}

void endTab()
{
    ImGui::PopItemFlag();
    ImGui::EndChild();
    ImGui::EndTabItem();
}

void renderPlayer(const koji_player::PlayerStatus &status)
{
    ImGui::Separator();
    ImGui::BeginChild("footer", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SameLine();
    ImGui::Text("%s %s", status.current_song == koji_player::SongEntry{} ? "⏹" : status.paused ? "⏸" : "⯈", status.current_song.title.empty() ? "nothing playing" : status.current_song.title.c_str());
    ImGui::SameLine();

    const char *shuffle_mode = status.shuffle ? "Shuf:On" : "Shuf:Off";
    const char *repeat_mode  = status.repeat_mode == koji_player::RepeatMode::Off ? "Rep:Off" : status.repeat_mode == koji_player::RepeatMode::All ? "Rep:All" : "Rep:Trk";

    std::string volume_percentage = ("Vol:" + std::to_string(status.volume) + "%");
    std::string position_time     = status.current_song != koji_player::SongEntry{} ? koji_utils::formatTime(status.position_seconds) : "--:--";
    std::string duration_time     = status.current_song != koji_player::SongEntry{} ? koji_utils::formatTime(status.current_song.duration) : "--:--";

    float playing_progress    = (status.current_song != koji_player::SongEntry{} && status.current_song.duration > 0.0f) ? (status.position_seconds / status.current_song.duration) : 0.0f;
    float right_segment_width = ImGui::CalcTextSize((position_time + "/" + duration_time).c_str()).x + 200.0f + ImGui::CalcTextSize(volume_percentage.c_str()).x + ImGui::CalcTextSize(shuffle_mode).x + ImGui::CalcTextSize(repeat_mode).x + ImGui::GetStyle().ItemSpacing.x * 3.0f;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - right_segment_width);

    ImGui::Text("%s/%s", position_time.c_str(), duration_time.c_str());
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::ProgressBar(playing_progress, ImVec2(200, 20), "");
    ImGui::PopStyleVar();
    ImGui::SameLine();

    ImGui::TextUnformatted(volume_percentage.c_str());
    ImGui::SameLine();

    ImGui::TextUnformatted(shuffle_mode);
    ImGui::SameLine();

    ImGui::TextUnformatted(repeat_mode);

    ImGui::Separator();
    ImGui::Text("shift+click: add to queue   tab: cycle tabs   s: shuffle   r: repeat   space: play/pause   x: stop   q: quit");
    ImGui::EndChild();
}

} // namespace koji_ui

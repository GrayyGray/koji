// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "player.h"
#include <SDL3/SDL.h>
#include "../..//backend/library/entries.h"
#include "../../backend/app.h"
#include "../../backend/utils/time.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

using namespace koji::backend::library;
using namespace koji::backend::app;

namespace koji::frontend::components::player
{
void renderPlayer(const PlayerContext &context)
{
    ImGui::Separator();
    ImGui::BeginChild("footer", ImVec2(0, 0), ImGuiChildFlags_None, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SameLine();
    ImGui::Text("%s %s", context.current_song == SongEntry{} ? "⏹" : context.paused ? "⏸" : "⯈", context.current_song.title.empty() ? "nothing playing" : context.current_song.title.c_str());
    ImGui::SameLine();

    const char *shuffle_mode = context.shuffle ? "Shuf:On" : "Shuf:Off";
    const char *repeat_mode  = context.repeat_mode == RepeatMode::Off ? "Rep:Off" : context.repeat_mode == RepeatMode::All ? "Rep:All" : "Rep:Trk";

    std::string volume_percentage = ("Vol:" + std::to_string(context.volume) + "%");
    std::string position_time     = context.current_song != SongEntry{} ? backend::utils::formatTime(context.position_seconds) : "--:--";
    std::string duration_time     = context.current_song != SongEntry{} ? backend::utils::formatTime(context.current_song.duration) : "--:--";

    float playing_progress    = (context.current_song != SongEntry{} && context.current_song.duration > 0.0f) ? (context.position_seconds / context.current_song.duration) : 0.0f;
    float right_segment_width = ImGui::CalcTextSize((position_time + "/" + duration_time).c_str()).x + 200.0f + ImGui::CalcTextSize(volume_percentage.c_str()).x + ImGui::CalcTextSize(shuffle_mode).x + ImGui::CalcTextSize(repeat_mode).x + ImGui::GetStyle().ItemSpacing.x * 4.0f;

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
    ImGui::Text("s: shuffle   r: repeat   space: play/pause   x: stop   q: quit");
    ImGui::EndChild();
}
} // namespace koji::frontend::components::player
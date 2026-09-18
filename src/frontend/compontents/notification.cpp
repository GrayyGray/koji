// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "notification.h"

namespace koji::frontend::components::notification
{

void setNotification(koji::backend::app::AppState &state, const std::string &message) { state.notification_state = {message, 3.0f}; }

void drawNotification(koji::backend::app::AppState &state)
{
    if (state.notification_state.time_left <= 0.0f)
        return;

    state.notification_state.time_left -= ImGui::GetIO().DeltaTime;

    const float padding = 20.0f;
    const float width   = ImGui::CalcTextSize(state.notification_state.message.c_str()).x + padding;
    const float height  = 60.0f;

    const ImVec2 window_size = ImGui::GetWindowSize();

    ImGui::SetNextWindowSize(ImVec2(width, height));
    ImGui::SetNextWindowPos(ImVec2(window_size.x - width - padding, window_size.y - height - padding - 100));
    ImGui::Begin("Notification", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove);
    ImGui::TextUnformatted(state.notification_state.message.c_str());
    ImGui::End();
}
} // namespace koji::frontend::components::notification

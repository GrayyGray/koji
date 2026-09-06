// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "imgui_utils.h"

#include <algorithm>
#include <format>

ImVec4 darkenColor(const ImVec4 &color, float amount)
{
    float red   = std::max(0.0f, color.x - 0.2f);
    float green = std::max(0.0f, color.y - 0.2f);
    float blue  = std::max(0.0f, color.z - 0.2f);
    return ImVec4(red, green, blue, color.w);
}

std::string formatTime(float seconds)
{
    if (seconds < 0.0f)
        return "--:--";

    int total   = static_cast<int>(seconds);
    int hours   = total / 3600;
    int minutes = (total % 3600) / 60;
    int secs    = total % 60;

    if (hours > 0)
        return std::format("{:02}:{:02}:{:02}", hours, minutes, secs);
    return std::format("{:02}:{:02}", minutes, secs);
}
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <algorithm>

#include "imgui.h"

ImVec4 darkenColor(const ImVec4 &color, float amount)
{
    float red   = std::max(0.0f, color.x - 0.2f);
    float green = std::max(0.0f, color.y - 0.2f);
    float blue  = std::max(0.0f, color.z - 0.2f);
    return ImVec4(red, green, blue, color.w);
}
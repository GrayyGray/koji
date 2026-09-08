// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <string>
#include "imgui.h"

namespace koji_utils
{
std::string formatTime(const float seconds);
ImVec4      darkenColor(const ImVec4 &color, float amount);
} // namespace koji_utils

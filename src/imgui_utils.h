// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <string>

#include "imgui.h"

ImVec4 darkenColor(const ImVec4 &color, float amount);

std::string formatTime(float seconds);
// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include "imgui.h"
#include <string>
#include "../../backend/app.h"

namespace koji::frontend::components::notification
{
void setNotification(koji::backend::app::AppState &state, const std::string& message);
void drawNotification(koji::backend::app::AppState &state);
} // namespace koji::frontend::components::notification


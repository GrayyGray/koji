// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include "../backend/app.h"
#include "../backend/player.h"
namespace koji_ui
{
void songQueueTab(const koji_app::AppState &state, koji_player::PlayerStatus &status);
void albumSelectionTab(const koji_app::AppState &state, koji_player::PlayerStatus &status);
} // namespace koji_ui



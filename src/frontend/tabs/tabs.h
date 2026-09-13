// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include "backend/app.h"
#include "backend/player.h"

namespace koji_frontend
{
void queueTab(koji_app::AppState &state, koji_player::PlayerStatus &status);
void albumTab(koji_app::AppState &state, koji_player::PlayerStatus &status);
void playlistTab(koji_app::AppState &state, koji_player::PlayerStatus &status);
} // namespace koji_frontend

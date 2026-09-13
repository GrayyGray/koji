// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "backend/app.h"
#include "backend/player.h"

namespace koji_frontend
{
void beginMainWindow(const koji_app::AppState &state);
void endMainWindow(const koji_app::AppState &state);

void renderPlayer(const koji_player::PlayerStatus &status);
} // namespace koji_frontend

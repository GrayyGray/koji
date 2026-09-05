// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "app.h"
#include "player.h"

void songQueueTab(const AppState &state, PlayerStatus &status);
void albumSelectionTab(const AppState &state, PlayerStatus &status);

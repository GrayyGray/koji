// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "app.h"
#include "library.h"

void songQueueTab(const AppState &state, std::vector<SongEntry> &queue, int &now_playing_index);
void albumSelectionTab(const AppState &state, std::vector<SongEntry> &queue, int &now_playing_index, const std::vector<AlbumEntry> &albums);

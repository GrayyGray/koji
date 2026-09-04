// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "app.h"
#include "library.h"

void songQueueTab(AppState state, std::vector<SongEntry> &queue);
void albumSelectionTab(AppState state, std::vector<SongEntry> &queue, const std::vector<AlbumEntry> &albums);

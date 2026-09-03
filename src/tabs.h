// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <optional>
#include <vector>

#include "library.h"

void albumSelectionTab(std::vector<SongEntry>        &queue,
                       const std::vector<AlbumEntry> &albums);
void songQueueTab(std::vector<SongEntry> &queue);
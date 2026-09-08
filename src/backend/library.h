// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include "backend/player.h"
#include <vector>

namespace koji_library
{
    std::vector<koji_player::AlbumEntry> getAlbums();
    std::vector<koji_player::SongEntry>  getAlbumSongs(const koji_player::AlbumEntry &album); 
} // namespace koji_library



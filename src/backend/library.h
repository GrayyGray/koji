// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include "player.h"
#include <vector>

namespace koji_library
{
    std::vector<koji_player::SongEntry>  getAlbumSongs(const koji_player::AlbumEntry &album);
    std::vector<koji_player::SongEntry>  getPlaylistSongs();
    bool initialize(koji_player::PlayerStatus &status);
} // namespace koji_library



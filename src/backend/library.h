// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <vector>
#include "player.h"

namespace koji_library
{
std::vector<koji_player::SongEntry> getAlbumSongs(const koji_player::AlbumEntry &album);
std::vector<koji_player::SongEntry> getPlaylistSongs(const koji_player::PlaylistEntry &playlist);
bool                                initialize(koji_player::PlayerStatus &status);
} // namespace koji_library

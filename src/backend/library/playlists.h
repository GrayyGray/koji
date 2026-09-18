// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <vector>
#include "entries.h"

namespace koji::backend::library
{
std::vector<PlaylistEntry> getPlaylists();
std::vector<SongEntry>     getPlaylistSongs(const PlaylistEntry &playlist);
void savePlaylist(const PlaylistEntry &entry, const std::vector<SongEntry> &playlist) ;
} // namespace koji::backend::library

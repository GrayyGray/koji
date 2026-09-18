// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <vector>
#include <string>
#include "entries.h"

namespace koji::backend::library
{
std::vector<PlaylistEntry> getPlaylists();
std::vector<SongEntry>     getPlaylistSongs(const PlaylistEntry &playlist);
bool                       renamePlaylist(const PlaylistEntry &entry, const std::string &name);
void                       savePlaylist(const PlaylistEntry &entry, const std::vector<SongEntry> &playlist);
bool                       duplicatePlaylist(const PlaylistEntry &entry);
} // namespace koji::backend::library

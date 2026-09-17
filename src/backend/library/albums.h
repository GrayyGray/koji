// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <vector>
#include "entries.h"

namespace koji::backend::library
{
std::vector<AlbumEntry> getAlbums();
std::vector<SongEntry>  getAlbumSongs(const AlbumEntry &album);
} // namespace koji::backend::library

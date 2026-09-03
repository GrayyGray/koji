// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

struct AlbumEntry
{
    std::filesystem::path path;
    std::string           artist;
    std::string           album_title;
    bool                  operator==(const AlbumEntry &) const = default;
};

struct SongEntry
{
    std::filesystem::path path;
    AlbumEntry            album;
    std::string           track_number;
    std::string           title;
    std::string           duration;
};

std::vector<AlbumEntry> getAlbums();
std::vector<SongEntry>  getAlbumSongs(std::vector<AlbumEntry> albums);
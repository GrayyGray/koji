// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <string>

namespace koji::backend::library
{
struct AlbumEntry
{
    std::filesystem::path path;
    std::string           title;
    std::string           artist;
    bool                  operator==(const AlbumEntry &) const = default;
};

struct PlaylistEntry
{
    std::filesystem::path path;
    std::string           title;
    bool                  operator==(const PlaylistEntry &) const = default;
};

struct SongEntry
{
    std::filesystem::path path;
    std::string           artist;
    std::string           album;
    std::string           title;
    float                 duration;
    bool                  operator==(const SongEntry &) const = default;
};
} // namespace koji::backend::library
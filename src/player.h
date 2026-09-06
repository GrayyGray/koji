// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace koji_player
{
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
    float                 duration;
    bool                  operator==(const SongEntry &) const = default;
};

std::vector<AlbumEntry> getAlbums();
std::vector<SongEntry>  getAlbumSongs(const AlbumEntry album);

enum class RepeatMode
{
    Off,
    All,
    Track
};

struct PlayerStatus
{
    int                     volume           = 35;
    bool                    paused           = false;
    bool                    shuffle          = false;
    float                   position_seconds = 0.0f;
    RepeatMode              repeat_mode      = RepeatMode::All;
    std::vector<AlbumEntry> albums           = getAlbums();
    SongEntry               current_song     = {};
    std::vector<SongEntry>  queue;
};

std::string formatTime(float seconds);
} // namespace koji_player

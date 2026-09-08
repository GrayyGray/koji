// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <algorithm>
#include <filesystem>
#include <random>
#include <string>
#include <variant>
#include <vector>
#include <mpv/client.h>
#include "app.h"

namespace koji_player
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

enum class RepeatMode
{
    Off,
    All,
    Track
};

struct PlayerStatus
{
    int        volume           = 35;
    bool       paused           = false;
    bool       shuffle          = false;
    float      position_seconds = 0.0f;
    RepeatMode repeat_mode      = RepeatMode::All;

    std::vector<AlbumEntry>    albums;
    std::vector<PlaylistEntry> playlists;
    SongEntry                  current_song = {};
    std::vector<SongEntry>     queue;
    std::vector<SongEntry>     unshuffled_queue;

    std::mt19937 random_engine{std::random_device{}()};
    mpv_handle  *mpv_context = nullptr;
};

bool initialize(koji_app::AppState &state, PlayerStatus &status);
bool pollEvents(PlayerStatus &status);
void cleanup(PlayerStatus &status);

void updateCurrentSong(PlayerStatus &status);
void updatePause(const PlayerStatus &status);
void updateVolume(const PlayerStatus &status);
void stopSong(PlayerStatus &status);

void togglePause(koji_player::PlayerStatus &status);
void toggleShuffle(koji_player::PlayerStatus &status);
void toggleRepeatMode(koji_player::RepeatMode &repeat_mode);

void addSongsToQueue(PlayerStatus &status, std::vector<SongEntry> &songs);
} // namespace koji_player

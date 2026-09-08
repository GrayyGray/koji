// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <algorithm>
#include <filesystem>
#include <random>
#include <string>
#include <vector>
#include <mpv/client.h>

struct AppState;

namespace koji_player
{
std::string formatTime(const float seconds);

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
std::vector<SongEntry>  getAlbumSongs(const AlbumEntry &album);

enum class RepeatMode
{
    Off,
    All,
    Track
};

struct PlayerStatus
{
    // Playback state
    int        volume           = 35;
    bool       paused           = false;
    bool       shuffle          = false;
    float      position_seconds = 0.0f;
    RepeatMode repeat_mode      = RepeatMode::All;

    // Library and queue
    std::vector<AlbumEntry> albums       = getAlbums();
    SongEntry               current_song = {};
    std::vector<SongEntry>  queue;
    std::vector<SongEntry>  unshuffled_queue;

    // Runtime
    std::mt19937 random_engine{std::random_device{}()};
    mpv_handle  *mpv_context = nullptr;
};

bool initializePlayer(AppState &state, PlayerStatus &status);
void cleanupPlayer(PlayerStatus &status);
void updateCurrentSong(PlayerStatus &status);
void updatePlayerPause(const PlayerStatus &status);
void updatePlayerVolume(const PlayerStatus &status);
void addSongsToQueue(PlayerStatus &status, std::vector<SongEntry> &songs);
void stopSong(PlayerStatus &status);
bool pollEvents(PlayerStatus &status);
} // namespace koji_player

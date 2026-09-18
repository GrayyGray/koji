// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <mpv/client.h>
#include "../backend/library/entries.h"
#include "imgui.h"

namespace koji::backend::app
{

enum class RepeatMode
{
    Off,
    All,
    Track
};

enum class EditorMode
{
    None,
    Edit,
    Rename
};

struct EditorContext
{
    EditorMode mode = EditorMode::None;
    std::string rename;
    koji::backend::library::PlaylistEntry          playlist;
    bool                                           edit_window = false;
    std::vector<koji::backend::library::SongEntry> basket_container;
    std::vector<koji::backend::library::SongEntry> playlist_container;
    koji::backend::library::SongEntry              basket_selected_song;
    koji::backend::library::SongEntry              playlist_selected_song;
};

struct PlayerContext
{
    int        volume           = 35;
    bool       paused           = false;
    bool       shuffle          = false;
    float      position_seconds = 0.0f;
    RepeatMode repeat_mode      = RepeatMode::All;

    std::vector<koji::backend::library::AlbumEntry>    albums;
    std::vector<koji::backend::library::PlaylistEntry> playlists;
    koji::backend::library::SongEntry                  current_song = {};
    std::vector<koji::backend::library::SongEntry>     queue;
    std::vector<koji::backend::library::SongEntry>     unshuffled_queue;

    std::mt19937 random_engine{std::random_device{}()};
    mpv_handle  *mpv_context = nullptr;
};

struct NotificationState
{
    std::string message;
    float time_left = 0.0f;
};

struct AppState
{
    ImGuiIO      *io;
    const char   *title;
    SDL_Window   *window;
    SDL_Renderer *renderer;
    int           width, height;
    float         display_content_scale;
    EditorContext editor_context;
    PlayerContext player_context;
    NotificationState notification_state;
};

bool initialize(AppState &state);

bool pollEvents(AppState &state);

void cleanup(AppState &state);

void updateCurrentSong(AppState &state);
void updatePause(AppState &state);
void updateVolume(AppState &state, const int level);
void stopSong(AppState &state);

void togglePause(AppState &state);
void toggleShuffle(AppState &state);
void toggleRepeatMode(RepeatMode &repeat_mode);

void addSongsToQueue(AppState &state, std::vector<koji::backend::library::SongEntry> &songs);

} // namespace koji::backend::app

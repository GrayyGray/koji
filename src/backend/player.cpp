// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "player.h"
#include <format>
#include <iostream>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <vector>
#include <SDL3/SDL.h>
#include <mpv/client.h>
#include <taglib/fileref.h>
#include "app.h"
#include "imgui.h"

using namespace std;

namespace koji_player
{
bool initialize(koji_app::AppState &state, PlayerStatus &status)
{
    status.mpv_context = mpv_create();

    if (!status.mpv_context)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating mpv context", state.window);
        cleanup(status);
        return false;
    }

    if (mpv_initialize(status.mpv_context) != 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing mpv", state.window);
        cleanup(status);
        return false;
    }

    mpv_set_option_string(status.mpv_context, "vo", "null");
    mpv_set_option_string(status.mpv_context, "audio-format", "s16le");
    updateVolume(status);

    return true;
}

void cleanup(PlayerStatus &status) { mpv_destroy(status.mpv_context); }

int indexSong(const vector<SongEntry> &songs, const SongEntry &song)
{
    vector<SongEntry>::const_iterator iterator = find(songs.begin(), songs.end(), song);

    if (iterator == songs.end())
        return -1;

    return distance(songs.begin(), iterator);
}

void togglePause(PlayerStatus &status)
{
    status.paused = !status.paused;
    updatePause(status);
}

void toggleShuffle(PlayerStatus &status)
{
    status.shuffle = !status.shuffle;

    if (status.shuffle)
    {
        status.unshuffled_queue = status.queue;
        status.random_engine.seed(random_device{}());
        ranges::shuffle(status.queue, status.random_engine);
    }
    else
    {
        status.queue = status.unshuffled_queue;
        status.unshuffled_queue.clear();
    }
}

void toggleRepeatMode(RepeatMode &repeat_mode)
{
    if (repeat_mode == RepeatMode::Off)
    {
        repeat_mode = RepeatMode::All;
    }
    else if (repeat_mode == RepeatMode::All)
    {
        repeat_mode = RepeatMode::Track;
    }
    else if (repeat_mode == RepeatMode::Track)
    {
        repeat_mode = RepeatMode::Off;
    }
}

void updateCurrentSong(PlayerStatus &status)
{
    status.paused = false;
    updatePause(status);
    const char *play_command[] = {"loadfile", status.current_song.path.c_str(), "replace", nullptr};
    mpv_command(status.mpv_context, play_command);
}

void updatePause(const PlayerStatus &status)
{
    if (status.paused)
        mpv_set_property_string(status.mpv_context, "pause", "yes");
    else
        mpv_set_property_string(status.mpv_context, "pause", "no");
}

void updateVolume(const PlayerStatus &status) { mpv_set_property_string(status.mpv_context, "volume", to_string(status.volume).c_str()); }

void stopSong(PlayerStatus &status)
{
    status.paused = true;
    updatePause(status);
    mpv_command_string(status.mpv_context, "stop");
}

void addSongsToQueue(PlayerStatus &status, vector<SongEntry> &songs)
{
    if (status.shuffle)
    {
        status.unshuffled_queue.insert(status.unshuffled_queue.end(), songs.begin(), songs.end());
        ranges::shuffle(songs, status.random_engine);
    }

    if (status.queue.empty())
    {
        status.current_song = songs[0];
        updateCurrentSong(status);
    }

    status.queue.insert(status.queue.end(), songs.begin(), songs.end());
}

void cycleSong(PlayerStatus &status)
{
    int current_song_index = indexSong(status.queue, status.current_song);

    if (current_song_index + 1 >= status.queue.size())
        status.current_song = status.queue[0];
    else
        status.current_song = status.queue[current_song_index + 1];

    updateCurrentSong(status);
}

bool songCycle(PlayerStatus &status)
{
    if (status.current_song == SongEntry{})
        return true;

    double time_remaining;
    mpv_get_property(status.mpv_context, "time-pos", MPV_FORMAT_DOUBLE, &time_remaining);
    status.position_seconds = static_cast<float>(time_remaining);

    mpv_event *event = mpv_wait_event(status.mpv_context, 0);

    if (event->event_id == MPV_EVENT_END_FILE)
    {
        mpv_event_end_file *end_file = static_cast<mpv_event_end_file *>(event->data);

        if (end_file->reason == MPV_END_FILE_REASON_EOF)
            cycleSong(status);
    }

    return true;
}

bool keyCycle(PlayerStatus &status)
{
    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Q))
        return false;

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X))
    {
        koji_player::stopSong(status);
        status.current_song = {};
    }

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space))
        koji_player::togglePause(status);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        koji_player::toggleShuffle(status);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        koji_player::toggleRepeatMode(status.repeat_mode);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Minus) && status.volume - 5 >= 0)
    {
        status.volume -= 5;
        koji_player::updateVolume(status);
    }

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Equal) && status.volume + 5 <= 100)
    {
        status.volume += 5;
        koji_player::updateVolume(status);
    }

    return true;
}

bool pollEvents(PlayerStatus &status)
{
    if (!songCycle(status))
        return false;

    if (!keyCycle(status))
        return false;

    return true;
}

} // namespace koji_player

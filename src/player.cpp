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

namespace koji_player
{
std::string formatTime(const float seconds)
{
    if (seconds < 0.0f)
        return "--:--";

    int total   = static_cast<int>(seconds);
    int hours   = total / 3600;
    int minutes = (total % 3600) / 60;
    int secs    = total % 60;

    if (hours > 0)
        return std::format("{:02}:{:02}:{:02}", hours, minutes, secs);
    return std::format("{:02}:{:02}", minutes, secs);
}

int indexSong(const std::vector<SongEntry> &songs, const SongEntry &song)
{
    std::vector<SongEntry>::const_iterator iterator = std::find(songs.begin(), songs.end(), song);

    if (iterator == songs.end())
        return -1;

    return std::distance(songs.begin(), iterator);
}

std::optional<std::filesystem::path> xdgConfigDir()
{
    const char *xdg_config = getenv("XDG_CONFIG_HOME");
    if (xdg_config && xdg_config[0] != '\0')
    {
        return std::filesystem::path(xdg_config);
    }

    // fallback per spec: $HOME/.config
    const char *home = getenv("HOME");
    if (home)
    {
        return std::filesystem::path(home) / ".config";
    }

    return std::nullopt;
}

std::vector<AlbumEntry> getAlbums()
{
    std::vector<AlbumEntry> albums;

    std::optional<std::filesystem::path> xdg_config_directory = xdgConfigDir();
    if (!xdg_config_directory)
    {
        std::cout << "xdg config directory locate failed" << std::endl;
        return albums;
    }

    std::filesystem::path album_directory = *xdg_config_directory / "koji" / "albums";
    for (const auto &album : std::filesystem::recursive_directory_iterator(album_directory))
    {
        if (!std::filesystem::is_directory(album))
        {
            continue;
        }

        std::smatch match;
        std::string relative_album_path_string = std::filesystem::relative(album.path(), album_directory).string();
        if (!std::regex_match(relative_album_path_string, match, std::regex(R"(([^/]+)/([^/]+))")))
        {
            continue;
        }

        std::string artist      = match.str(1);
        std::string album_title = match.str(2);

        AlbumEntry entry = {album.path(), artist, album_title};
        if (find(albums.begin(), albums.end(), entry) == albums.end())
        {
            albums.push_back(entry);
        }
    }

    return albums;
}

std::vector<SongEntry> getAlbumSongs(const AlbumEntry &album)
{
    std::vector<SongEntry> songs;

    for (const auto &song : std::filesystem::recursive_directory_iterator(album.path.c_str()))
    {
        if (std::filesystem::is_directory(song))
        {
            continue;
        }

        std::smatch match;
        std::string relative_song_path_string = std::filesystem::relative(song.path(), album.path).string();

        if (!std::regex_match(relative_song_path_string, match, std::regex(R"(([0-9]+) - (.+)\.(mp3|wav|flac|ogg|m4a))")))
        {
            continue;
        }

        std::string track_number = match.str(1);
        std::string song_title   = match.str(2);

        float           duration;
        TagLib::FileRef song_file(song.path().c_str());
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
        {
            duration = song_file.audioProperties()->lengthInSeconds();
        }
        else
            duration = 0.0f;

        SongEntry entry = {song.path(), album, track_number, song_title, duration};
        songs.push_back(entry);
    }
    return songs;
}


void updateCurrentSong(PlayerStatus &status)
{
    status.paused = false;
    updatePlayerPause(status);
    const char *play_command[] = {"loadfile", status.current_song.path.c_str(), "replace", nullptr};
    mpv_command(status.mpv_context, play_command);
}

void updatePlayerPause(const PlayerStatus &status)
{
    if (status.paused)
        mpv_set_property_string(status.mpv_context, "pause", "yes");
    else
        mpv_set_property_string(status.mpv_context, "pause", "no");
}

void updatePlayerVolume(const PlayerStatus &status)
{
    mpv_set_property_string(status.mpv_context, "volume", std::to_string(status.volume).c_str());
}

void stopSong(PlayerStatus &status)
{
    status.paused = true;
    updatePlayerPause(status);
    mpv_set_property_string(status.mpv_context, "seek", "0");
}

void cycleSong(PlayerStatus &status)
{
    int current_song_index = indexSong(status.queue, status.current_song);

    if (current_song_index + 1 > status.queue.size())
        status.current_song = status.queue[0];
    else
        status.current_song = status.queue[current_song_index + 1];
    
    updateCurrentSong(status);
}

bool initializePlayer(AppState &state, PlayerStatus &status)
{
    status.mpv_context = mpv_create();

    if (!status.mpv_context)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating mpv context", state.window);
        cleanupApp(state);
        cleanupPlayer(status);
        return false;
    }

    if (mpv_initialize(status.mpv_context) != 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing mpv", state.window);
        cleanupApp(state);
        cleanupPlayer(status);
        return false;
    }

    mpv_set_option_string(status.mpv_context, "vo", "null");
    mpv_set_option_string(status.mpv_context, "audio-format", "s16le");
    updatePlayerVolume(status);

    return true;
}

void cleanupPlayer(PlayerStatus &status) { mpv_destroy(status.mpv_context); }


void addSongsToQueue(PlayerStatus &status, std::vector<SongEntry> &songs)
{
    if (status.shuffle)
    {
        status.unshuffled_queue.insert(status.unshuffled_queue.end(), songs.begin(), songs.end());
        std::ranges::shuffle(songs, status.random_engine);
    }

    if (status.queue.empty())
        status.current_song = songs[0];

    status.queue.insert(status.queue.end(), songs.begin(), songs.end());
    updateCurrentSong(status);    
}

bool pollEvents(PlayerStatus &status)
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

} // namespace koji_player

// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "player.h"

#include <iostream>
#include <optional>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include <mpv/client.h>
#include <taglib/fileref.h>

#include "imgui.h"
#include "imgui_utils.h"

namespace koji_player
{

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

std::vector<SongEntry> getAlbumSongs(const AlbumEntry album)
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
} // namespace koji_player

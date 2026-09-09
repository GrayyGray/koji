// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "library.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <taglib/fileref.h>

namespace koji_library
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

std::vector<koji_player::AlbumEntry> getAlbums()
{
    std::vector<koji_player::AlbumEntry> albums;

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

        std::string            relative_album_path_string = std::filesystem::relative(album.path(), album_directory).string();
        std::string::size_type slash_positon              = relative_album_path_string.find('/');

        if (slash_positon == std::string::npos)
            continue;

        std::string artist      = relative_album_path_string.substr(0, slash_positon);
        std::string album_title = relative_album_path_string.substr(slash_positon + 1);

        koji_player::AlbumEntry entry = {album.path(), album_title, artist};
        if (find(albums.begin(), albums.end(), entry) == albums.end())
        {
            albums.push_back(entry);
        }
    }

    return albums;
}

std::vector<koji_player::PlaylistEntry> getPlaylists()
{
    std::vector<koji_player::PlaylistEntry> playlists;

    std::optional<std::filesystem::path> xdg_config_directory = xdgConfigDir();
    if (!xdg_config_directory)
    {
        std::cout << "xdg config directory locate failed" << std::endl;
        return playlists;
    }
    std::filesystem::path playlists_directory = *xdg_config_directory / "koji" / "playlists";

    for (const auto &playlist : std::filesystem::directory_iterator(playlists_directory))
    {
        if (std::filesystem::is_directory(playlist))
        {
            continue;
        }

        std::string            relative_album_path_string = std::filesystem::relative(playlist.path(), playlists_directory).string();
        std::string::size_type dot_positon                = relative_album_path_string.find('.');
        std::string            playlist_title             = relative_album_path_string.substr(0, dot_positon);

        koji_player::PlaylistEntry entry = {playlist.path(), playlist_title};
        if (find(playlists.begin(), playlists.end(), entry) == playlists.end())
        {
            playlists.push_back(entry);
        }
    }

    return playlists;
}

std::vector<koji_player::SongEntry> getAlbumSongs(const koji_player::AlbumEntry &album)
{
    std::vector<koji_player::SongEntry> songs;

    for (const auto &song : std::filesystem::recursive_directory_iterator(album.path.c_str()))
    {
        if (std::filesystem::is_directory(song))
        {
            continue;
        }
        std::string relative_song_path_string = std::filesystem::relative(song.path(), album.path).string();

        std::string::size_type track_separator     = relative_song_path_string.find(" - ");
        std::string::size_type extension_separator = relative_song_path_string.rfind('.');

        if (track_separator == std::string::npos)
            continue;
        if (extension_separator == std::string::npos)
            continue;

        std::string song_title = relative_song_path_string.substr(track_separator + 3, extension_separator - (track_separator + 3));

        TagLib::FileRef song_file(song.path().c_str());

        std::string artist;
        if (!song_file.isNull() && song_file.tag())
            artist = song_file.tag()->artist().to8Bit(true);
        else
            continue;

        float duration;
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
            duration = song_file.audioProperties()->lengthInSeconds();
        else
            continue;

        koji_player::SongEntry entry = {song.path(), artist, album.title, song_title, duration};
        songs.push_back(entry);
    }
    return songs;
}

std::vector<koji_player::SongEntry> getPlaylistSongs(const koji_player::PlaylistEntry &playlist)
{
    std::vector<koji_player::SongEntry> songs;

    std::ifstream playlist_file(playlist.path);

    if (!playlist_file)
        return songs;

    std::optional<std::filesystem::path> xdg_config_directory = xdgConfigDir();
    if (!xdg_config_directory)
    {
        std::cout << "xdg config directory locate failed" << std::endl;
        return songs;
    }

    std::filesystem::path songs_directory = *xdg_config_directory / "koji" / "playlists" / "songs";

    std::string song_title;

    while (std::getline(playlist_file, song_title))
    {
        std::filesystem::path song_path = songs_directory / song_title;

        TagLib::FileRef song_file(song_path.c_str());

        std::string artist;
        std::string album;
        if (!song_file.isNull() && song_file.tag())
        {
            artist = song_file.tag()->artist().to8Bit(true);
            album  = song_file.tag()->album().to8Bit(true);
        }
        else
            continue;

        float duration;
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
            duration = song_file.audioProperties()->lengthInSeconds();
        else
            continue;

        koji_player::SongEntry entry = {song_path, artist, album, song_title, duration};
        songs.push_back(entry);
    }
    return songs;
}

bool initialize(koji_player::PlayerStatus &status)
{
    status.albums    = getAlbums();
    status.playlists = getPlaylists();
    return true;
}
} // namespace koji_library

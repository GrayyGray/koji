// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "library.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <tuple>
#include <taglib/fileref.h>

using namespace std;
using namespace koji_player;

namespace koji_library
{
optional<filesystem::path> xdgConfigDir()
{
    const char *xdg_config = getenv("XDG_CONFIG_HOME");
    if (xdg_config && xdg_config[0] != '\0')
    {
        return filesystem::path(xdg_config);
    }

    // fallback per spec: $HOME/.config
    const char *home = getenv("HOME");
    if (home)
    {
        return filesystem::path(home) / ".config";
    }

    return nullopt;
}

vector<AlbumEntry> getAlbums()
{
    vector<AlbumEntry> albums;

    optional<filesystem::path> xdg_config_directory = xdgConfigDir();
    if (!xdg_config_directory)
    {
        cout << "xdg config directory locate failed" << endl;
        return albums;
    }

    filesystem::path album_directory = *xdg_config_directory / "koji" / "albums";
    for (const auto &album : filesystem::recursive_directory_iterator(album_directory))
    {
        if (!filesystem::is_directory(album))
        {
            continue;
        }

        string            relative_album_path_string = filesystem::relative(album.path(), album_directory).string();
        string::size_type slash_positon              = relative_album_path_string.find('/');

        if (slash_positon == string::npos)
            continue;

        string artist      = relative_album_path_string.substr(0, slash_positon);
        string album_title = relative_album_path_string.substr(slash_positon + 1);

        AlbumEntry entry = {album.path(), album_title, artist};
        if (find(albums.begin(), albums.end(), entry) == albums.end())
        {
            albums.push_back(entry);
        }
    }

    return albums;
}

vector<PlaylistEntry> getPlaylists()
{
    vector<PlaylistEntry> playlists;

    optional<filesystem::path> xdg_config_directory = xdgConfigDir();
    if (!xdg_config_directory)
    {
        cout << "xdg config directory locate failed" << endl;
        return playlists;
    }
    filesystem::path playlists_directory = *xdg_config_directory / "koji" / "playlists";

    for (const auto &playlist : filesystem::directory_iterator(playlists_directory))
    {
        if (filesystem::is_directory(playlist))
        {
            continue;
        }

        string            relative_album_path_string = filesystem::relative(playlist.path(), playlists_directory).string();
        string::size_type dot_positon                = relative_album_path_string.find('.');
        string            playlist_title             = relative_album_path_string.substr(0, dot_positon);

        PlaylistEntry entry = {playlist.path(), playlist_title};
        if (find(playlists.begin(), playlists.end(), entry) == playlists.end())
        {
            playlists.push_back(entry);
        }
    }

    return playlists;
}

vector<SongEntry> getAlbumSongs(const AlbumEntry &album)
{
    vector<tuple<int, SongEntry>> tracks;
    for (const auto &song : filesystem::recursive_directory_iterator(album.path.c_str()))
    {
        if (filesystem::is_directory(song))
        {
            continue;
        }

        TagLib::FileRef song_file(song.path().c_str());

        int    track_index;
        string artist;
        string title;
        if (!song_file.isNull() && song_file.tag())
        {
            track_index = song_file.tag()->track();
            title       = song_file.tag()->title().to8Bit(true);
            artist      = song_file.tag()->artist().to8Bit(true);
        }
        else
            continue;

        float duration;
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
            duration = song_file.audioProperties()->lengthInSeconds();
        else
            continue;

        SongEntry entry = {song.path(), artist, album.title, title, duration};
        tracks.push_back({track_index, entry});
    }

    std::sort(tracks.begin(), tracks.end(), [](const tuple<int, SongEntry> &entry_a, const tuple<int, SongEntry> &entry_b) { return get<0>(entry_a) < get<0>(entry_b); });

    vector<SongEntry> songs;
    songs.reserve(tracks.size());

    for (const tuple<int, SongEntry> &track : tracks)
        songs.push_back(get<1>(track));

    return songs;
}

vector<SongEntry> getPlaylistSongs(const PlaylistEntry &playlist)
{
    vector<SongEntry> songs;

    ifstream playlist_file(playlist.path);

    if (!playlist_file)
        return songs;

    optional<filesystem::path> xdg_config_directory = xdgConfigDir();
    if (!xdg_config_directory)
    {
        cout << "xdg config directory locate failed" << endl;
        return songs;
    }

    filesystem::path songs_directory = *xdg_config_directory / "koji" / "playlists" / "songs";

    string playlist_song;

    while (getline(playlist_file, playlist_song))
    {
        filesystem::path song_path = songs_directory / playlist_song;

        TagLib::FileRef song_file(song_path.c_str());

        string title;
        string album;
        string artist;
        if (!song_file.isNull() && song_file.tag())
        {
            title  = song_file.tag()->title().to8Bit(true);
            album  = song_file.tag()->album().to8Bit(true);
            artist = song_file.tag()->artist().to8Bit(true);
        }
        else
            continue;

        float duration;
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
            duration = song_file.audioProperties()->lengthInSeconds();
        else
            continue;

        SongEntry entry = {song_path, artist, album, title, duration};
        songs.push_back(entry);
    }
    return songs;
}

bool initialize(PlayerStatus &status)
{
    status.albums    = getAlbums();
    status.playlists = getPlaylists();
    return true;
}
} // namespace koji_library

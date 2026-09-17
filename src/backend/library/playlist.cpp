// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include <fstream>
#include <iostream>
#include <optional>
#include <tuple>
#include <taglib/fileref.h>
#include "../../backend/utils/filesystem.h"
#include "playlists.h"

using namespace std;
using namespace koji::backend::utils;

namespace koji::backend::library
{
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
            continue;

        string            relative_album_path_string = filesystem::relative(playlist.path(), playlists_directory).string();
        string::size_type dot_positon                = relative_album_path_string.rfind('.');
        string            playlist_title             = relative_album_path_string.substr(0, dot_positon);

        PlaylistEntry entry = {playlist.path(), playlist_title};
        if (find(playlists.begin(), playlists.end(), entry) == playlists.end())
            playlists.push_back(entry);
    }

    return playlists;
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

        if (title.empty())
        {
            size_t last_period = song_path.string().rfind('.');
            size_t last_slash  = song_path.string().rfind('/');
            title              = song_path.string().substr(last_slash + 1, last_period - last_slash - 1);
        }

        float duration;
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
            duration = song_file.audioProperties()->lengthInSeconds();
        else
            continue;

        if (title.empty())
            title = "Unknown";

        if (album.empty())
            album = "Unknown";

        if (artist.empty())
            artist = "Unknown";

        SongEntry entry = {song_path, artist, album, title, duration};
        songs.push_back(entry);
    }
    return songs;
}

void savePlaylist(const PlaylistEntry &entry, const vector<SongEntry> playlist)
{
    
}

} // namespace koji::backend::library

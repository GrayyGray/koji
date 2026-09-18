// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include <fstream>
#include <iostream>
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

    filesystem::path xdg_config_directory = xdgConfigDir();
    if (xdg_config_directory.empty())
    {
        cout << "xdg config directory locate failed" << endl;
        return playlists;
    }
    filesystem::path playlists_directory = xdg_config_directory / "koji" / "playlists";

    for (const filesystem::directory_entry &folder : filesystem::directory_iterator(playlists_directory))
    {
        if (!filesystem::is_directory(folder))
            continue;

        const filesystem::path m3u   = folder.path() / (folder.path().filename().string() + ".m3u");
        const string           title = m3u.stem().string();
        const PlaylistEntry    entry = {m3u, title};

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

    filesystem::path songs_directory = playlist.path.parent_path();

    string song;
    while (getline(playlist_file, song))
    {
        string           title;
        string           album;
        string           artist;
        filesystem::path song_path;
        if (filesystem::exists(songs_directory / song))
            song_path = songs_directory / song;
        else
            song_path = song;

        TagLib::FileRef song_file(song_path.c_str());

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
            title = song_path.stem().string();
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

void duplicatePlaylist(const PlaylistEntry &entry) {}

void savePlaylist(const PlaylistEntry &entry, const vector<SongEntry> &playlist)
{
    filesystem::path xdg_config_directory = xdgConfigDir();
    if (xdg_config_directory.empty())
    {
        cout << "xdg config directory locate failed" << endl;
        return;
    }

    ofstream playlist_file(entry.path, ios::trunc);

    if (!playlist_file.is_open())
        return;

    filesystem::path songs_directory = entry.path.parent_path();

    for (const auto &song : playlist)
    {
        const string song_filename = song.path.filename().string();
        if (filesystem::exists(songs_directory / song_filename))
            playlist_file << song_filename << endl;
        else
            playlist_file << song.path.string() << endl;
    }

    playlist_file.close();
}

} // namespace koji::backend::library

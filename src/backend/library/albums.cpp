// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "albums.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <tuple>
#include <taglib/fileref.h>
#include "../../backend/utils/filesystem.h"

using namespace std;
using namespace koji::backend::utils;

namespace koji::backend::library
{
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
            continue;

        string            relative_album_path_string = filesystem::relative(album.path(), album_directory).string();
        string::size_type slash_positon              = relative_album_path_string.find('/');

        if (slash_positon == string::npos)
            continue;

        string artist      = relative_album_path_string.substr(0, slash_positon);
        string album_title = relative_album_path_string.substr(slash_positon + 1);

        AlbumEntry entry = {album.path(), album_title, artist};
        if (find(albums.begin(), albums.end(), entry) == albums.end())
            albums.push_back(entry);
    }

    return albums;
}

vector<SongEntry> getAlbumSongs(const AlbumEntry &album)
{
    vector<tuple<int, SongEntry>> tracks;
    for (const auto &song : filesystem::recursive_directory_iterator(album.path.c_str()))
    {
        if (filesystem::is_directory(song))
            continue;

        string song_path = song.path().c_str();

        TagLib::FileRef song_file(song_path.c_str());

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

        if (title.empty())
        {
            size_t last_period = song_path.rfind('.');
            size_t last_slash  = song_path.rfind('/');
            title              = song_path.substr(last_slash + 1, last_period - last_slash - 1);
        }

        float duration;
        if (!song_file.isNull() && song_file.audioProperties() != nullptr)
            duration = song_file.audioProperties()->lengthInSeconds();
        else
            continue;

        if (title.empty())
            title = "Unknown";

        if (artist.empty())
            artist = "Unknown";

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
} // namespace koji::backend::library
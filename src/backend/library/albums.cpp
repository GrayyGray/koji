// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "albums.h"
#include <iostream>
#include <algorithm>
#include <taglib/fileref.h>
#include "../../backend/utils/filesystem.h"

using namespace std;
using namespace koji::backend::utils;

namespace koji::backend::library
{

struct TrackEntry
{
    int track_number;
    SongEntry song;
};

vector<AlbumEntry> getAlbums()
{
    vector<AlbumEntry> albums;

    filesystem::path xdg_config_directory = xdgConfigDir();
    if (xdg_config_directory.empty())
    {
        cout << "xdg config directory locate failed" << endl;
        return albums;
    }
    filesystem::path album_directory = xdg_config_directory / "koji" / "albums";

    for (const filesystem::directory_entry &artist : filesystem::directory_iterator(album_directory))
    {
        if (!filesystem::is_directory(artist))
            continue;
        
        for (const filesystem::directory_entry &album : filesystem::directory_iterator(artist))
        {
            if (!filesystem::is_directory(album))
                continue;
            
            const AlbumEntry entry = {album.path(), album.path().filename().string(), artist.path().filename().string()};
            albums.push_back(entry);
        }
    }

    return albums;
}

vector<SongEntry> getAlbumSongs(const AlbumEntry &album)
{
    vector<TrackEntry> tracks;
    for (const filesystem::directory_entry &song : filesystem::directory_iterator(album.path))
    {
        if (!filesystem::is_regular_file(song))
            continue;

        TagLib::FileRef song_file(song.path().string().c_str());

        string title;
        string artist;
        int    track_index;
    
        if (!song_file.isNull() && song_file.tag())
        {
            track_index = song_file.tag()->track();
            title       = song_file.tag()->title().to8Bit(true);
            artist      = song_file.tag()->artist().to8Bit(true);
        }
        else
            continue;

        if (title.empty())
            title              = song.path().stem().string();

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
    
    std::sort(tracks.begin(), tracks.end(), [](const TrackEntry& a, const TrackEntry& b){ return a.track_number < b.track_number; });

    vector<SongEntry> songs;
    songs.reserve(tracks.size());

    for (const TrackEntry &track : tracks)
        songs.push_back(track.song);

    return songs;
}
} // namespace koji::backend::library
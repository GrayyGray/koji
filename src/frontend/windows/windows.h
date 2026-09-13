// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "backend/app.h"
#include "backend/player.h"

namespace koji_frontend
{

struct EditorState
{
    koji_player::SongEntry song_to_append = {};
    koji_player::AlbumEntry album_to_append = {};
    koji_player::PlaylistEntry playlist_to_edit = {};
};

void editorWindow(koji_app::AppState &state, koji_player::PlayerStatus &status);
} // namespace koji_frontend

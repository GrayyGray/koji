// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <SDL3/SDL.h>
#include "imgui.h"

namespace koji_app
{
struct AppState
{
    ImGuiIO        *io;
    const char     *title;
    SDL_Window     *window;
    SDL_Renderer   *renderer;
    SDL_WindowFlags window_flags;
    int             width, height;
    float           display_content_scale;
    bool            edit_playlist_window = false;
};

bool initialize(AppState &state);

bool pollEvents(const AppState &state);

void cleanup(AppState &state);
} // namespace koji_app

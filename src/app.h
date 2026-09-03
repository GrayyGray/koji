// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <SDL3/SDL.h>

#include "imgui.h"

struct AppState
{
    ImGuiIO        *io;
    const char     *title;
    int             width, height;
    int             scaledWidth, scaledHeight;
    SDL_Window     *window;
    SDL_Renderer   *renderer;
    float           display_content_scale;
    SDL_WindowFlags window_flags;
};
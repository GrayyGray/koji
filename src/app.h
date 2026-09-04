// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <SDL3/SDL.h>

#include "imgui.h"

struct AppState
{
    ImGuiIO        *io;
    const char     *title;
    SDL_Window     *window;
    SDL_Renderer   *renderer;
    SDL_WindowFlags window_flags;
    int             width, height;
    float           display_content_scale;
    int             scaledWidth, scaledHeight;
};

bool initialize(AppState &state);

bool pollEvents(const AppState &state);

void beginMainWindow(const AppState &state);
void endMainWindow(const AppState &state);

bool beginTab(const char *label);
void endTab();

void cleanup(AppState &state);
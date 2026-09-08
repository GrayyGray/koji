// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "app.h"
#include <algorithm>
#include <random>
#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "player.h"

namespace koji_app
{
bool initialize(AppState &state)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        return false;
    }

    state.display_content_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    state.scaledWidth           = (int)(state.width * state.display_content_scale);
    state.scaledHeight          = (int)(state.height * state.display_content_scale);

    state.window = SDL_CreateWindow(state.title, state.scaledWidth, state.scaledHeight, state.window_flags);
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", state.window);
        cleanup(state);
        return false;
    }

    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
        cleanup(state);
        return false;
    }

    SDL_SetRenderVSync(state.renderer, 1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetStyle().TabBarBorderSize = 0.0f;
    state.io                           = &ImGui::GetIO();
    state.io->IniFilename              = nullptr;
    state.io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(state.display_content_scale);
    style.FontScaleDpi = state.display_content_scale;

    ImGui_ImplSDL3_InitForSDLRenderer(state.window, state.renderer);
    ImGui_ImplSDLRenderer3_Init(state.renderer);

    state.io->Fonts->AddFontFromFileTTF("assets/GoNotoCurrent-Regular.ttf", 18.0f);

    SDL_ShowWindow(state.window);
    return true;
}

bool pollEvents(const AppState &state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        SDL_ConvertEventToRenderCoordinates(state.renderer, &event);
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            return false;

        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(state.window))
            return false;
    }

    if (SDL_GetWindowFlags(state.window) & SDL_WINDOW_MINIMIZED)
        SDL_Delay(10);

    return true;
}

void cleanup(AppState &state)
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}
} // namespace koji_app

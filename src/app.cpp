// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "app.h"

#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

bool initialize(AppState &state)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error initializing SDL3", nullptr);
        return false;
    }

    // setup SDL3 scaling
    state.display_content_scale =
        SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    state.scaledWidth  = (int)(state.width * state.display_content_scale);
    state.scaledHeight = (int)(state.height * state.display_content_scale);

    state.window =
        SDL_CreateWindow(state.title, state.scaledWidth, state.scaledHeight, 0);
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error creating window", state.window);
        cleanup(state);
        return false;
    }

    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error",
                                 "Error creating renderer", state.window);
        cleanup(state);
        return false;
    }

    SDL_SetRenderVSync(state.renderer, 1);
    // SDL_SetWindowPosition(state.window, SDL_WINDOWPOS_CENTERED,
    // SDL_WINDOWPOS_CENTERED); // broken on wayland
    // SDL_SetRenderLogicalPresentation(state.renderer, state.width, state.height,
    // SDL_LOGICAL_PRESENTATION_LETTERBOX); // scaling set elsewhere

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::GetStyle().TabBarBorderSize = 0.0f;
    state.io                           = &ImGui::GetIO();
    state.io->IniFilename              = nullptr;
    state.io->ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); // or ImGui::StyleColorsLight();

    // Setup imgui scaling

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(
        state.display_content_scale); // Bake a fixed style scale. (until we have
                                      // a solution for dynamic style scaling,
                                      // changing this requires resetting Style +
                                      // calling this again)
    style.FontScaleDpi =
        state.display_content_scale; // Set initial font scale. (in docking
                                     // branch: using io.ConfigDpiScaleFonts=true
                                     // automatically overrides this for every
                                     // window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(state.window, state.renderer);
    ImGui_ImplSDLRenderer3_Init(state.renderer);

    state.io->Fonts->AddFontFromFileTTF("assets/NotoSans-Regular.ttf", 18.0f);

    SDL_ShowWindow(state.window);
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

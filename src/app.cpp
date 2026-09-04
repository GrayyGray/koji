// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "app.h"

#include <SDL3/SDL.h>

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
    state.display_content_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    state.scaledWidth           = (int)(state.width * state.display_content_scale);
    state.scaledHeight          = (int)(state.height * state.display_content_scale);

    state.window = SDL_CreateWindow(state.title, state.scaledWidth, state.scaledHeight, 0);
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

bool pollEvents(const AppState &state)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
        {
            return false;
        }

        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
            event.window.windowID == SDL_GetWindowID(state.window))
        {
            return false;
        }
    }

    if (SDL_GetWindowFlags(state.window) & SDL_WINDOW_MINIMIZED)
    {
        SDL_Delay(10);
    }

    return true;
}

void beginMainWindow(const AppState &state)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(state.io->DisplaySize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Begin("mainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings);
}

void endMainWindow(const AppState &state)
{
    ImGui::PopStyleColor();
    ImGui::End();
    ImGui::Render();
    SDL_SetRenderScale(state.renderer, state.io->DisplayFramebufferScale.x, state.io->DisplayFramebufferScale.y);
    SDL_RenderClear(state.renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer);
    SDL_RenderPresent(state.renderer);
}

bool beginTab(const char *label)
{
    if (!ImGui::BeginTabItem(label, nullptr, ImGuiTabItemFlags_NoArrowNav))
        return false;

    ImGui::Spacing();
    ImGui::BeginChild("mainBrowser", ImVec2(0, -65), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

    return true;
}

void endTab()
{
    ImGui::PopItemFlag();
    ImGui::EndChild();
    ImGui::EndTabItem();
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

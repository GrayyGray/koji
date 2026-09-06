// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "app.h"
#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "player.h"

bool initialize(AppState &state)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
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
    state.io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark(); // or ImGui::StyleColorsLight();

    // Setup imgui scaling

    ImGuiStyle &style = ImGui::GetStyle();
    style.ScaleAllSizes(state.display_content_scale); // Bake a fixed style scale. (until we have
                                                      // a solution for dynamic style scaling,
                                                      // changing this requires resetting Style +
                                                      // calling this again)
    style.FontScaleDpi = state.display_content_scale; // Set initial font scale. (in docking
                                                      // branch: using io.ConfigDpiScaleFonts=true
                                                      // automatically overrides this for every
                                                      // window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(state.window, state.renderer);
    ImGui_ImplSDLRenderer3_Init(state.renderer);

    state.io->Fonts->AddFontFromFileTTF("assets/GoNotoCurrent-Regular.ttf", 18.0f);

    SDL_ShowWindow(state.window);
    return true;
}

bool pollEvents(const AppState &state, koji_player::PlayerStatus &status)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT)
            return false;

        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(state.window))
            return false;
    }

    if (SDL_GetWindowFlags(state.window) & SDL_WINDOW_MINIMIZED)
        SDL_Delay(10);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Q))
        return false;

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X))
        status.current_song = {};

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space))
        status.paused = !status.paused;

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        status.shuffle = !status.shuffle;

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        status.repeat_mode =  static_cast<koji_player::RepeatMode>((static_cast<int>(status.repeat_mode ) + 1) % static_cast<int>(koji_player::RepeatMode::Track) + 1);
    
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

struct KeyBinding
{
    const char *key;
    const char *label;
};

void renderPlayer(const koji_player::PlayerStatus &status)
{
    ImGui::SameLine();
    ImGui::Text("%s %s", status.current_song == koji_player::SongEntry{} ? "⏹" : status.paused ? "⏸" : "⯈", status.current_song.title.empty() ? "nothing playing" : status.current_song.title.c_str());
    ImGui::SameLine();

    float       playing_progress = (status.current_song != koji_player::SongEntry{} && status.current_song.duration > 0.0f) ? (status.position_seconds / status.current_song.duration) : 0.0f;
    std::string duration         = status.current_song != koji_player::SongEntry{} ? koji_player::formatTime(status.current_song.duration) : "--:--";
    std::string overlay          = koji_player::formatTime(status.position_seconds) + "/" + duration;

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::ProgressBar(playing_progress, ImVec2(200, 20), overlay.c_str());
    ImGui::PopStyleVar();
    ImGui::SameLine();

    ImGui::Text("Vol:%d%%", status.volume);
    ImGui::SameLine();

    ImGui::TextUnformatted(status.shuffle ? "Shuf:On" : "Shuf:Off");
    ImGui::SameLine();

    const char *repeat_mode = status.repeat_mode == koji_player::RepeatMode::Off ? "Rep:Off" : status.repeat_mode == koji_player::RepeatMode::All ? "Rep:All" : "Rep:Trk";
    ImGui::TextUnformatted(repeat_mode);

    ImGui::Separator();
    ImGui::Text("shift+click: add to queue   tab: cycle tabs   s: shuffle   r: repeat   space: play/pause   x: stop   q: quit");
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

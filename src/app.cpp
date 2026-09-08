// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "app.h"
#include <algorithm>
#include <random>
#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "player.h"

bool initializeApp(AppState &state)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        return false;
    }

    state.display_content_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    state.scaledWidth           = (int)(state.width * state.display_content_scale);
    state.scaledHeight          = (int)(state.height * state.display_content_scale);

    state.window = SDL_CreateWindow(state.title, state.scaledWidth, state.scaledHeight, 0);
    if (!state.window)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating window", state.window);
        cleanupApp(state);
        return false;
    }

    state.renderer = SDL_CreateRenderer(state.window, nullptr);
    if (!state.renderer)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating renderer", state.window);
        cleanupApp(state);
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

void togglePause(koji_player::PlayerStatus &status)
{
    status.paused = !status.paused;
    koji_player::updatePlayerPause(status);
}

void toggleShuffle(koji_player::PlayerStatus &status)
{
    status.shuffle = !status.shuffle;

    if (status.shuffle)
    {
        status.unshuffled_queue = status.queue;
        status.random_engine.seed(std::random_device{}());
        std::ranges::shuffle(status.queue, status.random_engine);
    }
    else
    {
        status.queue = status.unshuffled_queue;
        status.unshuffled_queue.clear();
    }
}

void toggleRepeatMode(koji_player::RepeatMode &repeat_mode)
{
    if (repeat_mode == koji_player::RepeatMode::Off)
    {
        repeat_mode = koji_player::RepeatMode::All;
    }
    else if (repeat_mode == koji_player::RepeatMode::All)
    {
        repeat_mode = koji_player::RepeatMode::Track;
    }
    else if (repeat_mode == koji_player::RepeatMode::Track)
    {
        repeat_mode = koji_player::RepeatMode::Off;
    }
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
    {
        koji_player::stopSong(status);
        status.current_song = {};
    }

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space))
        togglePause(status);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        toggleShuffle(status);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        toggleRepeatMode(status.repeat_mode);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Minus) && status.volume - 5 >= 0)
    {
        status.volume -= 5;
        koji_player::updatePlayerVolume(status);
    }

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Equal) && status.volume + 5 <= 100)
    {
        status.volume += 5;
        koji_player::updatePlayerVolume(status);
    }
    
    if (!koji_player::pollEvents(status))
        return false;
    
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

    const char *shuffle_mode = status.shuffle ? "Shuf:On" : "Shuf:Off";
    const char *repeat_mode = status.repeat_mode == koji_player::RepeatMode::Off ? "Rep:Off" : status.repeat_mode == koji_player::RepeatMode::All ? "Rep:All" : "Rep:Trk";

    std::string volume_percentage = ("Vol:" + std::to_string(status.volume) + "%");
    std::string position_time = status.current_song != koji_player::SongEntry{} ? koji_player::formatTime(status.position_seconds) : "--:--";
    std::string duration_time = status.current_song != koji_player::SongEntry{} ? koji_player::formatTime(status.current_song.duration) : "--:--";

    float       playing_progress = (status.current_song != koji_player::SongEntry{} && status.current_song.duration > 0.0f) ? (status.position_seconds / status.current_song.duration) : 0.0f;
    float right_segment_width = ImGui::CalcTextSize((position_time + "/" + duration_time).c_str()).x + 200.0f + ImGui::CalcTextSize(volume_percentage.c_str()).x + ImGui::CalcTextSize(shuffle_mode).x + ImGui::CalcTextSize(repeat_mode).x +ImGui::GetStyle().ItemSpacing.x * 3.0f;

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - right_segment_width);

    ImGui::Text("%s/%s", position_time.c_str(), duration_time.c_str());
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::ProgressBar(playing_progress, ImVec2(200, 20), "");
    ImGui::PopStyleVar();
    ImGui::SameLine();

    ImGui::TextUnformatted(volume_percentage.c_str());
    ImGui::SameLine();

    ImGui::TextUnformatted(shuffle_mode);
    ImGui::SameLine();

    
    ImGui::TextUnformatted(repeat_mode);

    ImGui::Separator();
    ImGui::Text("shift+click: add to queue   tab: cycle tabs   s: shuffle   r: repeat   space: play/pause   x: stop   q: quit");
}

void cleanupApp(AppState &state)
{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

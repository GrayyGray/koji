// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#pragma once

#include <filesystem>
#include <random>
#include <string>
#include <vector>
#include <mpv/client.h>
#include "../backend/library/entries.h"
#include "../backend/library/albums.h"
#include "../backend/library/playlists.h"
#include <SDL3/SDL.h>
#include "imgui.h"
#include <algorithm>
#include <SDL3/SDL.h>
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include <iterator>
#include "app.h"

// NOLINTBEGIN(readability-identifier-naming)
extern const unsigned char _binary_dependencies_assets_GoNotoCurrent_Regular_ttf_start[];
extern const unsigned char _binary_dependencies_assets_GoNotoCurrent_Regular_ttf_end[];
// NOLINTEND(readability-identifier-naming)

using namespace std;
using namespace koji::backend::library;

namespace koji::backend::app
{
bool initialize(AppState &state)
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing SDL3", nullptr);
        return false;
    }

    state.display_content_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    int scaled_width            = (int)(state.width * state.display_content_scale) * 2;
    int scaled_height           = (int)(state.height * state.display_content_scale) * 2;

    state.window = SDL_CreateWindow(state.title, scaled_width, scaled_height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
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

    ImFontConfig font_config{};
    font_config.FontDataOwnedByAtlas = false;
    state.io->Fonts->AddFontFromMemoryTTF(const_cast<unsigned char *>(_binary_dependencies_assets_GoNotoCurrent_Regular_ttf_start), _binary_dependencies_assets_GoNotoCurrent_Regular_ttf_end - _binary_dependencies_assets_GoNotoCurrent_Regular_ttf_start, 36.0f, &font_config);

    SDL_ShowWindow(state.window);

    state.player_context.mpv_context = mpv_create();

    if (!state.player_context.mpv_context)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error creating mpv context", state.window);
        cleanup(state);
        return false;
    }

    if (mpv_initialize(state.player_context.mpv_context) != 0)
    {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", "Error initializing mpv", state.window);
        cleanup(state);
        return false;
    }

    mpv_set_option_string(state.player_context.mpv_context, "vo", "null");
    mpv_set_option_string(state.player_context.mpv_context, "audio-format", "s16le");
    updateVolume(state);

    state.player_context.albums    = getAlbums();
    state.player_context.playlists = getPlaylists();


    return true;
}

void cycleSong(AppState &state)
{
    int song_index;
    vector<SongEntry>::const_iterator iterator = find(state.player_context.queue.begin(), state.player_context.queue.end(), state.player_context.current_song);

    if (iterator == state.player_context.queue.end())
        song_index = -1;
    else
        song_index = distance(state.player_context.queue.cbegin(), iterator);
    
    if (song_index + 1 >= state.player_context.queue.size())
        state.player_context.current_song = state.player_context.queue[0];
    else
        state.player_context.current_song = state.player_context.queue[song_index + 1];

    updateCurrentSong(state);
}

bool songCycle(AppState &state)
{
    if (state.player_context.current_song == SongEntry{})
        return true;

    double time_remaining;
    mpv_get_property(state.player_context.mpv_context, "time-pos", MPV_FORMAT_DOUBLE, &time_remaining);
    state.player_context.position_seconds = static_cast<float>(time_remaining);

    mpv_event *event = mpv_wait_event(state.player_context.mpv_context, 0);

    if (event->event_id == MPV_EVENT_END_FILE)
    {
        mpv_event_end_file *end_file = static_cast<mpv_event_end_file *>(event->data);

        if (end_file->reason == MPV_END_FILE_REASON_EOF)
            cycleSong(state);
    }

    return true;
}

bool keyCycle(AppState &state)
{
    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Q))
        return false;

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_X))
        stopSong(state);
        state.player_context.current_song = {};

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Space))
        togglePause(state);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_S))
        toggleShuffle(state);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_R))
        toggleRepeatMode(state.player_context.repeat_mode);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Minus) && state.player_context.volume - 5 >= 0)
        updateVolume(state, -5);

    if (ImGui::IsKeyPressed(ImGuiKey::ImGuiKey_Equal) && state.player_context.volume + 5 <= 100)
        updateVolume(state, 5);

    return true;
}

bool pollEvents(AppState &state)
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


    if (!songCycle(state))
        return false;

    if (!keyCycle(state))
        return false;

    return true;
}

void cleanup(AppState &state)
{
    mpv_destroy(state.player_context.mpv_context);

    state.io->Fonts->Clear();
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(state.renderer);
    SDL_DestroyWindow(state.window);
    SDL_Quit();
}

void updateCurrentSong(AppState &state)
{
    state.player_context.paused = false;
    updatePause(state);
    const char *play_command[] = {"loadfile", state.player_context.current_song.path.c_str(), "replace", nullptr};
    mpv_command(state.player_context.mpv_context, play_command);
}

void updatePause(AppState &state)
{
    if (state.player_context.paused)
        mpv_set_property_string(state.player_context.mpv_context, "pause", "yes");
    else
        mpv_set_property_string(state.player_context.mpv_context, "pause", "no");
}

void updateVolume(AppState &state, const int level = 0)
{ 
    mpv_set_property_string(state.player_context.mpv_context, "volume", to_string(state.player_context.volume + level).c_str()); 
}

void stopSong(AppState &state)
{
    state.player_context.paused = true;
    updatePause(state);
    mpv_command_string(state.player_context.mpv_context, "stop");
}

void togglePause(AppState &state)
{
    state.player_context.paused = !state.player_context.paused;
    updatePause(state);
}

void toggleShuffle(AppState &state)
{
    state.player_context.shuffle = !state.player_context.shuffle;

    if (state.player_context.shuffle)
    {
        state.player_context.unshuffled_queue = state.player_context.queue;
        state.player_context.random_engine.seed(random_device{}());
        ranges::shuffle(state.player_context.queue, state.player_context.random_engine);
    }
    else
    {
        state.player_context.queue = state.player_context.unshuffled_queue;
        state.player_context.unshuffled_queue.clear();
    }
}

void toggleRepeatMode(RepeatMode &repeat_mode)
{
    if (repeat_mode == RepeatMode::Off)
    {
        repeat_mode = RepeatMode::All;
    }
    else if (repeat_mode == RepeatMode::All)
    {
        repeat_mode = RepeatMode::Track;
    }
    else if (repeat_mode == RepeatMode::Track)
    {
        repeat_mode = RepeatMode::Off;
    }
}

void addSongsToQueue(AppState &state, vector<SongEntry> &songs)
{
    if (state.player_context.shuffle)
    {
        state.player_context.unshuffled_queue.insert(state.player_context.unshuffled_queue.end(), songs.begin(), songs.end());
        ranges::shuffle(songs, state.player_context.random_engine);
    }

    if (state.player_context.queue.empty())
    {
        state.player_context.current_song = songs[0];
        updateCurrentSong(state);
    }

    state.player_context.queue.insert(state.player_context.queue.end(), songs.begin(), songs.end());
}
} // namespace app

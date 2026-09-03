// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include <filesystem>
#include <optional>
#include <vector>

#include <SDL3/SDL.h>
#include <stdio.h>

#include "app.h"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "library.h"
#include "tabs.h"

int main(int, char **)
{

    AppState state;
    state.window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN;
    state.width        = 1280;
    state.height       = 720;
    state.title        = "koji";

    if (!initialize(state))
    {
        return 1;
    }

    std::vector<AlbumEntry> albums = getAlbums();
    std::vector<SongEntry>  songs  = getAlbumSongs(albums);
    std::vector<SongEntry>  queue;

    bool done = false;
    while (!done)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL3_ProcessEvent(&event);
            if (event.type == SDL_EVENT_QUIT)
            {
                done = true;
                break;
            }

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(state.window))
            {
                done = true;
                break;
            }
        }

        // when minimized wait ten miliseconds before doing anything else (saves
        // cpu)
        if (SDL_GetWindowFlags(state.window) & SDL_WINDOW_MINIMIZED)
        {
            SDL_Delay(10);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        {
            // Fullscreen
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(state.io->DisplaySize);

            // Set backround to black
            ImGui::PushStyleColor(ImGuiCol_WindowBg,
                                  ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // RGBA values

            // Make a new window
            ImGui::Begin("mainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings);

            if (ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Queue", nullptr,
                                        ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::Spacing();
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0),
                                      ImGuiChildFlags_NavFlattened,
                                      ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

                    songQueueTab(queue);

                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Albums", nullptr,
                                        ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::Spacing();
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0),
                                      ImGuiChildFlags_NavFlattened,
                                      ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

                    albumSelectionTab(queue, albums);

                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Playlists", nullptr,
                                        ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::Spacing();
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0),
                                      ImGuiChildFlags_NavFlattened,
                                      ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Download", nullptr,
                                        ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::Spacing();
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0),
                                      ImGuiChildFlags_NavFlattened,
                                      ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                ImGui::EndTabBar();
            }
            ImGui::PopStyleColor();
            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        SDL_SetRenderScale(state.renderer, state.io->DisplayFramebufferScale.x,
                           state.io->DisplayFramebufferScale.y);

        SDL_RenderClear(state.renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer);
        SDL_RenderPresent(state.renderer);
    }

    cleanup(state);
    return 0;
}

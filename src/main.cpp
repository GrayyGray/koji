// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include <filesystem>
#include <optional>
#include <vector>

#include <SDL3/SDL.h>
#include <stdio.h>

#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "library.h"

using namespace std;

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


void albumSelectionTab(std::vector<SongEntry> &queue, const std::vector<AlbumEntry> &albums)
{
    if (ImGui::BeginTable("albumSelectionTab", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Album");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < albums.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(albums[i].artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                printf("%d\n", i);

                std::vector<SongEntry> album_songs = getAlbumSongs({albums[i]});
                queue.insert(queue.end(), album_songs.begin(), album_songs.end());
            }
            ImGui::TableNextColumn();
            ImGui::Text("%s", albums[i].album_title.c_str());

            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}

void songQueueTab(std::vector<SongEntry> &queue)
{
    if (ImGui::BeginTable("songQueueTab", 4, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders))
    {
        ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
        ImGui::TableSetupColumn("Artist");
        ImGui::TableSetupColumn("Title");
        ImGui::TableSetupColumn("Album");
        ImGui::TableSetupColumn("Length");

        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
        ImGui::TableHeadersRow();
        ImGui::PopStyleColor();
        ImGui::PopItemFlag();

        for (int i = 0; i < queue.size(); i++)
        {
            ImGui::PushID(i);
            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            if (ImGui::Selectable(queue[i].album.artist.c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
            {
                printf("%d\n", i);
                // OnSongClicked(i);
            }

            ImGui::TableNextColumn();
            ImGui::Text("%s", queue[i].title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", queue[i].album.album_title.c_str());

            ImGui::TableNextColumn();
            ImGui::Text("%s", queue[i].duration.c_str());
            ImGui::PopID();
        }
        ImGui::EndTable();
    }
}



bool initialize(AppState &state);

void cleanup(AppState &state);

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

    vector<AlbumEntry> albums = getAlbums();
    vector<SongEntry>  songs  = getAlbumSongs(albums);
    vector<SongEntry>  queue;

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

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(state.window))
            {
                done = true;
                break;
            }
        }

        // when minimized wait ten miliseconds before doing anything else (saves cpu)
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
            ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f)); // RGBA values

            // Make a new window
            ImGui::Begin("mainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings);
            ImGui::Spacing();
            if (ImGui::BeginTabBar("tabBar", ImGuiTabBarFlags_None))
            {
                if (ImGui::BeginTabItem("Queue", nullptr, ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

                    songQueueTab(queue);

                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Albums", nullptr, ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

                    albumSelectionTab(queue, albums);

                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Playlists", nullptr, ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
                    ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);
                    ImGui::PopItemFlag();
                    ImGui::EndChild();
                    ImGui::EndTabItem();
                }
                if (ImGui::BeginTabItem("Download", nullptr, ImGuiTabItemFlags_NoArrowNav))
                {
                    ImGui::BeginChild("mainBrowser", ImVec2(0, 0), ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_NoNavFocus);
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
        SDL_SetRenderScale(state.renderer, state.io->DisplayFramebufferScale.x, state.io->DisplayFramebufferScale.y);

        SDL_RenderClear(state.renderer);
        ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer);
        SDL_RenderPresent(state.renderer);
    }

    cleanup(state);
    return 0;
}

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
    // SDL_SetWindowPosition(state.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED); // broken on wayland
    // SDL_SetRenderLogicalPresentation(state.renderer, state.width, state.height, SDL_LOGICAL_PRESENTATION_LETTERBOX); // scaling set elsewhere

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
    style.ScaleAllSizes(state.display_content_scale); // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = state.display_content_scale; // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

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
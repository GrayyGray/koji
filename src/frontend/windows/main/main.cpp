// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray

#include "main.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

using namespace koji::backend::app;

namespace koji::frontend::windows::main
{
void beginMainWindow(const AppState &state)
{
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();

    state.io->DisplaySize = ImVec2(state.width * 2, state.height * 2);

    ImGui::NewFrame();
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(state.io->DisplaySize);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
    ImGui::Begin("mainWindow", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiTableFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
}

void endMainWindow(const  AppState &state)
{
    ImGui::PopStyleColor();
    ImGui::End();
    ImGui::Render();

    SDL_SetRenderLogicalPresentation(state.renderer, state.width * 2, state.height * 2, SDL_LOGICAL_PRESENTATION_STRETCH);

    SDL_RenderClear(state.renderer);
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), state.renderer);
    SDL_RenderPresent(state.renderer);
}
} // namespace koji::frontend::windows::main


// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2026 silver_gray
#include "tabs.h"
#include "../backend/library.h"
#include "../backend/utils.h"
#include "imgui.h"
#include "variant"

using namespace std;
using namespace koji_app;
using namespace koji_player;
using namespace koji_library;

namespace koji_ui
{

typedef variant<vector<SongEntry>, vector<AlbumEntry>, vector<PlaylistEntry>> ItemEntry;

void processCollectionEntry(AppState &state, PlayerStatus &status, string id, const int index)
{
    vector<SongEntry> songs;
    if (id == "album")
        songs = getAlbumSongs(status.albums[index]);
    else
        songs = getPlaylistSongs(status.playlists[index]);

    addSongsToQueue(status, songs);
}

void removeFromQueueButton(PlayerStatus &status, int index, float width)
{
    if (ImGui::Button("Remove from queue", ImVec2(width, 0)))
    {
        status.queue.erase(status.queue.begin() + index);
    }
}

void appendToQueueButton(AppState &state, PlayerStatus &status, string id, int index, float width)
{
    if (ImGui::Button("Append to queue", ImVec2(width, 0)))
        processCollectionEntry(state, status, id, index);
}

void editPlaylistButton(float width)
{
    if (ImGui::Button("Edit Playlist", ImVec2(width, 0)))
    {
        // editPlaylistPopup()
    }
}

void handleTableRow(AppState &state, PlayerStatus &status, string id, const vector<string> &headers, const int index, const vector<string> texts)
{
    const ImVec4 selected_background_color = koji_utils::darkenColor(ImGui::GetStyleColorVec4(ImGuiCol_HeaderHovered), 0.2f);

    ImGui::PushID(index);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    if (ImGui::Selectable(texts[0].c_str(), false, ImGuiSelectableFlags_SpanAllColumns))
    {
        if (id == "queue")
        {
            status.current_song = status.queue[index];
            updateCurrentSong(status);
        }
        else
        {
            if (!state.io->KeyShift && !status.queue.empty())
                status.queue.clear();
            processCollectionEntry(state, status, id, index);
        }
    }

    if (ImGui::BeginPopupContextItem())
    {
        float width = ImGui::GetContentRegionAvail().x;

        if (id == "queue")
            removeFromQueueButton(status, index, width);
        else
            appendToQueueButton(state, status, id, index, width);

        // if (id == "playlist")
        // editPlaylistButton(width);
        ImGui::EndPopup();
    }

    if (id == "queue" && index < status.queue.size() && status.queue[index] == status.current_song)
        ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, ImGui::ColorConvertFloat4ToU32(selected_background_color));

    for (int i = 1; i < headers.size(); i++)
    {
        ImGui::TableNextColumn();
        ImGui::Text("%s", texts[i].c_str());
    }

    ImGui::PopID();
}

void tab(AppState &state, PlayerStatus &status, string id)
{
    ItemEntry      entry;
    vector<string> headers;
    vector<string> texts;

    if (id == "queue")
        headers = {"Artist", "Title", "Album", "Duration"};
    else if (id == "album")
        headers = {"Artist", "Album"};
    else if (id == "playlist")
        headers = {"Playlist"};

    ImGui::BeginTable(id.c_str(), static_cast<int>(headers.size()), ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders);

    ImGui::PushItemFlag(ImGuiItemFlags_NoArrowNav, true);
    for (const string &header : headers)
    {
        ImGui::TableSetupColumn(header.c_str());
    }

    ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImGui::GetStyleColorVec4(ImGuiCol_TableHeaderBg));
    ImGui::TableHeadersRow();
    ImGui::PopStyleColor();
    ImGui::PopItemFlag();

    if (id == "queue")
    {
        for (int index = 0; index < status.queue.size(); index++)
        {
            texts.clear();
            texts.push_back(status.queue[index].artist);
            texts.push_back(status.queue[index].title);
            texts.push_back(status.queue[index].album);
            texts.push_back(koji_utils::formatTime(status.queue[index].duration));
            handleTableRow(state, status, id, headers, index, texts);
        }
    }
    else if (id == "album")
    {
        for (int index = 0; index < status.albums.size(); index++)
        {
            texts.clear();
            texts.push_back(status.albums[index].artist);
            texts.push_back(status.albums[index].title);
            handleTableRow(state, status, id, headers, index, texts);
        }
    }
    else if (id == "playlist")
    {
        for (int index = 0; index < status.playlists.size(); index++)
        {
            texts.clear();
            texts.push_back(status.playlists[index].title);
            handleTableRow(state, status, id, headers, index, texts);
        }
    }

    ImGui::EndTable();
}
} // namespace koji_ui

when right clicking on a song in the queue give an option to add to a playlist
when right clicking on a playlsit give the option to edit it (pops up a window? or list or something?)
when right clicking on an album add it to the queue without clearing

prob use something in this code for a popup menu in there or something 
```
{
    const char* names[5] = { "Label1", "Label2", "Label3", "Label4", "Label5" };
    static int selected = -1;
    for (int n = 0; n < 5; n++)
    {
        if (ImGui::Selectable(names[n], selected == n))
            selected = n;
        if (ImGui::BeginPopupContextItem()) // <-- use last item id as popup id
        {
            selected = n;
            ImGui::Text("This is a popup for \"%s\"!", names[n]);
            if (ImGui::Button("Close"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::SetItemTooltip("Right-click to open popup");
    }
}
```

add download tab using spot-dl or smth


idek ~v~

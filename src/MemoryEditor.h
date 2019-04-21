#pragma once

// Mini memory editor for ImGui (to embed in your game/tools)
// v0.10
//
// You can adjust the keyboard repeat delay/rate in ImGuiIO.
// The code assume a mono-space font for simplicity! If you don't use the default font, use ImGui::PushFont()/PopFont() to switch to a mono-space font before caling this.
//
// Usage:
//   static MemoryEditor memory_editor;                                                     // save your state somewhere
//   memory_editor.Draw("Memory Editor", mem_block, mem_block_size, (size_t)mem_block);     // run
//
// TODO: better resizing policy (ImGui doesn't have flexible window resizing constraints yet)

#include "Hydra/Kernel/Lib.h"

struct MemoryEditor {
    bool    Open;
    bool    AllowEdits;
    int     Rows;
    int     DataEditingAddr;
    bool    DataEditingTakeFocus;
    char    DataInput[32];
    char    AddrInput[32];

    MemoryEditor();

    void Draw( const char* title, unsigned char* mem_data, int mem_size, size_t base_display_addr = 0 );
};

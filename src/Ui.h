#pragma once

#include "Nes.h"

struct MainState;
struct Profiler;
struct MemoryEditor;
struct ImGuiTextBuffer;

namespace hydra {

    struct NesUI {

        enum EMemoryDebugType {
            EMemory_ChrRom = 0,
            EMemory_ChrRam,
            EMemory_PrgRom,
            EMemory_PrgRam,
            EMemory_PPU_NametableRam,
            EMemory_PPU_PalettetableRam,
            EMemory_CPU_Ram,
            EMemory_Count
        };

        NesUI( MainState* main, Nes& nes );

        void            Init();

        void            DrawControlWindow();
        void            DrawDebugger();
        void            DrawCpuStatus();
        void            DrawCartridge();
        void            DrawMemoryViewer();
        void            DrawProfiler(WindowMean<32>& fpsValues, Profiler* previousProfiler);
        void            DrawPpuStatus();
        void            DrawPpuDebugger();
        void            DrawScreen();
        void            DrawController();
        void            DrawApuStatus();

        void            Render(WindowMean<32>& fpsValues, Profiler* previousProfiler);

        void            ChangeDirectory();

        MainState*      main = nullptr;
        Nes&            nes;

        EMemoryDebugType memoryDebugType = EMemory_Count;

        // Visible windows
        bool            controlWindow = false;
        bool            cpuMemoryMap = false;
        bool            memoryViewer = false;
        bool            debugger = false;
        bool            cpuStatus = false;
        bool            cartridgeOpened = false;
        bool            profilerOpened = false;
        bool            screen = true;
        bool            ppuStatus = false;
        bool            controller = false;
        bool            ppuDebugger = false;
        bool            showASM = false;
        bool            debuggerScrollToEnd = false;
        bool            opened = false;
        bool            mouseDebug = false;
        bool            apuOpened = false;
        bool            executeOnStartup = false;

        int             debugPixel[2] = { -1, -1 };

        Nes::Controller::Buttons buttonToRemapKeyboard = Nes::Controller::Button_Count;
        Array<ProfilerSystem::CpuSample> samples;
        Array<uint16>   samplesStack;

        // Directory management
        StdArray<String> nesRoms;
        StdArray<String> directories;
        String          cwd, lastDirectory;

        MemoryEditor*   memoryEditor;
        ImGuiTextBuffer* debuggerText;
    };
}
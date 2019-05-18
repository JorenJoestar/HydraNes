#pragma once

#include <Hydra/Kernel/Lib.h>
#include <Hydra/Kernel/Input.h>
#include <Hydra/Application/Application.h>

#include "Nes.h"
#include "Ui.h"

using namespace hydra;

struct Profiler {

    StaticBuffer<1024> eventBuffer;
    ArrayView<uint8> eventBlob;
    EventStream events;

    struct commands {
        enum Type {
            start, end, push, pop
        };

        // command based profiling ?
        struct Push {
            cstring name;
            Time start;

            static uint32           Type() { return push; }
        };

        struct Pop {
            Time end;

            static uint32           Type() { return pop; }
        };
    };

    Profiler() {
        eventBlob = { eventBuffer.begin(), eventBuffer.maxSize() };
        events._blob = &eventBlob;
    }

    void Start() {

    }

    void End() {

    }

    void Push( cstring name ) {
        commands::Push e = { name, time::Now() };
        events.AddEvent<commands::Push>( &e );
    }

    void Pop() {
        commands::Pop e = { time::Now() };
        events.AddEvent<commands::Pop>( &e );
    }
}; // struct Profiler

struct Options {
    enum ZoomType {
        ZOOM_1X = 0,
        ZOOM_2X
    };

    uint16              width;
    uint16              height;
    float               masterVolume;
    std::string         lastOpenedRom;
    uint32              executeLastRomOnStartup : 1;
    uint32              zoomType : 2;
    uint32              muteAudio : 1;
    uint32              padding : 28;

    uint16              keys0[Nes::Controller::Button_Count];
}; // struct Options

struct Stream {
    uint8_t*            data;
    uint32              offset;

    void Start( void* data, uint32 size ) {
        this->data = (uint8_t*)data;
        offset = 0;
    }

    void Reset() {
        offset = 0;
    }

    void Read( void* destination, uint32 size ) {

        // ...if ( offset + size )
        memcpy( destination, data + offset, size );
        offset += size;
    }

    void Write( void* source, uint32 size ) {
        memcpy( data + offset, source, size );
        offset += size;
    }
};

struct RenderSystemNes;
struct AudioSystemNes;
struct InputSystemNes;

struct MainState : public application::State {

    StaticBuffer<1024>  windowEventBuffer;
    ArrayView<uint8>    windowEventBlob;

    Nes                 nes;
    NesUI               nesui;

    WindowMean<32>      fpsValues;

    Profiler            profiler[2];
    Profiler*           currentProfiler;
    Profiler*           previousProfiler;
    uint8               profilerIndex = 0;

    WindowSystem*       window;
    TimeSystem*         time;
    RenderSystemNes*    renderer;
    AudioSystemNes*     audio;
    InputSystemNes*     input;

    bool                cpuTestMode;
    FileHandle          cpuTestFile;
    TimeStamp           sramSaveTimer;

    Options             emulationOptions;

    HINSTANCE           hInstance;

    enum SystemOrders {
        ord_time,
        ord_window,
        ord_inputupdate,
        ord_input,
        ord_audio,
        ord_renderer
    };

    enum SimulationType {
        type_step,
        type_continuous,
        type_stopped
    };

    SimulationType simulationType;

    MainState();

    void                Init( application::InitContext& context ) override;
    bool                Update( application::UpdateContext& context  ) override;
    void                Render( application::RenderContext& context  ) override;
    void                Terminate( application::TerminateContext& context ) override;
    void                Resize( uint16 width, uint16 height ) override;

    void                LoadOptions( cstring iniFilename );
    void                SaveOptions( cstring iniFilename );

    void                LoadState();
    void                SaveState();

    // Cpu test based on nestest.nes rom and nestest.log.txt file. It does a simple string comparison to check CPU registers and expected values for each step.
    void                LoadCpuTest();
    void                ExecuteCpuTest();
};
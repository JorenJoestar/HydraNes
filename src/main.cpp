#include <hydra/Kernel/Lib.h>
#include <hydra/Kernel/Input.h>

#include <Hydra/Renderer/Renderer.h>

#include <hydra/Application/Application.h>

#include <Hydra/imgui/imgui.h>
#include <Hydra/imgui/imgui_hydra.h>

#include <GL/glew.h>
#include <GL/wglew.h>

#include "Main.h"
#include "Nes.h"
#include "Ui.h"

#include <stdlib.h>
#include <xaudio2.h>

#include "INIReader.h"

using namespace hydra;

application::App sc;

void SetExit() {
    sc._setExit = true;
}

void ResizeApplication( const window::callbacks::WindowResizeData& data ) {
    sc.Resize( data.width, data.height );
}

//////////////////////////////////////////////////////////////////////////

const GLchar *sScreenComputeCode =
"#version 430\n\
layout(binding=0) writeonly uniform image2D destTex;\
layout (local_size_x = 1, local_size_y = 1) in;\
\
uint nesRgb[] =\
{ 0x7C7C7C, 0x0000FC, 0x0000BC, 0x4428BC, 0x940084, 0xA80020, 0xA81000, 0x881400,\
  0x503000, 0x007800, 0x006800, 0x005800, 0x004058, 0x000000, 0x000000, 0x000000,\
  0xBCBCBC, 0x0078F8, 0x0058F8, 0x6844FC, 0xD800CC, 0xE40058, 0xF83800, 0xE45C10,\
  0xAC7C00, 0x00B800, 0x00A800, 0x00A844, 0x008888, 0x000000, 0x000000, 0x000000,\
  0xF8F8F8, 0x3CBCFC, 0x6888FC, 0x9878F8, 0xF878F8, 0xF85898, 0xF87858, 0xFCA044,\
  0xF8B800, 0xB8F818, 0x58D854, 0x58F898, 0x00E8D8, 0x787878, 0x000000, 0x000000,\
  0xFCFCFC, 0xA4E4FC, 0xB8B8F8, 0xD8B8F8, 0xF8B8F8, 0xF8A4C0, 0xF0D0B0, 0xFCE0A8,\
  0xF8D878, 0xD8F878, 0xB8F8B8, 0xB8F8D8, 0x00FCFC, 0xF8D8F8, 0x000000, 0x000000 };\
\
layout(std430, binding = 3) buffer ScreenData\
{\
    uint data[];\
};\
\
void main() {\
    ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);\
    uint color = data[storePos.y * 256 + storePos.x];\
    vec3 rgb = vec3((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);\
    rgb /= 255.f;\
    imageStore(destTex, storePos, vec4(rgb, 1.0));\
}";

const GLchar *sPostProcessVsCode =
"#version 330 core\n"

"out vec2 Frag_UV;\n"
"void main()\n"
"{\n"
"   float x = -1.0 + float((gl_VertexID & 1) << 2);\n"
"   float y = -1.0 + float( ( gl_VertexID & 2 ) << 1 );\n"
"   Frag_UV.x = ( x + 1.0 )*0.5;\n"
"   Frag_UV.y = ( y + 1.0 )*0.5; \n"
"	gl_Position = vec4(x, y,0,1);\n"
"}\n";

const GLchar* sPostProcessFsCode =
"#version 330 core\n"
"uniform sampler2D Texture;\n"
"in vec2 Frag_UV;\n"
"out vec4 Out_Color;\n"
"void main()\n"
"{\n"
"	Out_Color = texture( Texture, Frag_UV.st);\n"
"	Out_Color.a = 1;\n"
"}\n";


struct RenderSystemNes : public hydra::gfx::RenderSystem {

    gfx::RenderDevice device;
    WindowSystem* w;

    gfx::ComputeProgramHandle screenCP;
    gfx::TextureHandle screenTex;
    GLint textureLocation = -1;
    GLint textureLocationFrameBuffer = -1;

    gfx::FrameBufferStateHandle fbs, fbs2x; // Standard size, 2x size
    gfx::ShaderStateHandle screenGfxProgram;
    gfx::VertexLayoutHandle vl;

    GLuint screenSSBO;
    uint32* screenData;
    uint32 currentFrame;
    uint8 fbsType = 0;

    RenderSystemNes( WindowSystem* w, uint32 order ) : hydra::gfx::RenderSystem(order), w(w) {
        
    }

    void Init( gfx::RenderSystemInitContext& ctx ) override {
        device.Init( w );

        screenCP = device.CreateComputeProgram( gfx::ShaderCreation{ sScreenComputeCode, "ScreenCP" } );
        screenTex = device.CreateTexture2D( gfx::TextureCreation{nullptr, 256, 240, 1, gfx::TextureFormat::R8G8B8A8_UNORM } );

        textureLocation = glGetUniformLocation( screenCP->handle, "destTex" );
        if ( textureLocation == -1 ) {
            printf( "Could not locate uniform location for texture in CS" );
        }

        gfx::ShaderStateCreation ssc;
        ssc.vpSource = sPostProcessVsCode;
        ssc.fpSource = sPostProcessFsCode;
        screenGfxProgram = device.CreateShaderState(ssc);

        textureLocationFrameBuffer = glGetUniformLocation( screenGfxProgram->handle, "Texture" );
        if ( textureLocationFrameBuffer == -1 ) {
            printf( "Could not locate uniform location for texture in CS" );
        }

        // Create 1x framebuffer
        gfx::TextureCreation rtc{ nullptr, 256, 240, 1, gfx::TextureFormat::R8G8B8A8_UNORM };
        gfx::TextureHandle rt0 = ctx.rd.CreateTexture2D(rtc);

        gfx::FrameBufferCreation fbsc;
        fbsc.activeRts = 1;
        fbsc.rts[0] = rt0;
        fbsc.hasDepthStencil = 0;
        fbsc.clear = 1;
        fbsc.setColor(255, 25, 25, 255);
        fbs = ctx.rd.CreateFrameBufferState(fbsc);

        // Create 2x framebuffer
        gfx::TextureCreation rtc2x{ nullptr, 256 * 2, 240 * 2, 1, gfx::TextureFormat::R8G8B8A8_UNORM };
        gfx::TextureHandle rt02x = ctx.rd.CreateTexture2D( rtc2x );
        fbsc.rts[0] = rt02x;
        fbs2x = ctx.rd.CreateFrameBufferState( fbsc );

        vl = ctx.rd._renderDb.GetPostProcessVertexLayout();

        // Create screen SSBO
        glGenBuffers( 1, &screenSSBO );
        
        currentFrame = 0;
    }

    void Terminate() override {

    }

    void Update() override {

    }

    void Resize() {
    }

    void Render( gfx::RenderContext& rc ) {
        device.Render();

        // Upload colors from nes screen
        glBindBuffer( GL_SHADER_STORAGE_BUFFER, screenSSBO );
        glBufferData( GL_SHADER_STORAGE_BUFFER, Nes::Screen::kWidth * Nes::Screen::kHeight * 4, screenData, GL_DYNAMIC_DRAW );
        glBindBufferBase( GL_SHADER_STORAGE_BUFFER, 3, screenSSBO );

        // Write Screen Pixels to Texture using a compute shader.
        glUseProgram( screenCP->handle );
        glUniform1i( textureLocation, 0 );

        glBindImageTexture( 0, screenTex->handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8 );

        glDispatchCompute( screenTex->width, screenTex->height, 1 );
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        glBindBuffer( GL_SHADER_STORAGE_BUFFER, 0 ); // unbind SSBO

        // Render to frame buffer
        GLuint fb = 0;
        GLsizei viewportWidth = Nes::Screen::kWidth, viewportHeight = Nes::Screen::kHeight;
        switch ( fbsType ) {
            case 0:
                fb = fbs->handle;
                break;
            case 1:
                fb = fbs2x->handle;
                viewportWidth *= 2;
                viewportHeight *= 2;
                break;
        }
        
        glBindFramebuffer( GL_FRAMEBUFFER, fb );
        glViewport( 0, 0, viewportWidth, viewportHeight );

        glUseProgram(screenGfxProgram->handle);
        // create an empty vao
        glBindVertexArray(vl->handle);

        glBindTexture(GL_TEXTURE_2D, screenTex->handle);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureLocationFrameBuffer, 0);

        glDrawArrays( GL_TRIANGLES, 0, 3 );

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram( 0 );
    }

    void SwapBuffers() {
        device.SwapBuffers();
    }
};

//////////////////////////////////////////////////////////////////////////

class VoiceCallback : public IXAudio2VoiceCallback {
public:

    static const uint32 kBufferSize = 1024;

    HANDLE      hBufferEndEvent;
    Nes*        nes;
    IXAudio2SourceVoice* sourceVoice = nullptr;
    uint32      sampleSize;
    int16       sampleBuffer[kBufferSize];

    VoiceCallback() : hBufferEndEvent(CreateEvent(NULL, FALSE, FALSE, NULL)) {}
    ~VoiceCallback() { CloseHandle(hBufferEndEvent); }

    //Called when the voice has just finished playing a contiguous audio stream.
    void OnStreamEnd() { 
        SetEvent(hBufferEndEvent);
    }

    //Unused methods are stubs
    void OnVoiceProcessingPassEnd() { }
    void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {    }
    void OnBufferEnd(void * pBufferContext) {

        XAUDIO2_VOICE_STATE voiceState;
        sourceVoice->GetState(&voiceState);
        if (voiceState.BuffersQueued < 1) {
            if (nes->apu.blipBuffer->samples_avail()) {
                int32 readSamples = nes->apu.blipBuffer->read_samples(sampleBuffer, kBufferSize, false);

                XAUDIO2_BUFFER buffer = { 0 };
                buffer.AudioBytes = readSamples * sampleSize;
                buffer.LoopCount = 0;
                buffer.pAudioData = (BYTE *)&sampleBuffer;
                buffer.Flags = voiceState.BuffersQueued == 0 ? XAUDIO2_END_OF_STREAM : 0;

                (FAILED(sourceVoice->SubmitSourceBuffer(&buffer)));
            }
        }
    }
    void OnBufferStart(void * pBufferContext) {
    }
    void OnLoopEnd(void * pBufferContext) {    }
    void OnVoiceError(void * pBufferContext, HRESULT Error) {
        PrintFormat("Error\n");
    }
};

struct AudioSystemNes : public hydra::UpdateSystem {

    VoiceCallback   voiceCallback;
    Nes*            nes;

    IXAudio2SourceVoice *sourceVoice = nullptr;
    const int SAMPLE_BITS = 16;
    const int CHANNELS = 1;
    const int SAMPLE_RATE = 44100;
    const int VOICE_SAMPLES = SAMPLE_RATE * CHANNELS;

    // Triple buffered samples.
    static const uint32 kBufferSize = 32000;
    static const uint32 kBufferCount = 3;
    int16* samples;
    uint8 bufferIndex;

    AudioSystemNes(Nes* nes, uint32 order) : hydra::UpdateSystem(order), nes(nes) {
    }

    void Init() override {
        // TODO: UncoInitialize on error.
        HRESULT result;
        if ( FAILED( result = CoInitialize( NULL ) ) )
            return;

        IXAudio2 *xAudio;
        if ( FAILED( result = XAudio2Create( &xAudio ) ) )
            return;

        IXAudio2MasteringVoice *masteringVoice;
        if ( FAILED( result = xAudio->CreateMasteringVoice( &masteringVoice ) ) )
            return;

        samples = new int16[kBufferCount * kBufferSize];
        bufferIndex = 0;

        WAVEFORMATEX waveFormat = { 0 };
        waveFormat.wBitsPerSample = SAMPLE_BITS;
        waveFormat.nAvgBytesPerSec = (SAMPLE_BITS / 8) * CHANNELS * SAMPLE_RATE;
        waveFormat.nChannels = CHANNELS;
        waveFormat.nBlockAlign = CHANNELS * (SAMPLE_BITS / 8);
        waveFormat.nSamplesPerSec = SAMPLE_RATE;
        waveFormat.wFormatTag = WAVE_FORMAT_PCM;

        if ( FAILED( result = xAudio->CreateSourceVoice( &sourceVoice, &waveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &voiceCallback ) ) )
            return;

        voiceCallback.nes = nes;
        voiceCallback.sourceVoice = sourceVoice;
        voiceCallback.sampleSize = (SAMPLE_BITS / 8);

        if ( FAILED( sourceVoice->Start() ) )
            return;
    }

    void Terminate() override {

        delete[] samples;

        CoUninitialize();
    }

    void Update() override {
        XAUDIO2_VOICE_STATE voiceState;
        sourceVoice->GetState( &voiceState );
        //if ( voiceState.BuffersQueued < 1 )
        {
            const int32 availableSamples = nes->apu.blipBuffer->samples_avail();
            if ( availableSamples ) {
                int16* bufferAddress = &samples[kBufferSize * bufferIndex];
                const int32 readSamples = nes->apu.blipBuffer->read_samples( bufferAddress, kBufferSize, false );
                
                bufferIndex = (bufferIndex + 1) % kBufferCount;

                XAUDIO2_BUFFER buffer = { 0 };
                buffer.AudioBytes = readSamples * (SAMPLE_BITS / 8);
                buffer.LoopCount = 0;
                buffer.pAudioData = (BYTE *)bufferAddress;
                buffer.Flags = voiceState.BuffersQueued == 0 ? XAUDIO2_END_OF_STREAM : 0;

                FAILED( sourceVoice->SubmitSourceBuffer( &buffer ) );
                if ( availableSamples >= kBufferSize ) {
                    PrintFormat( "Audio buffer size is small! Need %u but has %u samples!\n", availableSamples, kBufferSize );
                }
            }
        }
    }
};


//////////////////////////////////////////////////////////////////////////
static cstring kIniFilename = "hydra_nes.ini";

MainState::MainState() : nesui( this, nes ) {

}

void MainState::Init( application::InitContext& context ) {

    LoadOptions( kIniFilename );

    sramSaveTimer = 0;

    currentProfiler = &profiler[0];

    windowEventBlob = { windowEventBuffer.begin(), windowEventBuffer.maxSize() };

    auto engine = context._engine;

    WindowInit wInit = { nullptr, hInstance, "Hydra", "Hydra", 0, 0, emulationOptions.width, emulationOptions.height, &windowEventBlob, false };
    window = engine->AddUpdate<WindowSystem>( wInit, ord_window );
    window->exitCallbacks.push_back(SetExit);
    window->resizeCallbacks.push_back(ResizeApplication);

    input = engine->AddUpdate<input::InputSystem>( ord_input );
    time = engine->AddUpdate<TimeSystem>( ord_time );
    audio = engine->AddUpdate<AudioSystemNes>( &nes, ord_audio );

    application::InputUpdateSystem::Init inInit{ *window, *input };
    engine->AddUpdate<application::InputUpdateSystem>( inInit, ord_inputupdate );

    renderer = engine->AddRender<RenderSystemNes>( window, ord_renderer );

    ImGuiInit( *window, renderer->device );

    // init and sort systems
    gfx::RenderSystemInitContext ric{ renderer->device, renderer->device._renderDb };
    engine->InitSystems( ric );

    renderer->device.EnableDebugOutput( false );

#if defined(NES_SHOW_ASM)
    nes.cpu.CpuDisassembleInit();
    nes.cpu.showAsm = false;
    nes.cpu.testAsm = false;
#endif
    nes.Init( &emulationOptions );

    nes.screenTexture = renderer->fbs->rts[0]->handle;    

    renderer->screenData = nes.screen.frameBuffer;

    if ( emulationOptions.lastOpenedRom.size() > 4 ) {
        nes.LoadRom( emulationOptions.lastOpenedRom.c_str() );
    } 
    else {
        nes.LoadRom( "nestest.nes" );
    }

    simulationType = emulationOptions.executeLastRomOnStartup ? type_continuous : type_step;
    nesui.executeOnStartup = emulationOptions.executeLastRomOnStartup;

    cpuTestMode = false;
    if ( cpuTestMode ) {
        LoadCpuTest();
    }
}

bool MainState::Update( application::UpdateContext& context ) {

    // TODO: Switch profilers. Improve code.
    ++profilerIndex;
    profilerIndex %= 2;
    previousProfiler = currentProfiler;
    currentProfiler = &profiler[profilerIndex];
    currentProfiler->events.Reset();

    currentProfiler->Push( "Main Update" );

    if ( nes.cart.IsCartridgeInserted() ) {
        sramSaveTimer += time->_deltaMs;
        if ( sramSaveTimer > 2000 && nes.memoryController.mapper->prgRamWrite ) {
            sramSaveTimer = 0;

            nes.SaveSRAM();
        }
    }

    context._engine->Update();

    window->ExecuteCallbacks();

    ImGuiNewFrame(*window, *input, renderer->device);

    fpsValues.AddValue( time->_deltaMs / 1000.0f );

    if ( cpuTestMode ) {

        ExecuteCpuTest();
    }
    else {
        if ( simulationType == type_continuous && nes.cart.IsCartridgeInserted() ) {

            // Update controllers
            static hydra::input::Keys kKeys[] = { hydra::input::KEY_X, hydra::input::KEY_Z, hydra::input::KEY_RSHIFT, hydra::input::KEY_RETURN, hydra::input::KEY_UP, hydra::input::KEY_DOWN, hydra::input::KEY_LEFT, hydra::input::KEY_RIGHT };
            for ( size_t i = 0; i < 8; ++i ) {

                if ( input->_input.keys.IsKeyDown( kKeys[i] ) ) {
                    nes.controllers.SetButton( 0, (Nes::Controller::Buttons) (i) );
                }
            }

            uint64 currentCpuCycles = nes.cpu.cycles;
            uint32 currentFrame = nes.ppu.frames;
            

            //PrintFormat( "Frame %u\n", currentFrame );
            // Execute 1 frame = execute until ppu changes frame.
            while ( currentFrame == nes.ppu.frames )
                nes.cpu.Step();

            uint64 elapsedCpuCycles = nes.cpu.cycles - currentCpuCycles;
            // Emulate APU
            nes.apu.EndFrame(elapsedCpuCycles);

            nes.cpu.frameCycles = 0;
        }

        // Reset per frame data if frame is changed
        if ( renderer->currentFrame != nes.ppu.frames ) {
            nes.controllers.NewFrame();

            renderer->currentFrame = nes.ppu.frames;
        }
    }    

    currentProfiler->Pop();

    // Always update screen texture
    switch ( nes.screen.zoomFactor ) {
        case Nes::Screen::Zoom_1x:
            nes.screenTexture = renderer->fbs->rts[0]->handle;
            break;

        case Nes::Screen::Zoom_2x:
            nes.screenTexture = renderer->fbs2x->rts[0]->handle;
            break;
    }
    renderer->fbsType = (uint8)nes.screen.zoomFactor;

    return true;
}

void MainState::Render( application::RenderContext& context ) {
    currentProfiler->Push( "Main Render" );

    nesui.Render(fpsValues, previousProfiler);
    
    hydra::gfx::RenderContext rc;
    currentProfiler->Push( "ImGui" );
    renderer->Render( rc );

    ImGui::Render();
    currentProfiler->Pop();

    currentProfiler->Push( "Swap buffer" );
    renderer->SwapBuffers();
    currentProfiler->Pop();

    currentProfiler->Pop();
}

void MainState::Terminate( application::TerminateContext& context ) {

    ImGuiTerminate( renderer->device );

    context._engine->TerminateSystems();

    SaveOptions( kIniFilename );
}

void MainState::Resize( uint16 width, uint16 height ) {

}

void MainState::LoadOptions( cstring ini_filename ) {
    FileHandle ini_file;
    OpenFile( ini_filename, "rb", &ini_file );
    if ( !ini_file ) {

        // If not existing, create it and set defaults.
        emulationOptions.width = 1800;
        emulationOptions.height = 1000;
        emulationOptions.lastOpenedRom = "";
        emulationOptions.executeLastRomOnStartup = 0;
        emulationOptions.zoomType = Options::ZOOM_1X;
        emulationOptions.muteAudio = 0;
        emulationOptions.masterVolume = 0.666f;
        SaveOptions( ini_filename );

        return;
    }

    INIReader reader( ini_file );
    emulationOptions.width = (uint16)reader.GetInteger( "Window", "window_width", 1024 );
    emulationOptions.height = (uint16)reader.GetInteger( "Window", "window_height", 768 );
    emulationOptions.lastOpenedRom = reader.Get( "Emulation", "last_rom_file", "" );
    emulationOptions.executeLastRomOnStartup = reader.GetInteger( "Emulation", "emulation_rom_startup", 0 );
    emulationOptions.zoomType = reader.GetInteger( "Graphics", "graphics_zoom", 0 );
    emulationOptions.muteAudio = reader.GetInteger( "Audio", "audio_mute", 0 );
    emulationOptions.masterVolume = (float)reader.GetReal( "Audio", "audio_master_volume", 0.666f );
    fclose( ini_file );
}

void MainState::SaveOptions( cstring ini_filename ) {
    FileHandle ini_file;
    OpenFile( ini_filename, "wb", &ini_file );
    if ( !ini_file ) {
        PrintFormat( "Error creating configuration file %s. Cannot save settings.\n", ini_filename );
        return;
    }

    emulationOptions.zoomType = (uint8)nes.screen.zoomFactor;
    emulationOptions.muteAudio = nes.apu.mute ? 1 : 0;
    emulationOptions.masterVolume = nes.apu.volume;
    emulationOptions.lastOpenedRom = nes.cart.filename;
    emulationOptions.executeLastRomOnStartup = nesui.executeOnStartup;

    fprintf_s( ini_file, "[Window]\n" );
    fprintf_s( ini_file, "window_width=%u\n", emulationOptions.width );
    fprintf_s( ini_file, "window_height=%u\n", emulationOptions.height );
    fprintf_s( ini_file, "[Graphics]\n" );
    fprintf_s( ini_file, "graphics_zoom=%u\n", emulationOptions.zoomType );
    fprintf_s( ini_file, "[Emulation]\n" );
    fprintf_s( ini_file, "emulation_rom_startup=%u\n", emulationOptions.executeLastRomOnStartup );
    fprintf_s( ini_file, "last_rom_file=%s\n", emulationOptions.lastOpenedRom.c_str() );
    fprintf_s( ini_file, "[Audio]\n" );
    fprintf_s( ini_file, "audio_mute=%u\n", emulationOptions.muteAudio );
    fprintf_s( ini_file, "audio_master_volume=%f\n", emulationOptions.masterVolume );
    fclose( ini_file );
}

void MainState::LoadCpuTest() {
    cstring filename = "nestest.log.txt";
    OpenFile( filename, "r", &cpuTestFile );

    if ( !cpuTestFile ) {

        PrintFormat( "Error loading %s\n", filename );
        fclose( cpuTestFile );
        cpuTestMode = false;

        return;
    }

    nes.cpu.testAsm = true;
    nes.cpu.showAsm = true;
    nes.cpu.PC = 0xC000;
}

void MainState::ExecuteCpuTest() {

    char line[512];
    char buf[8];
    // Data to test against, coming from the log file.
    uint8       A, X, Y, S, P;
    uint16      PC;
    uint64      cycles;
    uint16      scanline, pixel;

    while ( fgets( line, sizeof line, cpuTestFile ) != NULL ) {

        char* strPC = line;
        char* strPCEnd = strPC + 4;
        buf[0] = line[0]; buf[1] = line[1]; buf[2] = line[2]; buf[3] = line[3]; buf[4] = 0;
        PC = (uint16)strtoul( strPC, &strPCEnd, 16 );

        if ( PC != nes.cpu.PC ) {
            Assert( false && "Error!" );
        }

        char* strA = strstr( line, "A:" );
        buf[0] = strA[2]; buf[1] = strA[3]; buf[2] = 0;
        A = (uint8)strtoul( buf, nullptr, 16 );


        if ( A != nes.cpu.A ) {
            Assert( false && "Error!" );
        }

        char* strX = strstr( line, "X:" );
        buf[0] = strX[2]; buf[1] = strX[3]; buf[2] = 0;
        X = (uint8)strtoul( buf, nullptr, 16 );

        if ( X != nes.cpu.X ) {
            Assert( false && "Error!" );
        }


        char* strY = strstr( line, "Y:" );
        buf[0] = strY[2]; buf[1] = strY[3]; buf[2] = 0;
        Y = (uint8)strtoul( buf, nullptr, 16 );


        if ( Y != nes.cpu.Y ) {
            Assert( false && "Error!" );
        }


        char* strP = strstr( line, "P:" );
        buf[0] = strP[2]; buf[1] = strP[3]; buf[2] = 0;
        P = (uint8)strtoul( buf, nullptr, 16 );


        if ( P != nes.cpu.P.data ) {
            Assert( false && "Error!" );
        }


        char* strSP = strstr( line, "SP:" );
        buf[0] = strSP[3]; buf[1] = strSP[4]; buf[2] = 0;
        S = (uint8)strtoul( buf, nullptr, 16 );

        if ( S != nes.cpu.S ) {
            Assert( false && "Error!" );
        }

        char* strPPU = strstr( line, "PPU:" );
        char* strPPUComma = strstr( strPPU, "," );

        pixel = (uint16)strtoul( strPPU + 4, &strPPUComma, 10 );

        char* strCYC = strstr( line, "CYC:" );
        cycles = (uint64)strtoul( strCYC + 4, nullptr, 10 );

        scanline = (uint16)strtoul( strPPUComma + 1, &strCYC, 10 );

        //if ( scanline != nes.ppu.scanline ) {
        //    Assert( false && "Error!" );
        //}

        if ( pixel != nes.ppu.pixelCycle ) {
            Assert( false && "Error!" );
        }

        if ( cycles != nes.cpu.cycles ) {
            Assert( false && "Error!" );
        }

        nes.cpu.Step();

        // Read nes state from file and compare to the actual state.
        // C000  4C F5 C5  JMP $C5F5                       A:00 X:00 Y:00 P:24 SP:FD PPU:  0,  0 CYC:7
    }
}

//////////////////////////////////////////////////////////////////////////
// main
HyWinMain() {
    MainState m;
    m.hInstance = hInstance;

    application::StateId id = sc.AddState( &m );
    sc.ChangeState( id );

    sc.MainLoop();

    return 0;
}
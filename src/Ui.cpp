#include "Ui.h"

#include "hydra/imgui/imgui.h"
#include "Main.h"
#include "MemoryEditor.h"

namespace hydra {

inline void SetupImGuiStyle( bool bStyleDark_ ) {

    //ImGui::GetIO().Fonts->AddFontFromFileTTF( "consola.ttf", 13 );
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 Full = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
    style.FrameRounding = 3.0f;
    style.Colors[ImGuiCol_Text] = ImVec4( 0.00f, 0.00f, 0.00f, 1.00f );
    style.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.60f, 0.60f, 0.60f, 1.00f );
    style.Colors[ImGuiCol_WindowBg] = ImVec4( 0.94f, 0.94f, 0.94f, 1.00f );
    style.Colors[ImGuiCol_ChildWindowBg] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
    style.Colors[ImGuiCol_Border] = ImVec4( 0.00f, 0.00f, 0.00f, 0.39f );
    style.Colors[ImGuiCol_BorderShadow] = ImVec4( 1.00f, 1.00f, 1.00f, 0.10f );
    style.Colors[ImGuiCol_FrameBg] = ImVec4( 1.00f, 1.00f, 1.00f, 1.00f );
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.40f );
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
    style.Colors[ImGuiCol_TitleBg] = ImVec4( 0.96f, 0.96f, 0.96f, 1.00f );
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4( 1.00f, 1.00f, 1.00f, 0.51f );
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4( 0.82f, 0.82f, 0.82f, 1.00f );
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4( 0.86f, 0.86f, 0.86f, 1.00f );
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4( 0.98f, 0.98f, 0.98f, 0.53f );
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4( 0.69f, 0.69f, 0.69f, 0.80f );
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4( 0.49f, 0.49f, 0.49f, 0.80f );
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4( 0.49f, 0.49f, 0.49f, 1.00f );
    style.Colors[ImGuiCol_ComboBg] = ImVec4( 0.86f, 0.86f, 0.86f, 0.99f );
    style.Colors[ImGuiCol_CheckMark] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_SliderGrab] = ImVec4( 0.26f, 0.59f, 0.98f, 0.78f );
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_Button] = ImVec4( 0.26f, 0.59f, 0.98f, 0.40f );
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_ButtonActive] = ImVec4( 0.06f, 0.53f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_Header] = ImVec4( 0.26f, 0.59f, 0.98f, 0.31f );
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.80f );
    style.Colors[ImGuiCol_HeaderActive] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_Column] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
    style.Colors[ImGuiCol_ColumnHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.78f );
    style.Colors[ImGuiCol_ColumnActive] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4( 1.00f, 1.00f, 1.00f, 0.50f );
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4( 0.26f, 0.59f, 0.98f, 0.67f );
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4( 0.26f, 0.59f, 0.98f, 0.95f );
    style.Colors[ImGuiCol_CloseButton] = ImVec4( 0.59f, 0.59f, 0.59f, 0.50f );
    style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4( 0.98f, 0.39f, 0.36f, 1.00f );
    style.Colors[ImGuiCol_CloseButtonActive] = ImVec4( 0.98f, 0.39f, 0.36f, 1.00f );
    style.Colors[ImGuiCol_PlotLines] = ImVec4( 0.39f, 0.39f, 0.39f, 1.00f );
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4( 0.26f, 0.59f, 0.98f, 0.35f );
    //style.Colors[ImGuiCol_TooltipBg] = ImVec4( 1.00f, 1.00f, 1.00f, 0.94f );
    style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4( 0.20f, 0.20f, 0.20f, 0.35f );

    if ( bStyleDark_ ) {
        for ( int i = 0; i <= ImGuiCol_COUNT; i++ ) {
            ImVec4& col = style.Colors[i];
            float H, S, V;
            ImGui::ColorConvertRGBtoHSV( col.x, col.y, col.z, H, S, V );

            if ( S < 0.1f ) {
                V = 1.0f - V;
            }
            ImGui::ColorConvertHSVtoRGB( H, S, V, col.x, col.y, col.z );
        }
    }
}

//////////////////////////////////////////////////////////////////////////
void NesUI_KeyCallback( void* userData, const window::callbacks::KeyModifiedData& data ) {
    NesUI* nesui = (NesUI*)userData;

    // Ignore callback if we are not remapping the controller.
    if ( nesui->buttonToRemapKeyboard == Nes::Controller::Button_Count ) {
        return;
    }

    // Get when key is up
    if ( !data.keyDown ) {
        if ( data.virtualKey != hydra::input::KEY_ESCAPE ) {
            nesui->main->emulationOptions.keys0[nesui->buttonToRemapKeyboard] = data.virtualKey;
        }
        nesui->buttonToRemapKeyboard = Nes::Controller::Button_Count;
    }
}

NesUI::NesUI( MainState* main, Nes& nes ) : nes( nes ), main(main) {

    debuggerText = new ImGuiTextBuffer();
    memoryEditor = new MemoryEditor();

    SetupImGuiStyle( true );

    controlWindow = cartridgeOpened = true;
#if defined(HY_DEBUG)
    ppuDebugger = ppuStatus = mouseDebug = true;
#endif

    // Load rom names
    cwd.clear();
    lastDirectory.clear();
    nesRoms.clear();
    directories.clear();

    cwd.append( "." );

    FindFilesInPath( ".nes", cwd.c_str(), nesRoms, directories );
}

void NesUI::Init() {
    main->window->AddKeyCallback( NesUI_KeyCallback, this );
}

void NesUI::DrawControlWindow() {
    if ( controlWindow ) {
        const char* statusLabels[] = { "Step", "Continuous", "Stopped" };
        ImGui::Text( "Status: %s", statusLabels[main->simulationType] );
        
        ImGui::Begin( "Simulation Control", &controlWindow, ImGuiWindowFlags_MenuBar );

        if ( main->simulationType == MainState::type_continuous ) {
            if ( ImGui::Button( "Stop" ) ) {
                main->simulationType = MainState::type_stopped;
            }
        } else {
            if ( ImGui::Button( "Continue" ) ) {
                main->simulationType = MainState::type_continuous;
            }
        }
        ImGui::SameLine();
        if ( ImGui::Button( "Single Step" ) ) {

            nes.cpu.Step();

            if ( showASM ) {
                debuggerText->append( "%s\n", nes.cpu.asmBuffer.begin() );
                debuggerScrollToEnd = true;
            }
        }

        if ( ImGui::Button( "Scanline Step" ) ) {

            uint16 currentScanline = nes.ppu.scanline;
            while ( currentScanline == nes.ppu.scanline ) {
                nes.cpu.Step();
            }
        }

        if ( ImGui::Button( "Frame Step" ) ) {

            uint32 currentFrame = nes.ppu.frames;

            while (currentFrame == nes.ppu.frames ) {
                nes.cpu.Step();
            }
        }

        ImGui::Checkbox( "Show ASM", &showASM );

        nes.cpu.showAsm = main->simulationType == MainState::type_continuous ? false : showASM;

        if ( ImGui::Button( "Reset" ) ) {
            nes.Reset();
        }

        ImGui::Separator();

        // Combo using proper array. You can also pass a callback to retrieve array value, no need to create/copy an array just for that.
        static const char* memoryTypes[] = { "ChrRom", "ChrRam", "PrgRom", "PrgRam", "Nametable", "PaletteTable", "CPU Ram" };
        ImGui::Combo( "Memory Debug", (int*)&memoryDebugType, memoryTypes, ArrayLength( memoryTypes ) );

        ImGui::End();
    }
}

void NesUI::DrawDebugger() {
    if ( debugger ) {
        ImGui::Begin( "Debugger", &debugger );

        if ( ImGui::Button( "Clear" ) ) {
            debuggerText->clear();

            nes.cpu.asmBuffer[0] = 0;
        }

        ImGui::BeginChild( "scrolling" );
        ImGui::TextUnformatted( debuggerText->begin() );
        if ( debuggerScrollToEnd ) {
            ImGui::SetScrollHere( 1.0f );
            debuggerScrollToEnd = false;
        }

        ImGui::EndChild();
        ImGui::End();
    }
}

void NesUI::DrawCpuStatus() {
    if ( !cpuStatus )
        return;

    ImGui::SetNextWindowSize( ImVec2( 430, 450 ), ImGuiSetCond_FirstUseEver );
   
    ImGui::Begin( "Cpu", &cpuStatus );

    ImGui::Columns( 2 );
    ImGui::Text( "OpCode" );
    ImGui::NextColumn();
    ImGui::Text( "%X", nes.cpu.opCode );
    ImGui::Separator();

    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 2, 2 ) );
    ImGui::Columns( 7 );

    static cstring kLabels[] = { "A", "X", "Y", "S", "P" };
    static uint8* sValues[] = { &nes.cpu.A, &nes.cpu.X, &nes.cpu.Y, &nes.cpu.S, &nes.cpu.P.data };
    const uint32 registerCount = ArrayLength( sValues );

    for ( uint8 i = 0; i < registerCount; ++i ) {
        ImGui::AlignFirstTextHeightToWidgets();
        ImGui::Text( "%s: %X", kLabels[i], *( sValues[i] ) );
        ImGui::NextColumn();
    }
    
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::Text( "PC: %X", nes.cpu.PC );
    ImGui::NextColumn();

    // Why the log is 3 times ?
    ImGui::AlignFirstTextHeightToWidgets();
    ImGui::Text( "Cycles: %d", nes.ppu.pixelCycle );
    ImGui::NextColumn();

    ImGui::Columns( 1 );
    ImGui::Separator();

    ImGui::Columns( 7 );

    static cstring kLabelsFlag[] = { "Carry", "Zero", "Interrupt", "Decimal", "Break", "", "Overflow", "Sign" };
    uint8* flags = (uint8*)( &nes.cpu.P.flags );

    for ( uint8 i = 0; i < 8; ++i ) {
        // Skip empty bit
        if ( i == 5 )
            continue;
        ImGui::AlignFirstTextHeightToWidgets();  // Text and Tree nodes are less high than regular widgets, here we add vertical spacing to make the tree lines equal high.
        ImGui::Text( kLabelsFlag[i] );
        ImGui::NextColumn();
        
    }
    for ( uint8 i = 0; i < 8; ++i ) {
        // Skip empty bit
        if ( i == 5 )
            continue;
        ImGui::AlignFirstTextHeightToWidgets();
        uint8 value = ( ( ( *flags ) >> i ) & 1 );
        ImGui::Text( "%X", value );
        ImGui::NextColumn();
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void NesUI::DrawCartridge() {
    if ( !cartridgeOpened )
        return;

    ImGui::Begin( "Cartridge", &cartridgeOpened );
	ImGui::Text("CWD: %s", cwd.c_str());
	ImGui::Separator();

    bool changedDirectory = false;

    for ( String s : directories ) {
        bool selected = false;
        if ( ImGui::Selectable( s.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick ) ) {
            if ( ImGui::IsMouseDoubleClicked( 0 ) ) {
                lastDirectory.clear();
                lastDirectory.append( s );

                changedDirectory = true;
            }
        }
    }

    for ( String s : nesRoms ) {
        bool selected = false;
        if ( ImGui::Selectable( s.c_str(), selected, ImGuiSelectableFlags_AllowDoubleClick ) ) {
            if ( ImGui::IsMouseDoubleClicked( 0 ) ) {
				String fullpath(cwd);
				fullpath.append("\\");
				fullpath.append(s);
                nes.LoadRom( fullpath.c_str() );
            }
        }
    }

    ImGui::Separator();

    ImGui::Text( "File: %s", nes.cart.filename.c_str() );
    ImGui::Text( "Prg Rom Pages (16Kb): %d", nes.cart.romHeader.prgRomPages );
    ImGui::Text( "Chr Rom Pages (8Kb): %d", nes.cart.romHeader.chrRomPages );
    ImGui::Text( "Prg Ram Pages (8Kb): %d", nes.cart.romHeader.prgRamPages );
    ImGui::Text( "Mapper Index %u", nes.cart.mapperIndex );

    ImGui::Separator();
    ImGui::Checkbox( "Execute on startup last executed rom.", &executeOnStartup );

    ImGui::End();

    if ( changedDirectory ) {

        if ( lastDirectory.compare( ".." ) == 0 ) {
            size_t last = cwd.rfind('\\', cwd.length());
            String tmp = cwd.substr( 0, last);
            
            cwd.clear();
            cwd.append( tmp );
        }
        else {
            cwd.append( "\\" );
            cwd.append( lastDirectory );
        }
        
        ChangeDirectory();
    }
}

void NesUI::DrawMemoryViewer() {

    switch ( memoryDebugType ) {

        case EMemory_ChrRom:
            memoryEditor->Draw( "Chr Rom", nes.cart.chrRom.begin(), nes.cart.chrRom.size(), 0 );
            break;

        case EMemory_ChrRam:
            memoryEditor->Draw( "Chr Ram", &nes.cart.chrRam[0], Nes::kChrBankSize, 0 );
            break;

        case EMemory_PrgRom:
            memoryEditor->Draw( "Prg Rom", nes.cart.prgRom.begin(), nes.cart.prgRom.size(), 0 );
            break;

        case EMemory_PrgRam:
            memoryEditor->Draw( "Prg Ram", nes.cart.prgRam.begin(), nes.cart.prgRam.size(), 0 );
            break;

        case EMemory_PPU_NametableRam:
            memoryEditor->Draw( "Nametable", &nes.ppu.nametableRam[0], Nes::Ppu::kNameTableRamSize, 0 );
            break;

        case EMemory_PPU_PalettetableRam:
            memoryEditor->Draw( "Palette", &nes.ppu.paletteRam[0], Nes::Ppu::kPaletteRamSize , 0 );
            break;

        case EMemory_CPU_Ram:
            memoryEditor->Draw( "CPU Ram", &nes.cpu.ram[0], Nes::Cpu::kRamSize, 0 );
            break;
    }
    
}

void NesUI::DrawProfiler(WindowMean<32>& fpsValues, Profiler* previousProfiler) {

    if (!profilerOpened)
        return;

    ImGui::SetNextWindowSize( ImVec2( 430, 450 ), ImGuiSetCond_FirstUseEver );
    ImGui::Begin( "Frame Timing", &profilerOpened );

    ImGui::LabelText( "", "Time %3.4f", main->fpsValues.GetMean() );

    //ImGui::InputTextMultiline();
    
    // read profiler sample commands        
    ProfilerSystem::CpuSample* currentSample = nullptr;
    uint8 depth = 0;
    if ( previousProfiler->events.HasEvents() ) {
        while ( !previousProfiler->events.EndOfStream() ) {
            uint32 type = previousProfiler->events.GetEventType();
            switch ( type ) {
                case Profiler::commands::push:
                {
                    const Profiler::commands::Push& e = previousProfiler->events.GetEvent<Profiler::commands::Push>();
                    ProfilerSystem::CpuSample s = { e.start, e.start, e.name };
                    samples.push_back( s );
                    samplesStack.push_back( samples.size() - 1 );
                    currentSample = &samples.back();

                    ++depth;

                    break;
                }

                case Profiler::commands::pop:
                {
                    const Profiler::commands::Pop& e = previousProfiler->events.GetEvent<Profiler::commands::Pop>();
                    currentSample->end = e.end;
                    currentSample->depth = depth;
                    samplesStack.pop_back();

                    currentSample = samplesStack.size() ? &samples[samplesStack.back()] : nullptr;
                    --depth;

                    break;
                }
            }
        }
        samplesStack.clear();
    }

    ImGui::Separator();

    ImGui::SetNextTreeNodeOpened( true );
    if ( ImGui::TreeNode( "Timing" ) ) {
        for ( const ProfilerSystem::CpuSample& s : samples ) {
            TimeStamp t = time::IntervalMilliseconds( s.end, s.start );

            for ( uint8 d = 0; d < s.depth; ++d ) {

                ImGui::Text( "---" );
                ImGui::SameLine();
            }

            ImGui::Text( "%s, %f\n", s.name, t / 1000.0f );
        }
        ImGui::TreePop();
    }

    samples.clear();

    ImGui::Separator();
    ImGui::PlotLines( "", fpsValues._values.data, fpsValues.GetWindowSize(), fpsValues._lastValueIndex, "Fps", 0.0f, 0.1f, ImVec2( ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - ImGui::GetCursorPosY() ) );
    
    ImGui::End();

}

void NesUI::DrawPpuStatus() {
    if ( !ppuStatus )
        return;

    ImGui::Begin( "PPU", &ppuStatus );

    ImGui::Text( "Scanline: %u, Pixel %u", nes.ppu.scanline, nes.ppu.pixelCycle );
    ImGui::Text( "V: %x", nes.ppu.v );
    ImGui::Text( "T: %x", nes.ppu.t );
    ImGui::Text( "Frame: %u", nes.ppu.frames );
    ImGui::Text( "VBlank: %u", nes.ppu.verticalBlank );

    ImGui::Separator();

    if ( ImGui::Button( "Debug Frame" ) ) {
        nes.ppu.debugFrame = true;
        ppuDebugger = true;
    }

    ImGui::End();
}

static uint32 RGBtoBGR( uint32 rgb ) {
    uint32 color = 0xff000000 | ((rgb >> 16) & 0xff) | ((rgb & 0xff) << 16) | (rgb & 0x00ff00);
    return color;
}

void NesUI::DrawPpuDebugger() {
    if ( !ppuDebugger )
        return;

    ImGui::Begin( "PPU Debugger", &ppuDebugger );

    ImGui::BeginGroup();
    ImGui::Checkbox( "Mouse Debug", &mouseDebug );

    ImGui::InputInt2( "Scanline, Pixel", debugPixel );

    if ( ImGui::Button( "Prev Pixel" ) ) {
        debugPixel[1] = debugPixel[1] > 0 ? debugPixel[1] - 1 : debugPixel[1];
    }
    ImGui::SameLine();

    if ( ImGui::Button( "Next Pixel" ) ) {
        debugPixel[1] = debugPixel[1] < Nes::Ppu::kMaxPixels ? debugPixel[1] + 1 : debugPixel[1];
    }
    ImGui::SameLine();

    if ( ImGui::Button( "Pixel - 8" ) ) {
        debugPixel[1] = debugPixel[1] > 7 ? debugPixel[1] - 8 : debugPixel[1];
    }
    ImGui::SameLine();

    if ( ImGui::Button( "Pixel + 8" ) ) {
        debugPixel[1] = debugPixel[1] < Nes::Ppu::kMaxPixels - 8 ? debugPixel[1] + 8 : debugPixel[1];
    }
    ImGui::SameLine();

    if ( ImGui::Button( "Prev Scanline" ) ) {
        debugPixel[0] = debugPixel[0] > 0 ? debugPixel[0] - 1 : debugPixel[0];
    }
    ImGui::SameLine();

    if ( ImGui::Button( "Next Scanline" ) ) {
        debugPixel[0] = debugPixel[0] < Nes::Ppu::kMaxScanlines ? debugPixel[0] + 1 : debugPixel[0];
    }
    ImGui::SameLine();
    ImGui::EndGroup();
    ImGui::Separator();
    

    static cstring sEventNames[] = {
        "Draw",
        "IncrementX",
        "IncrementY",
        "CopyX",
        "CopyY",
        "ReloadShift",
        "Shift"
    };

#if defined(NES_TEST_PPU_CYCLE)
    if ( debugPixel[0] >= 0 && debugPixel[0] < Nes::Ppu::kMaxPixels && debugPixel[1] >= 0 && debugPixel[1] < Nes::Ppu::kMaxScanlines && nes.ppu.debugger.pixelData ) {
        const Nes::Ppu::PpuDebugger::PixelData& pixelData = nes.ppu.debugger.pixelData[debugPixel[1] + debugPixel[0] * Nes::Ppu::kMaxPixels];

        ImGui::BeginGroup();
        ImGui::Text( "PPU Address: %x", pixelData.nameTableAddress );
        ImGui::Text( "NT: %x", pixelData.nameTableByte );
        ImGui::Text( "AT: %x", pixelData.attributeTableByte );
        ImGui::Text( "AT Address: %x", pixelData.attributeTableAddress );
        ImGui::Text( "TH: %x", pixelData.tileHighByte );
        ImGui::Text( "TL: %x", pixelData.tileLowByte );
        ImGui::Text( "Tile Address: %x", pixelData.tileAddress );
        ImGui::Text( "Palette Address: %x", pixelData.paletteAddress );
        ImGui::Text( "Palette index: %x", pixelData.paletteIndex );
        
        uint8 events = pixelData.events;
        for ( size_t i = 0; i < 8; ++i ) {
            if ( events & 1 ) {
                ImGui::Text( "EV: %s", sEventNames[i] );
            }

            events >>= 1;
        }
        ImGui::EndGroup();

        ImGui::SameLine();

        ImGui::BeginGroup();
        
        ImVec2 pos = ImGui::GetCursorScreenPos();
        uint32 nesColor = nes.ppu.GetPaletteColor( pixelData.paletteIndex );
        uint32 color = RGBtoBGR( nesColor );
        ImGui::GetWindowDrawList()->AddRectFilled( ImVec2( pos.x, pos.y ), ImVec2( pos.x + 64, pos.y + 64 ), color );
        ImGui::EndGroup();

        ImGui::SameLine( 0.f, 96.f );

        ImGui::BeginGroup();
        pos = ImGui::GetCursorScreenPos();
        const float rectsize = 16;
        ImGui::Text( "Palette RAM content:" );
        pos = ImGui::GetCursorScreenPos();

        for ( uint32 i = 0; i < 32; ++i ) {
            uint8 paletteIndex = nes.memoryController.PpuRead( 0x3F00 + i );
            nesColor = nes.ppu.GetPaletteColor( paletteIndex );
            uint32 color = RGBtoBGR( nesColor );

            float x = (i % 4) * rectsize + pos.x;
            float y = (i / 4) * rectsize + pos.y;
            ImGui::GetWindowDrawList()->AddRectFilled( ImVec2( x, y ), ImVec2( x + 16, y + 16 ), color );
        }

        ImGui::EndGroup();
    }
#endif // NES_TEST_PPU_CYCLE
    ImGui::End();
}

void NesUI::DrawScreen() {
    ImGui::Begin( "Screen", &screen );
    
    static const char* zoomTypes[] = { "1X", "2X" };
    ImGui::Combo( "Zoom Type", (int*)&nes.screen.zoomFactor, zoomTypes, ArrayLength( zoomTypes ) );

    // Cursor position relative to image
    ImVec2 cursorPos = ImGui::GetMousePos();
    cursorPos.x -= ImGui::GetCursorScreenPos().x;
    cursorPos.y -= ImGui::GetCursorScreenPos().y;

    float zoomFactor = ((uint8)nes.screen.zoomFactor) * 2.0f + 1.0f;
    const float screenWidth = Nes::Screen::kWidth * zoomFactor;
    const float screenHeight = Nes::Screen::kHeight * zoomFactor;
    ImGui::Image( (void*)(intptr_t)nes.screenTexture, ImVec2( screenWidth, screenHeight) );

    if ( mouseDebug && ImGui::IsMouseClicked( 2, true ) && cursorPos.x >= 0 && (cursorPos.x < Nes::Ppu::kMaxPixels * zoomFactor) && cursorPos.y >= 0 && (cursorPos.y < Nes::Ppu::kMaxScanlines * zoomFactor) ) {
        // Inverted. debugpixel = scanline, pixel
        debugPixel[0] = (int)(cursorPos.y / zoomFactor);
        debugPixel[1] = (int)(cursorPos.x / zoomFactor);
    }
    
    ImGui::End();
}

void NesUI::DrawController() {
    if ( !controller )
        return;

    ImGui::Begin( "Controller", &controller );

    ImGui::Text("Click the button under the labels to remap the corresponding key.");
    ImGui::Text( "Any key pressed after will be the new one." );

    ImGui::Separator();

    if ( buttonToRemapKeyboard != Nes::Controller::Button_Count ) {
        ImGui::Text( "Press any key to change the binding of button %s", nes.controllers.kDefaultKeys0Names[buttonToRemapKeyboard] );
        ImGui::Separator();
    }

    ImGui::Columns( Nes::Controller::Button_Count );
    // Write the controller button names
    for ( size_t i = 0; i < Nes::Controller::Button_Count; ++i ) {
        ImGui::Text( "%s", nes.controllers.kDefaultKeys0Names[i] );
        ImGui::NextColumn();
    }
    // Write the actual key bindings
    for ( size_t i = 0; i < Nes::Controller::Button_Count; ++i ) {
        cstring keyBindingName = hydra::input::sNames[main->emulationOptions.keys0[i]];
        if ( ImGui::Button( keyBindingName ) ) {
            buttonToRemapKeyboard = (Nes::Controller::Buttons)i;
        }

        ImGui::NextColumn();
    }

    ImGui::End();
}

void NesUI::DrawApuStatus() {
    if ( !apuOpened )
        return;

    ImGui::Begin( "APU", &apuOpened);
    if ( ImGui::Checkbox( "Mute", &nes.apu.mute ) ) {
        nes.apu.Mute( nes.apu.mute );
    }

    if ( ImGui::SliderFloat( "Volume", &nes.apu.volume, 0.0f, 1.0f ) ) {
        nes.apu.SetVolume( nes.apu.volume );
    }

    ImGui::End();
}

void NesUI::Render( WindowMean<32>& fpsValues, Profiler* previousProfiler ) {

    ImGui::BeginMainMenuBar();
    if ( ImGui::BeginMenu( "Windows" ) ) {
        ImGui::MenuItem( "CpuMemoryMap", NULL, &cpuMemoryMap );
        ImGui::MenuItem( "MemoryViewer", NULL, &memoryViewer );
        ImGui::MenuItem( "Debugger", NULL, &debugger );
        ImGui::MenuItem( "CpuStatus", NULL, &cpuStatus );
        ImGui::MenuItem( "Cartridge", NULL, &cartridgeOpened );
        ImGui::MenuItem( "Controller", NULL, &controller );
        ImGui::MenuItem( "Execution", NULL, &controlWindow );
        ImGui::MenuItem( "PPU Debugger", NULL, &ppuDebugger );
        ImGui::MenuItem( "Profiler", NULL, &profilerOpened );
        ImGui::MenuItem( "PPU Status", NULL, &ppuStatus );
        ImGui::MenuItem( "APU Status", NULL, &apuOpened );
        ImGui::EndMenu();
    }


    ImGui::EndMainMenuBar();

    DrawScreen();
    DrawControlWindow();
    DrawDebugger();
    DrawCpuStatus();
    DrawCartridge();
    DrawMemoryViewer();
    DrawProfiler(fpsValues, previousProfiler);
    DrawPpuStatus();
    DrawController();
    DrawPpuDebugger();
    DrawApuStatus();

    //ImGui::ShowTestWindow( &opened );
}

void NesUI::ChangeDirectory() {
    nesRoms.clear();
    directories.clear();

    FindFilesInPath( ".nes", cwd.c_str(), nesRoms, directories );
}

} // namespace hydra
#include "Hydra/Kernel/Lib.h"
#include "Hydra/Kernel/Input.h"

#include <string>
#include <dbt.h>

namespace hydra {

AssertCallbackDelegate AssertCallbackDelegateImpl = AssertCallback;

bool AssertCallback( cstring filename, uint32 line, cstring cond ) {
    PrintFormat( "%s(%u): %s\n", filename, line, cond );
    return true;
}


//////////////////////////////////////////////////////////////////////////
char* GetNextToken( char* str, const char* delimiters, char** nextToken ) {
    return strtok_s( str, delimiters, nextToken );
}

void TrimLeft2( char* token, msize tokenSize, int& indexStart, char charToTrim0, char charToTrim1 ) {
    char* c = token;
    while ( *c == charToTrim0 || *c == charToTrim1 ) {
        ++c;
    }
    indexStart = (int)( c - token );
}


void TrimRight2( char* token, msize tokenSize, int& indexEnd, char charToTrim0, char charToTrim1 ) {
    char* c = token + tokenSize - 1;
    while ( *c == charToTrim0 || *c == charToTrim1 ) {
        --c;
    }
    indexEnd = (int)( c - token );
}

void TrimLeft( char* token, msize tokenSize, int& indexStart, char charToTrim ) {
    char* c = token;
    while ( *c == charToTrim ) {
        ++c;
    }
    indexStart = (int)( c - token );
}


void TrimRight( char* token, msize tokenSize, int& indexEnd, char charToTrim ) {
    char* c = token + tokenSize - 1;
    while ( *c == charToTrim ) {
        --c;
    }
    indexEnd = (int)( c - token );
}

void Tokenize( char* tokens, char* delimiter, std::vector<std::string>& tokenList ) {
    char* nextToken = nullptr;
    char* token = GetNextToken( tokens, delimiter, &nextToken );

    while ( token != nullptr ) {
        printf( "%s\n", token );

        int start, end;
        msize tokenSize = strlen( token );
        if ( tokenSize ) {
            TrimLeft( token, tokenSize, start, ' ' );
            TrimRight( token, tokenSize, end, ' ' );

            std::string stringToken( token );
            std::string trimmedToken = stringToken.substr( start, end - start + 1 );
            tokenList.emplace_back( trimmedToken );
        }

        token = GetNextToken( nullptr, delimiter, &nextToken );
    }
}

void Tokenize2( char* tokens, char* delimiter, std::vector<std::string>& tokenList ) {
    char* nextToken = nullptr;
    char* token = GetNextToken( tokens, delimiter, &nextToken );

    while ( token != nullptr ) {
        printf( "%s\n", token );

        int start, end;
        msize tokenSize = strlen( token );
        if ( tokenSize ) {

            TrimLeft2( token, tokenSize, start, ' ', '"' );
            TrimRight2( token, tokenSize, end, ' ', '"' );

            std::string stringToken( token );
            std::string trimmedToken = stringToken.substr( start, end - start + 1 );
            tokenList.emplace_back( trimmedToken );
        }

        token = GetNextToken( nullptr, delimiter, &nextToken );
    }
}

void CreateEnum( FILE* Output, char* tokens, const char* name, bool generateMasks, bool addMax, bool fromJsonArray ) {

    std::vector<std::string> enumValues;
    if ( fromJsonArray )
        Tokenize2( tokens, ",", enumValues );
    else
        Tokenize( tokens, ",", enumValues );

    std::string allValues;
    std::string allMasks;
    std::string allStrings( "\"" );

    for ( unsigned int i = 0; i < enumValues.size() - 1; ++i ) {

        if ( generateMasks ) {
            allMasks += enumValues[i];
            allMasks += "_mask = 1 << ";
            allMasks += std::to_string( i );
            allMasks += ", ";
        }

        allValues += enumValues[i];
        allValues += ", ";

        allStrings += enumValues[i];
        allStrings += "\", \"";
    }

    allMasks += enumValues.back();
    allMasks += "_mask = 1 << ";
    allMasks += std::to_string( enumValues.size() - 1 );

    allValues += enumValues.back();

    allStrings += enumValues.back();
    allStrings += "\"";

    if ( addMax ) {
        allValues += ", max";

        allStrings += ", \"max\"";

        allMasks += ", max_mask = 1 << ";
        allMasks += std::to_string( enumValues.size() );
    }

    // Write the actual enum
    fprintf( Output, "namespace %s {\n", name );

    fprintf( Output, "\tenum Enum {\n" );
    fprintf( Output, "\t\t%s\n", allValues.c_str() );
    fprintf( Output, "\t};\n" );

    // Write the mask
    if ( generateMasks ) {
        fprintf( Output, "\n\tenum Mask {\n" );
        fprintf( Output, "\t\t%s\n", allMasks.c_str() );
        fprintf( Output, "\t};\n" );
    }

    // Write the string values
    fprintf( Output, "\n\tstatic const char* sStrings[] = {\n" );
    fprintf( Output, "\t\t%s\n", allStrings.c_str() );
    fprintf( Output, "\t};\n" );

    fprintf( Output, "\n\tstatic const char* ToString( Enum e ) {\n" );
    fprintf( Output, "\t\treturn sStrings[(int)e];\n" );
    fprintf( Output, "\t}\n" );

    fprintf( Output, "}\n" );
    // fprintf( Output, "};\n\n" );
}

//////////////////////////////////////////////////////////////////////////
// http://blog.molecular-matters.com/2011/09/05/properly-handling-keyboard-input/
// http://blog.molecular-matters.com/2011/09/29/wiring-physical-devices-to-abstract-inputs/

#if defined(_WIN64)

static void GetWindowSizes( WindowSystem& w ) {
     // Retrieve frame buffer actual size
    RECT clientRect, windowRect;
    GetClientRect( w.handle, &clientRect );
    // Update window area
    GetWindowRect( w.handle, &windowRect );

    w.windowArea = { (uint16)windowRect.left, (uint16)windowRect.top, (uint16)(windowRect.right - windowRect.left),
                     (uint16)(windowRect.bottom - windowRect.top) };

    w.frameBufferWidth = (uint16)clientRect.right;
    w.frameBufferHeight = (uint16)clientRect.bottom;
}


static void GetMouseCoords(LPARAM lParam, int32& x, int32& y) {
    x = LOWORD( lParam );
    y = HIWORD( lParam );
}

static void SetMouseState(LPARAM lParam, bool down, input::MouseButtons::Buttons button, WindowSystem& window) {
    int32 mouseX, mouseY;
    GetMouseCoords( lParam, mouseX, mouseY );

    window::events::MouseClick mouseEvent{ (uint16)mouseX, (uint16)mouseY, (uint8)button, down ? (uint8)0 : (uint8)1 };
    window._eventStream.AddEvent( &mouseEvent );
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

    using namespace hydra::input;

    WindowSystem* window = (WindowSystem*)GetWindowLongPtr( hWnd, GWLP_USERDATA );
    switch ( uMsg ) {

        case WM_QUIT:
            window->flags.set( WindowSystem::Flags_requestExit );
            window->callbacksActivationMask.set( window::callbacks::Type_RequestExit );

            break;

        case WM_CLOSE:
            DestroyWindow( hWnd );

            window->flags.set( WindowSystem::Flags_requestExit );
            window->callbacksActivationMask.set( window::callbacks::Type_RequestExit );

            break;

        case WM_CREATE:
            if ( window == nullptr ) {
                CREATESTRUCT* createStruct;
                createStruct = (CREATESTRUCT*)( lParam );
                window = static_cast<WindowSystem*>( createStruct->lpCreateParams );
                SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)window );
            }
            break;

        case WM_KILLFOCUS: {
            if ( window ) {
                window::events::ChangeFocus event{ false };
                window->_eventStream.AddEvent( &event );

                window->flags.set( WindowSystem::Flags_focus );
                window->callbacksActivationMask.set( window::callbacks::Type_ChangeFocus );
                window->focusData.inFocus = false;
            }
            break;
        }

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        case WM_ACTIVATE: {
            int32 status = LOWORD( wParam );
            window->flags.set( WindowSystem::Flags_inFocus, status != WA_INACTIVE);
            break;
        }

        case WM_SIZE: {
            //https://msdn.microsoft.com/en-us/library/windows/desktop/ms632646(v=vs.85).aspx
            // TODO: proper way of handling resize!
            int32 newWidth = LOWORD( lParam );
            int32 newHeight = HIWORD( lParam );
            window::events::Resize event{ (uint16)newWidth, (uint16)newHeight };
            window->_eventStream.AddEvent( &event );
            window->callbacksActivationMask.set( window::callbacks::Type_WindowResize );

            window->isResizing = false;
            // Calculate new 'requested size'
            window->requestedWidth += newWidth - window->frameBufferWidth;
            window->requestedHeight += newHeight - window->frameBufferHeight;

            GetWindowSizes( *window );

            break;
        }

        case WM_DEVICECHANGE: {
            // TODO: for gamepad wParam is always set to 7.
            // Just fire the callback and let the application handle this.
            window::events::DeviceChange deviceChange{ 0, 0 };
            window->_eventStream.AddEvent( &deviceChange );
            window->callbacksActivationMask.set( window::callbacks::Type_ChangeDevice );

            break;
        }

        case WM_MOUSEMOVE: {
            int32 mouseX, mouseY;
            GetMouseCoords( lParam, mouseX, mouseY );

            window::events::MouseMove mouseEvent { (uint16)mouseX, (uint16)mouseY };
            window->_eventStream.AddEvent( &mouseEvent );
            
            break;
        }
        case WM_LBUTTONDOWN:
            SetMouseState( lParam, true, hydra::input::MouseButtons::Left, *window );
            break;

        case WM_MBUTTONDOWN:
            SetMouseState( lParam, true, hydra::input::MouseButtons::Center, *window );
            break;

        case WM_RBUTTONDOWN:
            SetMouseState( lParam, true, hydra::input::MouseButtons::Right, *window );
            break;

        case WM_LBUTTONUP:
            SetMouseState( lParam, false, hydra::input::MouseButtons::Left, *window );
            break;

        case WM_MBUTTONUP:
            SetMouseState( lParam, false, hydra::input::MouseButtons::Center, *window );
            break;

        case WM_RBUTTONUP:
            SetMouseState( lParam, false, hydra::input::MouseButtons::Right, *window );
            break;

        case WM_MOUSEWHEEL: {
            window::events::MouseWheel mouseEvent{ (int16)HIWORD( wParam ) };
            window->_eventStream.AddEvent( &mouseEvent );
            break;
        }
        case WM_INPUT:
            {
                char buffer[sizeof( RAWINPUT )] = {};
                UINT size = sizeof( RAWINPUT );
                GetRawInputData( reinterpret_cast<HRAWINPUT>( lParam ), RID_INPUT, buffer, &size, sizeof( RAWINPUTHEADER ) );

                // extract keyboard raw input data
                RAWINPUT* raw = reinterpret_cast<RAWINPUT*>( buffer );
                if ( raw->header.dwType == RIM_TYPEKEYBOARD ) {
                    const RAWKEYBOARD& rawKB = raw->data.keyboard;
                    // do something with the data here
                    UINT virtualKey = rawKB.VKey;
                    UINT scanCode = rawKB.MakeCode;
                    UINT flags = rawKB.Flags;

                    if ( virtualKey == 255 ) {
                        // discard "fake keys" which are part of an escaped sequence
                        return 0;
                    } else if ( virtualKey == VK_SHIFT ) {
                        // correct left-hand / right-hand SHIFT
                        virtualKey = MapVirtualKey( scanCode, MAPVK_VSC_TO_VK_EX );
                    } else if ( virtualKey == VK_NUMLOCK ) {
                        // correct PAUSE/BREAK and NUM LOCK silliness, and set the extended bit
                        scanCode = ( MapVirtualKey( rawKB.VKey, MAPVK_VK_TO_VSC ) | 0x100 );
                    }

                    if ( raw->data.keyboard.Message == WM_KEYDOWN ) {
                        WORD wideChar;
                        BYTE lpKeyboard[256];

                        GetKeyState( VK_CAPITAL ); GetKeyState( VK_SCROLL ); GetKeyState( VK_NUMLOCK );
                        GetKeyboardState( lpKeyboard );

                        if ( ToAscii( virtualKey, scanCode, lpKeyboard, &wideChar, 0 ) == 1 ) {
                            //PrintFormat( "KeyChar %c\n", (char)wideChar );
                            window->callbacksActivationMask.set( window::callbacks::Type_CharWritten );
                            window->charCallbackData.ansiChar = (char)wideChar;
                        }
                    }

                    // e0 and e1 are escape sequences used for certain special keys, such as PRINT and PAUSE/BREAK.
                    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                    const bool isE0 = ( ( flags & RI_KEY_E0 ) != 0 );
                    const bool isE1 = ( ( flags & RI_KEY_E1 ) != 0 );

                    if ( isE1 ) {
                        // for escaped sequences, turn the virtual key into the correct scan code using MapVirtualKey.
                        // however, MapVirtualKey is unable to map VK_PAUSE (this is a known bug), hence we map that by hand.
                        if ( virtualKey == VK_PAUSE )
                            scanCode = 0x45;
                        else
                            scanCode = MapVirtualKey( virtualKey, MAPVK_VK_TO_VSC );
                    }

                    switch ( virtualKey ) {
                        // right-hand CONTROL and ALT have their e0 bit set
                        case VK_CONTROL:
                            if ( isE0 )
                                virtualKey = Keys::KEY_RCONTROL;
                            else
                                virtualKey = Keys::KEY_LCONTROL;
                            break;

                        case VK_MENU:
                            if ( isE0 )
                                virtualKey = Keys::KEY_RALT;
                            else
                                virtualKey = Keys::KEY_LALT;
                            break;

                            // NUMPAD ENTER has its e0 bit set
                        case VK_RETURN:
                            // TODO: no numpad key ?
                            if ( isE0 )
                                virtualKey = Keys::KEY_SEPARATOR;
                            break;

                            // the standard INSERT, DELETE, HOME, END, PRIOR and NEXT keys will always have their e0 bit set, but the
                            // corresponding keys on the NUMPAD will not.
                        case VK_INSERT:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD0;
                            break;

                        case VK_DELETE:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_DECIMAL;
                            break;

                        case VK_HOME:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD7;
                            break;

                        case VK_END:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD1;
                            break;

                        case VK_PRIOR:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD9;
                            break;

                        case VK_NEXT:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD3;
                            break;

                            // the standard arrow keys will always have their e0 bit set, but the
                            // corresponding keys on the NUMPAD will not.
                        case VK_LEFT:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD4;
                            break;

                        case VK_RIGHT:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD6;
                            break;

                        case VK_UP:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD8;
                            break;

                        case VK_DOWN:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD2;
                            break;

                            // NUMPAD 5 doesn't have its e0 bit set
                        case VK_CLEAR:
                            if ( !isE0 )
                                virtualKey = Keys::KEY_NUMPAD5;
                            break;
                    }

                    // a key can either produce a "make" or "break" scancode. this is used to differentiate between down - presses and releases
                    // see http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html
                    const bool wasUp = ( ( flags & RI_KEY_BREAK ) != 0 );

                    window::events::KeyInput inputEvent = { (uint8)virtualKey, !wasUp };
                    window->_eventStream.AddEvent( &inputEvent );

                    // Add callback data
                    window->callbacksActivationMask.set( window::callbacks::Type_KeyModified );
                    window->keyCallbackData.keyDown = !wasUp;
                    window->keyCallbackData.virtualKey = (uint8)virtualKey;

                    
                    // getting a human-readable string
                    //UINT key = ( scanCode << 16 ) | ( isE0 << 24 );
                    //char buffer[512] = {};
                    //GetKeyNameTextA( (LONG)key, buffer, 512 );
                    //PrintFormat( "VK: 0x%02X, %s, was%s, name %s, desc %s, keyChar %c\n", virtualKey, buffer, wasUp ? "Up" : "Down", input::sNames[virtualKey], input::sDescriptions[virtualKey], '0' );
                    
                }
            }
            break;

        default:
            return ( DefWindowProc( hWnd, uMsg, wParam, lParam ) );
    }

    return 0;
}

void InitRawInput( WindowHandle hWnd ) {
    RAWINPUTDEVICE device;
    device.usUsagePage = 1;
    device.usUsage = 6;
    // TODO: removing this breaks the conversion virtual key -> char
    device.dwFlags = 0;// RIDEV_NOLEGACY | RIDEV_INPUTSINK;        // do not generate legacy messages such as WM_KEYDOWN
    device.hwndTarget = hWnd;
    RegisterRawInputDevices( &device, 1, sizeof( device ) );
}
#endif // _WIN64


static const char* sStrings[] = {
    "Type_RequestExit", "Type_CharWritten", "Type_KeyModified", "Type_Resize", "Type_ChangeFocus", "Type_DeviceChange"
};

cstring window::callbacks::ToString( window::callbacks::Types e ) {
    return sStrings[(int)e];
}

static void InitWindow( const WindowInit& desc ) {

#if defined(_WIN64)
    WNDCLASSEXA wndClass;
    wndClass.cbSize = sizeof( WNDCLASSEXA );
    wndClass.style = CS_HREDRAW | CS_VREDRAW;
    wndClass.lpfnWndProc = WndProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = desc.process;
    wndClass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
    wndClass.hCursor = LoadCursor( NULL, IDC_ARROW );
    wndClass.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = desc.name;
    wndClass.hIconSm = LoadIcon( NULL, IDI_WINLOGO );

    if ( !RegisterClassExA( &wndClass ) ) {
        PrintFormat( "Could not register window class!\n" );
        exit( 1 );
    }

    DWORD dwExStyle;
    DWORD dwStyle;

    const bool fullscreen = desc.fullscreen;
    int screenWidth = GetSystemMetrics( SM_CXSCREEN );
    int screenHeight = GetSystemMetrics( SM_CYSCREEN );

    if ( fullscreen ) {
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    } else {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    }

    RECT windowRect;
    if ( fullscreen ) {
        windowRect = { 0, 0, screenWidth, screenHeight };
        
    } else {
        windowRect = { (long)screenWidth / 2 - desc.rect.width / 2, (long)screenHeight / 2 - desc.rect.height / 2,
                       (long)desc.rect.width, (long)desc.rect.height };
    }

    AdjustWindowRectEx( &windowRect, dwStyle, FALSE, dwExStyle );

    HWND windowHandle = CreateWindowExA( 0, desc.name, desc.title, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        windowRect.left, windowRect.top, windowRect.right, windowRect.bottom, NULL, NULL, desc.process, desc.outWindow );

    if ( !windowHandle ) {
        PrintFormat( "Could not create window!\n" );
        exit( 1 );
    }

    
    WindowSystem& w = *desc.outWindow;
    w.flags.set( WindowSystem::Flags_fullscreen, desc.fullscreen );
    w.name = desc.name;
    w.handle = windowHandle;
    w.processHandle = desc.process;
    w._eventStream._blob = desc.blob;
    w.requestedWidth = desc.rect.width;
    w.requestedHeight = desc.rect.height;

    // Retrieve framebuffer and window sizes
    GetWindowSizes( w );
   
    InitRawInput( windowHandle );

    ShowWindow( windowHandle, SW_SHOW );
    SetForegroundWindow( windowHandle );
    SetFocus( windowHandle );

#else
#endif
}


WindowSystem::WindowSystem( WindowInit& init, uint32 order ) : UpdateSystem(order) {
    init.outWindow = this;
    InitWindow( init );

    callbacksActivationMask = 0;
    isResizing = false;
}


void WindowSystem::Update() {
    _eventStream.Reset();

#if defined(_WIN64)
    MSG messages;
    while ( PeekMessage( &messages, NULL, 0, 0, PM_REMOVE ) ) {

        TranslateMessage( &messages );
        DispatchMessage( &messages );
    }
#else
#endif
}


void WindowSystem::ExecuteCallbacks() {
    if ( callbacksActivationMask.test(window::callbacks::Type_RequestExit ) ) {

        PrintFormat( "Requesting exit.\n" );
        for ( uint32 i = 0; i < exitCallbacks.size(); ++i ) {
            window::callbacks::RequestExit& callback = exitCallbacks[i];
            callback( exitCallbacksUserData[i] );
        }

        callbacksActivationMask.reset(window::callbacks::Type_RequestExit );
    }

    if ( callbacksActivationMask.test( window::callbacks::Type_CharWritten ) ) {
        for ( uint32 i = 0; i < charCallbacks.size(); ++i ) {
            window::callbacks::CharModified& callback = charCallbacks[i];
            callback( charCallbackUserData[i], charCallbackData );
        }

        callbacksActivationMask.reset( window::callbacks::Type_CharWritten );
    }

    if ( callbacksActivationMask.test( window::callbacks::Type_KeyModified ) ) {
        for ( uint32 i = 0; i < keyCallbacks.size(); ++i ) {
            window::callbacks::KeyModified& callback = keyCallbacks[i];
            callback( keyCallbackUserData[i], keyCallbackData );
        }

        callbacksActivationMask.reset( window::callbacks::Type_KeyModified );
    }

    if ( callbacksActivationMask.test( window::callbacks::Type_WindowResize ) ) {
        for ( uint32 i = 0; i < resizeCallbacks.size(); ++i ) {
            window::callbacks::WindowResized& callback = resizeCallbacks[i];
            callback( resizeCallbackUserData[i], resizeData );
        }

        callbacksActivationMask.reset( window::callbacks::Type_WindowResize );
    }

    if ( callbacksActivationMask.test( window::callbacks::Type_ChangeFocus ) ) {
        for ( uint32 i = 0; i < focusCallbacks.size(); ++i ) {
            window::callbacks::ChangeFocus& callback = focusCallbacks[i];
            callback( focusCallbackUserData[i], focusData );
        }

        callbacksActivationMask.reset( window::callbacks::Type_ChangeFocus );
    }

    if ( callbacksActivationMask.test( window::callbacks::Type_ChangeDevice ) ) {
        for ( uint32 i = 0; i < deviceCallbacks.size(); ++i ) {
            window::callbacks::ChangeDevice& callback = deviceCallbacks[i];
            callback( deviceCallbackUserData[i], deviceData );
        }

        callbacksActivationMask.reset( window::callbacks::Type_ChangeDevice );
    }
}

void WindowSystem::AddRequestExitCallback( window::callbacks::RequestExit callback, void* userData ) {
    exitCallbacks.push_back( callback );
    exitCallbacksUserData.push_back( userData );
}

void WindowSystem::AddKeyCallback( window::callbacks::KeyModified callback, void * userData ) {
    keyCallbacks.push_back( callback );
    keyCallbackUserData.push_back( userData );
}

void WindowSystem::AddCharCallback( window::callbacks::CharModified callback, void * userData ) {
    charCallbacks.push_back( callback );
    charCallbackUserData.push_back( userData );
}

void WindowSystem::AddResizeCallback( window::callbacks::WindowResized callback, void * userData ) {
    resizeCallbacks.push_back( callback );
    resizeCallbackUserData.push_back( userData );
}

void WindowSystem::AddFocusCallback( window::callbacks::ChangeFocus callback, void * userData ) {
    focusCallbacks.push_back( callback );
    focusCallbackUserData.push_back( userData );
}

void WindowSystem::AddDeviceCallback( window::callbacks::ChangeDevice callback, void* userData ) {
    deviceCallbacks.push_back( callback );
    deviceCallbackUserData.push_back( userData );
}

//////////////////////////////////////////////////////////////////////////

TimeSystem::TimeSystem( uint32 order ) : UpdateSystem(order) {
    _deltaMs = 0;
    _frameStart = time::Now();
    _frameCount = 0;
}


void TimeSystem::Update() {
    ++_frameCount;
    _deltaMs = time::IntervalMilliseconds( time::Now(), _frameStart );

    _frameStart = time::Now();
}

//////////////////////////////////////////////////////////////////////////
ProfilerSystem::ProfilerSystem( uint32 order ) : UpdateSystem( order ) {

}

void ProfilerSystem::Update() {

}

void ProfilerSystem::Start() {

}

void ProfilerSystem::End() {

}

void ProfilerSystem::PushSample( cstring name ) {
    Commands::Push e = { name, time::Now() };
    _eventStream.AddEvent<Commands::Push>( &e );
}

void ProfilerSystem::PopSample() {
    Commands::Pop e = { time::Now() };
    _eventStream.AddEvent<Commands::Pop>( &e );
}

} // namespace hydra
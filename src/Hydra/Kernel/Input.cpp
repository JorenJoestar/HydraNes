#include "Hydra/Kernel/Input.h"
#include "Hydra/Kernel/Lib.h"

namespace hydra {

namespace input {
/*

void InputSystem::Update() {
    
    // get gamepad ?
}*/

//////////////////////////////////////////////////////////////////////////
// Creating enum and descs straight from the virtual keys:
// https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx?f=255&MSPPError=-2147217396
//////////////////////////////////////////////////////////////////////////

struct hhb_vkey {
    char* name;
    uint8_t vkey;
    char* description;
};

const hhb_vkey hhb_vkey_list[] = {
    { "KEY_START", 0x00, "" },
    { "KEY_LBUTTON", 0x01, "Left mouse button" },
    { "KEY_RBUTTON", 0x02, "Right mouse button" },
    { "KEY_CANCEL", 0x03, "Control-break processing" },
    { "KEY_MBUTTON", 0x04, "Middle mouse button (three-button mouse)" },
    { "KEY_XBUTTON1", 0x05, "X1 mouse button" },
    { "KEY_XBUTTON2", 0x06, "X2 mouse button" },
    { "KEY_UNDEF0", 0x07, "Undefined" },
    { "KEY_BACK", 0x08, "BACKSPACE key" },
    { "KEY_TAB", 0x09, "TAB key" },
    { "KEY_UNDEF1", 0x0A, "Reserved" },
    { "KEY_UNDEF1B", 0x0B, "Reserved" },
    { "KEY_CLEAR", 0x0C, "CLEAR key" },
    { "KEY_RETURN", 0x0D, "ENTER key" },
    { "KEY_UNDEF2", 0x0E, "Undefined" },
    { "KEY_UNDEF2B", 0x0F, "Undefined" },
    { "KEY_SHIFT", 0x10, "SHIFT key" },
    { "KEY_CONTROL", 0x11, "CTRL key" },
    { "KEY_MENU", 0x12, "ALT key" },
    { "KEY_PAUSE", 0x13, "PAUSE key" },
    { "KEY_CAPITAL", 0x14, "CAPS LOCK key" },
    { "KEY_KANA", 0x15, "IME Kana mode" },
    /* { "KEY_HANGUEL", 0x15, "IME Hanguel mode (maintained for compatibility; use KEY_HANGUL)" },
    { "KEY_HANGUL", 0x15, "IME Hangul mode" },*/
    { "KEY_UNDEF3", 0x16, "Undefined" },
    { "KEY_JUNJA", 0x17, "IME Junja mode" },
    { "KEY_FINAL", 0x18, "IME final mode" },
    { "KEY_HANJA", 0x19, "IME Hanja mode" },
    //{ "KEY_KANJI", 0x19, "IME Kanji mode" },
    { "KEY_UNDEF4", 0x1A, "Undefined" },
    { "KEY_ESCAPE", 0x1B, "ESC key" },
    { "KEY_CONVERT", 0x1C, "IME convert" },
    { "KEY_NONCONVERT", 0x1D, "IME nonconvert" },
    { "KEY_ACCEPT", 0x1E, "IME accept" },
    { "KEY_MODECHANGE", 0x1F, "IME mode change request" },
    { "KEY_SPACE", 0x20, "SPACEBAR" },
    { "KEY_PRIOR", 0x21, "PAGE UP key" },
    { "KEY_NEXT", 0x22, "PAGE DOWN key" },
    { "KEY_END", 0x23, "END key" },
    { "KEY_HOME", 0x24, "HOME key" },
    { "KEY_LEFT", 0x25, "LEFT ARROW key" },
    { "KEY_UP", 0x26, "UP ARROW key" },
    { "KEY_RIGHT", 0x27, "RIGHT ARROW key" },
    { "KEY_DOWN", 0x28, "DOWN ARROW key" },
    { "KEY_SELECT", 0x29, "SELECT key" },
    { "KEY_PRINT", 0x2A, "PRINT key" },
    { "KEY_EXECUTE_", 0x2B, "EXECUTE key" },
    { "KEY_SNAPSHOT", 0x2C, "PRINT SCREEN key" },
    { "KEY_INSERT", 0x2D, "INS key" },
    { "KEY_DELETE", 0x2E, "DEL key" },
    { "KEY_HELP", 0x2F, "HELP key" },
    { "KEY_0", 0x30, "0 key" },
    { "KEY_1", 0x31, "1 key" },
    { "KEY_2", 0x32, "2 key" },
    { "KEY_3", 0x33, "3 key" },
    { "KEY_4", 0x34, "4 key" },
    { "KEY_5", 0x35, "5 key" },
    { "KEY_6", 0x36, "6 key" },
    { "KEY_7", 0x37, "7 key" },
    { "KEY_8", 0x38, "8 key" },
    { "KEY_9", 0x39, "9 key" },
    { "KEY_UNDEF5", 0x3A, "undefined" },
    { "KEY_UNDEF5A", 0x3B, "undefined" },
    { "KEY_UNDEF5B", 0x3C, "undefined" },
    { "KEY_UNDEF5C", 0x3D, "undefined" },
    { "KEY_UNDEF5D", 0x3E, "undefined" },
    { "KEY_UNDEF5E", 0x3F, "undefined" },
    { "KEY_UNDEF5F", 0x40, "undefined" },
    { "KEY_A", 0x41, "A key" },
    { "KEY_B", 0x42, "B key" },
    { "KEY_C", 0x43, "C key" },
    { "KEY_D", 0x44, "D key" },
    { "KEY_E", 0x45, "E key" },
    { "KEY_F", 0x46, "F key" },
    { "KEY_G", 0x47, "G key" },
    { "KEY_H", 0x48, "H key" },
    { "KEY_I", 0x49, "I key" },
    { "KEY_J", 0x4A, "J key" },
    { "KEY_K", 0x4B, "K key" },
    { "KEY_L", 0x4C, "L key" },
    { "KEY_M", 0x4D, "M key" },
    { "KEY_N", 0x4E, "N key" },
    { "KEY_O", 0x4F, "O key" },
    { "KEY_P", 0x50, "P key" },
    { "KEY_Q", 0x51, "Q key" },
    { "KEY_R", 0x52, "R key" },
    { "KEY_S", 0x53, "S key" },
    { "KEY_T", 0x54, "T key" },
    { "KEY_U", 0x55, "U key" },
    { "KEY_V", 0x56, "V key" },
    { "KEY_W", 0x57, "W key" },
    { "KEY_X", 0x58, "X key" },
    { "KEY_Y", 0x59, "Y key" },
    { "KEY_Z", 0x5A, "Z key" },
    { "KEY_LWIN", 0x5B, "Left Windows key (Natural keyboard)" },
    { "KEY_RWIN", 0x5C, "Right Windows key (Natural keyboard)" },
    { "KEY_APPS", 0x5D, "Applications key (Natural keyboard)" },
    { "KEY_UNDEF6", 0x5E, "Reserved" },
    { "KEY_SLEEP", 0x5F, "Computer Sleep key" },
    { "KEY_NUMPAD0", 0x60, "Numeric keypad 0 key" },
    { "KEY_NUMPAD1", 0x61, "Numeric keypad 1 key" },
    { "KEY_NUMPAD2", 0x62, "Numeric keypad 2 key" },
    { "KEY_NUMPAD3", 0x63, "Numeric keypad 3 key" },
    { "KEY_NUMPAD4", 0x64, "Numeric keypad 4 key" },
    { "KEY_NUMPAD5", 0x65, "Numeric keypad 5 key" },
    { "KEY_NUMPAD6", 0x66, "Numeric keypad 6 key" },
    { "KEY_NUMPAD7", 0x67, "Numeric keypad 7 key" },
    { "KEY_NUMPAD8", 0x68, "Numeric keypad 8 key" },
    { "KEY_NUMPAD9", 0x69, "Numeric keypad 9 key" },
    { "KEY_MULTIPLY", 0x6A, "Multiply key" },
    { "KEY_ADD", 0x6B, "Add key" },
    { "KEY_SEPARATOR", 0x6C, "Separator key" },
    { "KEY_SUBTRACT", 0x6D, "Subtract key" },
    { "KEY_DECIMAL", 0x6E, "Decimal key" },
    { "KEY_DIVIDE", 0x6F, "Divide key" },
    { "KEY_F1", 0x70, "F1 key" },
    { "KEY_F2", 0x71, "F2 key" },
    { "KEY_F3", 0x72, "F3 key" },
    { "KEY_F4", 0x73, "F4 key" },
    { "KEY_F5", 0x74, "F5 key" },
    { "KEY_F6", 0x75, "F6 key" },
    { "KEY_F7", 0x76, "F7 key" },
    { "KEY_F8", 0x77, "F8 key" },
    { "KEY_F9", 0x78, "F9 key" },
    { "KEY_F10", 0x79, "F10 key" },
    { "KEY_F11", 0x7A, "F11 key" },
    { "KEY_F12", 0x7B, "F12 key" },
    { "KEY_F13", 0x7C, "F13 key" },
    { "KEY_F14", 0x7D, "F14 key" },
    { "KEY_F15", 0x7E, "F15 key" },
    { "KEY_F16", 0x7F, "F16 key" },
    { "KEY_F17", 0x80, "F17 key" },
    { "KEY_F18", 0x81, "F18 key" },
    { "KEY_F19", 0x82, "F19 key" },
    { "KEY_F20", 0x83, "F20 key" },
    { "KEY_F21", 0x84, "F21 key" },
    { "KEY_F22", 0x85, "F22 key" },
    { "KEY_F23", 0x86, "F23 key" },
    { "KEY_F24", 0x87, "F24 key" },
    { "KEY_UNDEF7", 0x88, "Unassigned" },
    { "KEY_UNDEF7B", 0x89, "Unassigned" },
    { "KEY_UNDEF7C", 0x8A, "Unassigned" },
    { "KEY_UNDEF7D", 0x8B, "Unassigned" },
    { "KEY_UNDEF7E", 0x8C, "Unassigned" },
    { "KEY_UNDEF7F", 0x8D, "Unassigned" },
    { "KEY_UNDEF7G", 0x8E, "Unassigned" },
    { "KEY_UNDEF7H", 0x8F, "Unassigned" },
    { "KEY_NUMLOCK", 0x90, "NUM LOCK key" },
    { "KEY_SCROLL", 0x91, "SCROLL LOCK key" },
    //{"-", 0x92-96,"OEM specific"},
    { "KEY_UNDEF8",  0x92, "Unassigned" },
    { "KEY_UNDEF9",  0x93, "Unassigned" },
    { "KEY_UNDEF10", 0x94, "Unassigned" },
    { "KEY_UNDEF11", 0x95, "Unassigned" },
    { "KEY_UNDEF12", 0x96, "Unassigned" },
    { "KEY_UNDEF13", 0x97, "Unassigned" },
    { "KEY_UNDEF13A", 0x98, "Unassigned" },
    { "KEY_UNDEF13B", 0x99, "Unassigned" },
    { "KEY_UNDEF13C", 0x9A, "Unassigned" },
    { "KEY_UNDEF13D", 0x9B, "Unassigned" },
    { "KEY_UNDEF13E", 0x9C, "Unassigned" },
    { "KEY_UNDEF13F", 0x9D, "Unassigned" },
    { "KEY_UNDEF13G", 0x9E, "Unassigned" },
    { "KEY_UNDEF13H", 0x9F, "Unassigned" },
    { "KEY_LSHIFT", 0xA0, "Left SHIFT key" },
    { "KEY_RSHIFT", 0xA1, "Right SHIFT key" },
    { "KEY_LCONTROL", 0xA2, "Left CONTROL key" },
    { "KEY_RCONTROL", 0xA3, "Right CONTROL key" },
    { "KEY_LALT", 0xA4, "Left ALT key" },
    { "KEY_RALT", 0xA5, "Right ALT key" },
    { "KEY_BROWSER_BACK", 0xA6, "Browser Back key" },
    { "KEY_BROWSER_FORWARD", 0xA7, "Browser Forward key" },
    { "KEY_BROWSER_REFRESH", 0xA8, "Browser Refresh key" },
    { "KEY_BROWSER_STOP", 0xA9, "Browser Stop key" },
    { "KEY_BROWSER_SEARCH", 0xAA, "Browser Search key" },
    { "KEY_BROWSER_FAVORITES", 0xAB, "Browser Favorites key" },
    { "KEY_BROWSER_HOME", 0xAC, "Browser Start and Home key" },
    { "KEY_VOLUME_MUTE", 0xAD, "Volume Mute key" },
    { "KEY_VOLUME_DOWN", 0xAE, "Volume Down key" },
    { "KEY_VOLUME_UP", 0xAF, "Volume Up key" },
    { "KEY_MEDIA_NEXT_TRACK", 0xB0, "Next Track key" },
    { "KEY_MEDIA_PREV_TRACK", 0xB1, "Previous Track key" },
    { "KEY_MEDIA_STOP", 0xB2, "Stop Media key" },
    { "KEY_MEDIA_PLAY_PAUSE", 0xB3, "Play/Pause Media key" },
    { "KEY_LAUNCH_MAIL", 0xB4, "Start Mail key" },
    { "KEY_LAUNCH_MEDIA_SELECT", 0xB5, "Select Media key" },
    { "KEY_LAUNCH_APP1", 0xB6, "Start Application 1 key" },
    { "KEY_LAUNCH_APP2", 0xB7, "Start Application 2 key" },
    { "KEY_UNDEF14", 0xB8, "Reserved" },
    { "KEY_UNDEF14b", 0xB9, "Reserved" },
    { "KEY_OEM_1", 0xBA, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \';:\' key KEY_OEM_PLUS" },
    { "KEY_OEM_PLUS", 0xBB, "For any country/region, the \'+\' key" },
    { "KEY_OEM_COMMA", 0xBC, "For any country/region, the \',\' key" },
    { "KEY_OEM_MINUS", 0xBD, "For any country/region, the \'-\' key" },
    { "KEY_OEM_PERIOD", 0xBE, "For any country/region, the \'.\' key" },
    { "KEY_OEM_2", 0xBF, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'/?\' key" },
    { "KEY_OEM_3", 0xC0, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'`~\' key" },
    { "KEY_RESERVED0", 0xC1, "Reserved" },
    { "KEY_RESERVED0A", 0xC2, "Reserved" },
    { "KEY_RESERVED0B", 0xC3, "Reserved" },
    { "KEY_RESERVED0C", 0xC4, "Reserved" },
    { "KEY_RESERVED0D", 0xC5, "Reserved" },
    { "KEY_RESERVED0E", 0xC6, "Reserved" },
    { "KEY_RESERVED0F", 0xC7, "Reserved" },
    { "KEY_RESERVED0G", 0xC8, "Reserved" },
    { "KEY_RESERVED0H", 0xC9, "Reserved" },
    { "KEY_RESERVED0I", 0xCA, "Reserved" },
    { "KEY_RESERVED0J", 0xCB, "Reserved" },
    { "KEY_RESERVED0K", 0xCC, "Reserved" },
    { "KEY_RESERVED0L", 0xCD, "Reserved" },
    { "KEY_RESERVED0M", 0xCE, "Reserved" },
    { "KEY_RESERVED0N", 0xCF, "Reserved" },

    { "KEY_RESERVED0O", 0xD0, "Reserved" },
    { "KEY_RESERVED0P", 0xD1, "Reserved" },
    { "KEY_RESERVED0Q", 0xD2, "Reserved" },
    { "KEY_RESERVED0R", 0xD3, "Reserved" },
    { "KEY_RESERVED0S", 0xD4, "Reserved" },
    { "KEY_RESERVED0T", 0xD5, "Reserved" },
    { "KEY_RESERVED0U", 0xD6, "Reserved" },
    { "KEY_RESERVED0V", 0xD7, "Reserved" },
    { "KEY_RESERVED0W", 0xD8, "Reserved" },
    { "KEY_RESERVED0X", 0xD9, "Reserved" },
    { "KEY_RESERVED0Y", 0xDA, "Reserved" },
    /*{ "KEY_RESERVED0", 0xC1 - D7, "Reserved" },
    { "KEY_UNDEF15", 0xD8 - DA, "Unassigned" },*/
    { "KEY_OEM_4", 0xDB, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'[{\' key" },
    { "KEY_OEM_5", 0xDC, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \\\\| key" },
    { "KEY_OEM_6", 0xDD, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \']}\' key" },
    { "KEY_OEM_7", 0xDE, "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \'single-quote/double-quote\' key" },
    { "KEY_OEM_8", 0xDF, "Used for miscellaneous characters; it can vary by keyboard." },
    { "KEY_RESERVED1", 0xE0, "Reserved" },
    { "KET_OEM_9", 0xE1, "OEM specific" },
    { "KEY_OEM_102", 0xE2, "Either the angle bracket key or the backslash key on the RT 102-key keyboard" },
    /*{ "KET_OEM_10", 0xE3 - E4, "OEM specific" },
    { "KEY_PROCESSKEY", 0xE5, "IME PROCESS key" },
    { "KET_OEM_11", 0xE6, "OEM specific" },
    { "KEY_PACKET", 0xE7, "Used to pass Unicode characters as if they were keystrokes. The KEY_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT, SendInput, WM_KEYDOWN, and WM_KEYUP" },
    { "KEY_UNDEF16", 0xE8, "Unassigned" },
    { "KEY_OEM_12",0xE6,"OEM specific"},
    { "KEY_ATTN", 0xF6, "Attn key" },
    { "KEY_CRSEL", 0xF7, "CrSel key" },
    { "KEY_EXSEL", 0xF8, "ExSel key" },
    { "KEY_EREOF", 0xF9, "Erase EOF key" },
    { "KEY_PLAY", 0xFA, "Play key" },
    { "KEY_ZOOM", 0xFB, "Zoom key" },
    { "KEY_NONAME", 0xFC, "Reserved" },
    { "KEY_PA1", 0xFD, "PA1 key" },
    { "KEY_OEM_CLEAR", 0xFE, "Clear key" }*/

};

void CreateInputKeys( FILE* Output ) {
    int size = _countof( hhb_vkey_list );

    std::vector<std::string> names;
    std::vector<std::string> descs;
    std::vector<uint8_t> vkeys;

    for ( int i = 0; i < size; ++i ) {
        const hhb_vkey& key = hhb_vkey_list[i];

        names.emplace_back( key.name );
        descs.emplace_back( key.description );
        vkeys.emplace_back( key.vkey );
    }

    const char* space = "input";
    const char* name = "Keys";
    fprintf( Output, "namespace %s {\n", "input" );

    fprintf( Output, "\tenum Enum %s {\n", name );
    // Write the enum
    for ( int i = 0; i < size; ++i ) {
        fprintf( Output, "\t\t%s = 0x%02X,\n", names[i].c_str(), vkeys[i] );
    }
    fprintf( Output, "\t};\n" );

    // Write the names
    fprintf( Output, "\n\tstatic const char* sNames[] = {\n" );
    fprintf( Output, "\t\t\"%s\",", names[0].c_str() );
    for ( int i = 1; i < size; ++i ) {
        fprintf( Output, " \"%s\",", names[i].c_str() );
    }
    fprintf( Output, "\t};\n" );

    // Write the descriptions
    fprintf( Output, "\n\tstatic const char* sDescriptions[] = {\n" );
    fprintf( Output, "\t\t\"%s\",", descs[0].c_str() );
    for ( int i = 1; i < size; ++i ) {
        fprintf( Output, " \"%s\",", descs[i].c_str() );
    }
    fprintf( Output, "\t};\n" );

    fprintf( Output, "} // namespace %s\n", space );
}


InputSystem::InputSystem( uint32 order ) : UpdateSystem( order ) {
    _input.mouse.Z = 0;
}

void InputSystem::Update() {
    
}


} // namespace input
} // namespace hydra
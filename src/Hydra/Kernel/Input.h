#pragma once

#include "Hydra/Kernel/Lib.h"

namespace hydra {
    namespace input {

        enum Keys {
            KEY_START = 0x00,
            KEY_LBUTTON = 0x01,
            KEY_RBUTTON = 0x02,
            KEY_CANCEL = 0x03,
            KEY_MBUTTON = 0x04,
            KEY_XBUTTON1 = 0x05,
            KEY_XBUTTON2 = 0x06,
            KEY_UNDEF0 = 0x07,
            KEY_BACK = 0x08,
            KEY_TAB = 0x09,
            KEY_UNDEF1 = 0x0A,
            KEY_UNDEF1B = 0x0B,
            KEY_CLEAR = 0x0C,
            KEY_RETURN = 0x0D,
            KEY_UNDEF2 = 0x0E,
            KEY_UNDEF2B = 0x0F,
            KEY_SHIFT = 0x10,
            KEY_CONTROL = 0x11,
            KEY_MENU = 0x12,
            KEY_PAUSE = 0x13,
            KEY_CAPITAL = 0x14,
            KEY_KANA = 0x15,
            KEY_UNDEF3 = 0x16,
            KEY_JUNJA = 0x17,
            KEY_FINAL = 0x18,
            KEY_HANJA = 0x19,
            KEY_UNDEF4 = 0x1A,
            KEY_ESCAPE = 0x1B,
            KEY_CONVERT = 0x1C,
            KEY_NONCONVERT = 0x1D,
            KEY_ACCEPT = 0x1E,
            KEY_MODECHANGE = 0x1F,
            KEY_SPACE = 0x20,
            KEY_PRIOR = 0x21,
            KEY_NEXT = 0x22,
            KEY_END = 0x23,
            KEY_HOME = 0x24,
            KEY_LEFT = 0x25,
            KEY_UP = 0x26,
            KEY_RIGHT = 0x27,
            KEY_DOWN = 0x28,
            KEY_SELECT = 0x29,
            KEY_PRINT = 0x2A,
            KEY_EXECUTE_ = 0x2B,
            KEY_SNAPSHOT = 0x2C,
            KEY_INSERT = 0x2D,
            KEY_DELETE = 0x2E,
            KEY_HELP = 0x2F,
            KEY_0 = 0x30,
            KEY_1 = 0x31,
            KEY_2 = 0x32,
            KEY_3 = 0x33,
            KEY_4 = 0x34,
            KEY_5 = 0x35,
            KEY_6 = 0x36,
            KEY_7 = 0x37,
            KEY_8 = 0x38,
            KEY_9 = 0x39,
            KEY_UNDEF5 = 0x3A,
            KEY_UNDEF5A = 0x3B,
            KEY_UNDEF5B = 0x3C,
            KEY_UNDEF5C = 0x3D,
            KEY_UNDEF5D = 0x3E,
            KEY_UNDEF5E = 0x3F,
            KEY_UNDEF5F = 0x40,
            KEY_A = 0x41,
            KEY_B = 0x42,
            KEY_C = 0x43,
            KEY_D = 0x44,
            KEY_E = 0x45,
            KEY_F = 0x46,
            KEY_G = 0x47,
            KEY_H = 0x48,
            KEY_I = 0x49,
            KEY_J = 0x4A,
            KEY_K = 0x4B,
            KEY_L = 0x4C,
            KEY_M = 0x4D,
            KEY_N = 0x4E,
            KEY_O = 0x4F,
            KEY_P = 0x50,
            KEY_Q = 0x51,
            KEY_R = 0x52,
            KEY_S = 0x53,
            KEY_T = 0x54,
            KEY_U = 0x55,
            KEY_V = 0x56,
            KEY_W = 0x57,
            KEY_X = 0x58,
            KEY_Y = 0x59,
            KEY_Z = 0x5A,
            KEY_LWIN = 0x5B,
            KEY_RWIN = 0x5C,
            KEY_APPS = 0x5D,
            KEY_UNDEF6 = 0x5E,
            KEY_SLEEP = 0x5F,
            KEY_NUMPAD0 = 0x60,
            KEY_NUMPAD1 = 0x61,
            KEY_NUMPAD2 = 0x62,
            KEY_NUMPAD3 = 0x63,
            KEY_NUMPAD4 = 0x64,
            KEY_NUMPAD5 = 0x65,
            KEY_NUMPAD6 = 0x66,
            KEY_NUMPAD7 = 0x67,
            KEY_NUMPAD8 = 0x68,
            KEY_NUMPAD9 = 0x69,
            KEY_MULTIPLY = 0x6A,
            KEY_ADD = 0x6B,
            KEY_SEPARATOR = 0x6C,
            KEY_SUBTRACT = 0x6D,
            KEY_DECIMAL = 0x6E,
            KEY_DIVIDE = 0x6F,
            KEY_F1 = 0x70,
            KEY_F2 = 0x71,
            KEY_F3 = 0x72,
            KEY_F4 = 0x73,
            KEY_F5 = 0x74,
            KEY_F6 = 0x75,
            KEY_F7 = 0x76,
            KEY_F8 = 0x77,
            KEY_F9 = 0x78,
            KEY_F10 = 0x79,
            KEY_F11 = 0x7A,
            KEY_F12 = 0x7B,
            KEY_F13 = 0x7C,
            KEY_F14 = 0x7D,
            KEY_F15 = 0x7E,
            KEY_F16 = 0x7F,
            KEY_F17 = 0x80,
            KEY_F18 = 0x81,
            KEY_F19 = 0x82,
            KEY_F20 = 0x83,
            KEY_F21 = 0x84,
            KEY_F22 = 0x85,
            KEY_F23 = 0x86,
            KEY_F24 = 0x87,
            KEY_UNDEF7 = 0x88,
            KEY_UNDEF7B = 0x89,
            KEY_UNDEF7C = 0x8A,
            KEY_UNDEF7D = 0x8B,
            KEY_UNDEF7E = 0x8C,
            KEY_UNDEF7F = 0x8D,
            KEY_UNDEF7G = 0x8E,
            KEY_UNDEF7H = 0x8F,
            KEY_NUMLOCK = 0x90,
            KEY_SCROLL = 0x91,
            KEY_UNDEF8 = 0x92,
            KEY_UNDEF9 = 0x93,
            KEY_UNDEF10 = 0x94,
            KEY_UNDEF11 = 0x95,
            KEY_UNDEF12 = 0x96,
            KEY_UNDEF13 = 0x97,
            KEY_UNDEF13A = 0x98,
            KEY_UNDEF13B = 0x99,
            KEY_UNDEF13C = 0x9A,
            KEY_UNDEF13D = 0x9B,
            KEY_UNDEF13E = 0x9C,
            KEY_UNDEF13F = 0x9D,
            KEY_UNDEF13G = 0x9E,
            KEY_UNDEF13H = 0x9F,
            KEY_LSHIFT = 0xA0,
            KEY_RSHIFT = 0xA1,
            KEY_LCONTROL = 0xA2,
            KEY_RCONTROL = 0xA3,
            KEY_LALT = 0xA4,
            KEY_RALT = 0xA5,
            KEY_BROWSER_BACK = 0xA6,
            KEY_BROWSER_FORWARD = 0xA7,
            KEY_BROWSER_REFRESH = 0xA8,
            KEY_BROWSER_STOP = 0xA9,
            KEY_BROWSER_SEARCH = 0xAA,
            KEY_BROWSER_FAVORITES = 0xAB,
            KEY_BROWSER_HOME = 0xAC,
            KEY_VOLUME_MUTE = 0xAD,
            KEY_VOLUME_DOWN = 0xAE,
            KEY_VOLUME_UP = 0xAF,
            KEY_MEDIA_NEXT_TRACK = 0xB0,
            KEY_MEDIA_PREV_TRACK = 0xB1,
            KEY_MEDIA_STOP = 0xB2,
            KEY_MEDIA_PLAY_PAUSE = 0xB3,
            KEY_LAUNCH_MAIL = 0xB4,
            KEY_LAUNCH_MEDIA_SELECT = 0xB5,
            KEY_LAUNCH_APP1 = 0xB6,
            KEY_LAUNCH_APP2 = 0xB7,
            KEY_UNDEF14 = 0xB8,
            KEY_UNDEF14b = 0xB9,
            KEY_OEM_1 = 0xBA,
            KEY_OEM_PLUS = 0xBB,
            KEY_OEM_COMMA = 0xBC,
            KEY_OEM_MINUS = 0xBD,
            KEY_OEM_PERIOD = 0xBE,
            KEY_OEM_2 = 0xBF,
            KEY_OEM_3 = 0xC0,
            KEY_RESERVED0 = 0xC1,
            KEY_RESERVED0A = 0xC2,
            KEY_RESERVED0B = 0xC3,
            KEY_RESERVED0C = 0xC4,
            KEY_RESERVED0D = 0xC5,
            KEY_RESERVED0E = 0xC6,
            KEY_RESERVED0F = 0xC7,
            KEY_RESERVED0G = 0xC8,
            KEY_RESERVED0H = 0xC9,
            KEY_RESERVED0I = 0xCA,
            KEY_RESERVED0J = 0xCB,
            KEY_RESERVED0K = 0xCC,
            KEY_RESERVED0L = 0xCD,
            KEY_RESERVED0M = 0xCE,
            KEY_RESERVED0N = 0xCF,
            KEY_RESERVED0O = 0xD0,
            KEY_RESERVED0P = 0xD1,
            KEY_RESERVED0Q = 0xD2,
            KEY_RESERVED0R = 0xD3,
            KEY_RESERVED0S = 0xD4,
            KEY_RESERVED0T = 0xD5,
            KEY_RESERVED0U = 0xD6,
            KEY_RESERVED0V = 0xD7,
            KEY_RESERVED0W = 0xD8,
            KEY_RESERVED0X = 0xD9,
            KEY_RESERVED0Y = 0xDA,
            KEY_OEM_4 = 0xDB,
            KEY_OEM_5 = 0xDC,
            KEY_OEM_6 = 0xDD,
            KEY_OEM_7 = 0xDE,
            KEY_OEM_8 = 0xDF,
            KEY_RESERVED1 = 0xE0,
            KET_OEM_9 = 0xE1,
            KEY_OEM_102 = 0xE2,
            KEY_LAST = 0xFF,
        };

        static cstring sNames[] = {
            "START", "LBUTTON", "RBUTTON", "CANCEL", "MBUTTON", "XBUTTON1", "XBUTTON2", "UNDEF0", "BACK", "TAB", "UNDEF1", "UNDEF1B", "CLEAR", "RETURN", "UNDEF2", "UNDEF2B", "SHIFT", "CONTROL", "MENU", "PAUSE", "CAPITAL", "KANA", "UNDEF3", "JUNJA", "FINAL", "HANJA", "UNDEF4", "ESCAPE", "CONVERT", "NONCONVERT", "ACCEPT", "MODECHANGE", "SPACE", "PRIOR", "NEXT", "END", "HOME", "LEFT", "UP", "RIGHT", "DOWN", "SELECT", "PRINT", "EXECUTE_", "SNAPSHOT", "INSERT", "DELETE", "HELP", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "UNDEF5", "UNDEF5A", "UNDEF5B", "UNDEF5C", "UNDEF5D", "UNDEF5E", "UNDEF5F", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "LWIN", "RWIN", "APPS", "UNDEF6", "SLEEP", "NUMPAD0", "NUMPAD1", "NUMPAD2", "NUMPAD3", "NUMPAD4", "NUMPAD5", "NUMPAD6", "NUMPAD7", "NUMPAD8", "NUMPAD9", "MULTIPLY", "ADD", "SEPARATOR", "SUBTRACT", "DECIMAL", "DIVIDE", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "F13", "F14", "F15", "F16", "F17", "F18", "F19", "F20", "F21", "F22", "F23", "F24", "UNDEF7", "UNDEF7B", "UNDEF7C", "UNDEF7D", "UNDEF7E", "UNDEF7F", "UNDEF7G", "UNDEF7H", "NUMLOCK", "SCROLL", "UNDEF8", "UNDEF9", "UNDEF10", "UNDEF11", "UNDEF12", "UNDEF13", "UNDEF13A", "UNDEF13B", "UNDEF13C", "UNDEF13D", "UNDEF13E", "UNDEF13F", "UNDEF13G", "UNDEF13H", "LSHIFT", "RSHIFT", "LCONTROL", "RCONTROL", "LALT", "RALT", "BROWSER_BACK", "BROWSER_FORWARD", "BROWSER_REFRESH", "BROWSER_STOP", "BROWSER_SEARCH", "BROWSER_FAVORITES", "BROWSER_HOME", "VOLUME_MUTE", "VOLUME_DOWN", "VOLUME_UP", "MEDIA_NEXT_TRACK", "MEDIA_PREV_TRACK", "MEDIA_STOP", "MEDIA_PLAY_PAUSE", "LAUNCH_MAIL", "LAUNCH_MEDIA_SELECT", "LAUNCH_APP1", "LAUNCH_APP2", "UNDEF14", "UNDEF14b", "OEM_1", "OEM_PLUS", "OEM_COMMA", "OEM_MINUS", "OEM_PERIOD", "OEM_2", "OEM_3", "RESERVED0", "RESERVED0A", "RESERVED0B", "RESERVED0C", "RESERVED0D", "RESERVED0E", "RESERVED0F", "RESERVED0G", "RESERVED0H", "RESERVED0I", "RESERVED0J", "RESERVED0K", "RESERVED0L", "RESERVED0M", "RESERVED0N", "RESERVED0O", "RESERVED0P", "RESERVED0Q", "RESERVED0R", "RESERVED0S", "RESERVED0T", "RESERVED0U", "RESERVED0V", "RESERVED0W", "RESERVED0X", "RESERVED0Y", "OEM_4", "OEM_5", "OEM_6", "OEM_7", "OEM_8", "RESERVED1", "KET_OEM_9", "OEM_102", };

        static const char* sDescriptions[] = {
            "", "Left mouse button", "Right mouse button", "Control-break processing", "Middle mouse button (three-button mouse)", "X1 mouse button", "X2 mouse button", "Undefined", "BACKSPACE key", "TAB key", "Reserved", "Reserved", "CLEAR key", "ENTER key", "Undefined", "Undefined", "SHIFT key", "CTRL key", "ALT key", "PAUSE key", "CAPS LOCK key", "IME Kana mode", "Undefined", "IME Junja mode", "IME final mode", "IME Hanja mode", "Undefined", "ESC key", "IME convert", "IME nonconvert", "IME accept", "IME mode change request", "SPACEBAR", "PAGE UP key", "PAGE DOWN key", "END key", "HOME key", "LEFT ARROW key", "UP ARROW key", "RIGHT ARROW key", "DOWN ARROW key", "SELECT key", "PRINT key", "EXECUTE key", "PRINT SCREEN key", "INS key", "DEL key", "HELP key", "0 key", "1 key", "2 key", "3 key", "4 key", "5 key", "6 key", "7 key", "8 key", "9 key", "undefined", "undefined", "undefined", "undefined", "undefined", "undefined", "undefined", "A key", "B key", "C key", "D key", "E key", "F key", "G key", "H key", "I key", "J key", "K key", "L key", "M key", "N key", "O key", "P key", "Q key", "R key", "S key", "T key", "U key", "V key", "W key", "X key", "Y key", "Z key", "Left Windows key (Natural keyboard)", "Right Windows key (Natural keyboard)", "Applications key (Natural keyboard)", "Reserved", "Computer Sleep key", "Numeric keypad 0 key", "Numeric keypad 1 key", "Numeric keypad 2 key", "Numeric keypad 3 key", "Numeric keypad 4 key", "Numeric keypad 5 key", "Numeric keypad 6 key", "Numeric keypad 7 key", "Numeric keypad 8 key", "Numeric keypad 9 key", "Multiply key", "Add key", "Separator key", "Subtract key", "Decimal key", "Divide key", "F1 key", "F2 key", "F3 key", "F4 key", "F5 key", "F6 key", "F7 key", "F8 key", "F9 key", "F10 key", "F11 key", "F12 key", "F13 key", "F14 key", "F15 key", "F16 key", "F17 key", "F18 key", "F19 key", "F20 key", "F21 key", "F22 key", "F23 key", "F24 key", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "NUM LOCK key", "SCROLL LOCK key", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Unassigned", "Left SHIFT key", "Right SHIFT key", "Left CONTROL key", "Right CONTROL key", "Left ALT key", "Right ALT key", "Browser Back key", "Browser Forward key", "Browser Refresh key", "Browser Stop key", "Browser Search key", "Browser Favorites key", "Browser Start and Home key", "Volume Mute key", "Volume Down key", "Volume Up key", "Next Track key", "Previous Track key", "Stop Media key", "Play/Pause Media key", "Start Mail key", "Select Media key", "Start Application 1 key", "Start Application 2 key", "Reserved", "Reserved", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ';:' key KEY_OEM_PLUS", "For any country/region, the '+' key", "For any country/region, the ',' key", "For any country/region, the '-' key", "For any country/region, the '.' key", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '/?' key", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '`~' key", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Reserved", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the '[{' key", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the \\| key", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the ']}' key", "Used for miscellaneous characters; it can vary by keyboard. For the US standard keyboard, the 'single-quote/double-quote' key", "Used for miscellaneous characters; it can vary by keyboard.", "Reserved", "OEM specific", "Either the angle bracket key or the backslash key on the RT 102-key keyboard", };

        namespace MouseButtons {
            enum Buttons {
                Left    = 0,
                Center  = 1,
                Right   = 2,
                MaxBtns = 3
            };

            // Buttons status sequence:
            // up -> clicked -> down -> unclicked -> up
            enum ButtonStatusShifts {
                Clicked    = 0,
                Unclicked  = 1,
                Down       = 2,
                Up         = 3
            };
        }
        /////////////////////////////////////////////////////////////////////////////////
        struct Keyboard {
            bool KeyDown[KEY_LAST];
            bool KeyUp[KEY_LAST];
            bool LastKeyDown[KEY_LAST];

            bool IsKeyDown( Keys key/*, u32 handle*/ ) {
                //if ( handle != m_focusHandle )
                //return false;
                return KeyDown[key] == true;
            }

            bool IsKeyPressed( Keys key/*, u32 handle*/ ) {
                //if ( handle != m_focusHandle )
                //return false;

                if ( LastKeyDown[key] == false && KeyDown[key] == false ) {
                    LastKeyDown[key] = true;
                    return true;
                }

                return false;
            }
        };

        struct Mouse {
            int32 lastX, prevX;
            int32 lastY, prevY;
            int32 Z;

            bool  buttonDown[MouseButtons::MaxBtns];
            bool  buttonUp[MouseButtons::MaxBtns];
            bool  buttonLastDown[MouseButtons::MaxBtns];

	        void UpdatePosition( int32 x, int32 y ) {
	    	    prevX = lastX;
		        prevY = lastY;
		        lastX = x;
		        lastY = y;
	        }

            bool IsButtonDown( MouseButtons::Buttons button ) {
                return buttonDown[button] == true;
            }

            bool IsButtonPressed( MouseButtons::Buttons button ) {
                if (buttonLastDown[button] == false && buttonDown[button] == false) {
                    buttonLastDown[button] = true;
                    return true;
                }

                return false;
            }

            int32 OffsetX() { return lastX - prevX; }
            int32 OffsetY() { return lastY - prevY; }
        };

        struct InputComponent : public Component {
            Keyboard            keys;
            Mouse               mouse;
        };

        struct InputSystem : public UpdateSystem {

            InputSystem( uint32 order );
            ~InputSystem() {}

            InputComponent _input;

            void SetKeyDown( uint32 virtualKey ) {
                _input.keys.KeyDown[virtualKey] = true;
            }

            void SetKeyUp( uint32 virtualKey ) {
                if ( _input.keys.KeyDown[virtualKey] )
                     _input.keys.KeyUp[virtualKey] = true;

                _input.keys.KeyDown[virtualKey] = false;
                _input.keys.LastKeyDown[virtualKey] = false;
            }

            void SetMouseDown( uint32 button ) {
                _input.mouse.buttonDown[button] = true;
            }

            void SetMouseUp( uint32 button ) {
                if (_input.mouse.buttonDown[button])
                    _input.mouse.buttonUp[button] = true;

                _input.mouse.buttonDown[button] = false;
                _input.mouse.buttonLastDown[button] = false;
            }

            void                Update() override;
        };


        /* What we really want is a mapping of every raw key on the keyboard.
        To support international keyboards, we use the range 0xA1 - 0xFF
        as international virtual keycodes.  We'll follow in the footsteps of X11...
        The names of the keys
        */
        /*
        enum Keys {
        // The keyboard syms have been cleverly chosen to map to ASCII
        TURGID_KEY_UNKNOWN = 0,
        TURGID_KEY_FIRST = 0,
        TURGID_KEY_BACKSPACE = 8,
        TURGID_KEY_TAB = 9,
        TURGID_KEY_CLEAR = 12,
        TURGID_KEY_RETURN = 13,
        TURGID_KEY_PAUSE = 19,
        TURGID_KEY_ESCAPE = 27,
        TURGID_KEY_SPACE = 32,
        TURGID_KEY_EXCLAIM = 33,
        TURGID_KEY_QUOTEDBL = 34,
        TURGID_KEY_HASH = 35,
        TURGID_KEY_DOLLAR = 36,
        TURGID_KEY_AMPERSAND = 38,
        TURGID_KEY_QUOTE = 39,
        TURGID_KEY_LEFTPAREN = 40,
        TURGID_KEY_RIGHTPAREN = 41,
        TURGID_KEY_ASTERISK = 42,
        TURGID_KEY_PLUS = 43,
        TURGID_KEY_COMMA = 44,
        TURGID_KEY_MINUS = 45,
        TURGID_KEY_PERIOD = 46,
        TURGID_KEY_SLASH = 47,
        TURGID_KEY_0 = 48,
        TURGID_KEY_1 = 49,
        TURGID_KEY_2 = 50,
        TURGID_KEY_3 = 51,
        TURGID_KEY_4 = 52,
        TURGID_KEY_5 = 53,
        TURGID_KEY_6 = 54,
        TURGID_KEY_7 = 55,
        TURGID_KEY_8 = 56,
        TURGID_KEY_9 = 57,
        TURGID_KEY_COLON = 58,
        TURGID_KEY_SEMICOLON = 59,
        TURGID_KEY_LESS = 60,
        TURGID_KEY_EQUALS = 61,
        TURGID_KEY_GREATER = 62,
        TURGID_KEY_QUESTION = 63,
        TURGID_KEY_AT = 64,

        uppercase letters

        TURGID_KEY_A = 65,
        TURGID_KEY_B = 66,
        TURGID_KEY_C = 67,
        TURGID_KEY_D = 68,
        TURGID_KEY_E = 69,
        TURGID_KEY_F = 70,
        TURGID_KEY_G = 71,
        TURGID_KEY_H = 72,
        TURGID_KEY_I = 73,
        TURGID_KEY_J = 74,
        TURGID_KEY_K = 75,
        TURGID_KEY_L = 76,
        TURGID_KEY_M = 77,
        TURGID_KEY_N = 78,
        TURGID_KEY_O = 79,
        TURGID_KEY_P = 80,
        TURGID_KEY_Q = 81,
        TURGID_KEY_R = 82,
        TURGID_KEY_S = 83,
        TURGID_KEY_T = 84,
        TURGID_KEY_U = 85,
        TURGID_KEY_V = 86,
        TURGID_KEY_W = 87,
        TURGID_KEY_X = 88,
        TURGID_KEY_Y = 89,
        TURGID_KEY_Z = 90,
        TURGID_KEY_LEFTBRACKET = 91,
        TURGID_KEY_BACKSLASH = 92,
        TURGID_KEY_RIGHTBRACKET = 93,
        //TURGID_KEY_CARET			= 94,
        TURGID_KEY_UNDERSCORE = 95,
        TURGID_KEY_BACKQUOTE = 96,
        TURGID_KEY_DELETE = 127,
        // End of ASCII mapped keysyms

        // International keyboard syms
        //TURGID_KEY_WORLD_0		= 160,		// 0xA0
        /*TURGID_KEY_WORLD_1		= 161,
        TURGID_KEY_WORLD_2		= 162,
        TURGID_KEY_WORLD_3		= 163,
        TURGID_KEY_WORLD_4		= 164,
        TURGID_KEY_WORLD_5		= 165,
        TURGID_KEY_WORLD_6		= 166,
        TURGID_KEY_WORLD_7		= 167,
        TURGID_KEY_WORLD_8		= 168,
        TURGID_KEY_WORLD_9		= 169,
        TURGID_KEY_WORLD_10		= 170,
        TURGID_KEY_WORLD_11		= 171,
        TURGID_KEY_WORLD_12		= 172,
        TURGID_KEY_WORLD_13		= 173,
        TURGID_KEY_WORLD_14		= 174,
        TURGID_KEY_WORLD_15		= 175,
        TURGID_KEY_WORLD_16		= 176,
        TURGID_KEY_WORLD_17		= 177,
        TURGID_KEY_WORLD_18		= 178,
        TURGID_KEY_WORLD_19		= 179,
        TURGID_KEY_WORLD_20		= 180,
        TURGID_KEY_WORLD_21		= 181,
        TURGID_KEY_WORLD_22		= 182,
        TURGID_KEY_WORLD_23		= 183,
        TURGID_KEY_WORLD_24		= 184,
        TURGID_KEY_WORLD_25		= 185,
        TURGID_KEY_WORLD_26		= 186,
        TURGID_KEY_WORLD_27		= 187,
        TURGID_KEY_WORLD_28		= 188,
        TURGID_KEY_WORLD_29		= 189,
        TURGID_KEY_WORLD_30		= 190,
        TURGID_KEY_WORLD_31		= 191,
        TURGID_KEY_WORLD_32		= 192,
        TURGID_KEY_WORLD_33		= 193,
        TURGID_KEY_WORLD_34		= 194,
        TURGID_KEY_WORLD_35		= 195,
        TURGID_KEY_WORLD_36		= 196,
        TURGID_KEY_WORLD_37		= 197,
        TURGID_KEY_WORLD_38		= 198,
        TURGID_KEY_WORLD_39		= 199,
        TURGID_KEY_WORLD_40		= 200,
        TURGID_KEY_WORLD_41		= 201,
        TURGID_KEY_WORLD_42		= 202,
        TURGID_KEY_WORLD_43		= 203,
        TURGID_KEY_WORLD_44		= 204,
        TURGID_KEY_WORLD_45		= 205,
        TURGID_KEY_WORLD_46		= 206,
        TURGID_KEY_WORLD_47		= 207,
        TURGID_KEY_WORLD_48		= 208,
        TURGID_KEY_WORLD_49		= 209,
        TURGID_KEY_WORLD_50		= 210,
        TURGID_KEY_WORLD_51		= 211,
        TURGID_KEY_WORLD_52		= 212,
        TURGID_KEY_WORLD_53		= 213,
        TURGID_KEY_WORLD_54		= 214,
        TURGID_KEY_WORLD_55		= 215,
        TURGID_KEY_WORLD_56		= 216,
        TURGID_KEY_WORLD_57		= 217,
        TURGID_KEY_WORLD_58		= 218,
        TURGID_KEY_WORLD_59		= 219,
        TURGID_KEY_WORLD_60		= 220,
        TURGID_KEY_WORLD_61		= 221,
        TURGID_KEY_WORLD_62		= 222,
        TURGID_KEY_WORLD_63		= 223,
        TURGID_KEY_WORLD_64		= 224,
        TURGID_KEY_WORLD_65		= 225,
        TURGID_KEY_WORLD_66		= 226,
        TURGID_KEY_WORLD_67		= 227,
        TURGID_KEY_WORLD_68		= 228,
        TURGID_KEY_WORLD_69		= 229,
        TURGID_KEY_WORLD_70		= 230,
        TURGID_KEY_WORLD_71		= 231,
        TURGID_KEY_WORLD_72		= 232,
        TURGID_KEY_WORLD_73		= 233,
        TURGID_KEY_WORLD_74		= 234,
        TURGID_KEY_WORLD_75		= 235,
        TURGID_KEY_WORLD_76		= 236,
        TURGID_KEY_WORLD_77		= 237,
        TURGID_KEY_WORLD_78		= 238,
        TURGID_KEY_WORLD_79		= 239,
        TURGID_KEY_WORLD_80		= 240,
        TURGID_KEY_WORLD_81		= 241,
        TURGID_KEY_WORLD_82		= 242,
        TURGID_KEY_WORLD_83		= 243,
        TURGID_KEY_WORLD_84		= 244,
        TURGID_KEY_WORLD_85		= 245,
        TURGID_KEY_WORLD_86		= 246,
        TURGID_KEY_WORLD_87		= 247,
        TURGID_KEY_WORLD_88		= 248,
        TURGID_KEY_WORLD_89		= 249,
        TURGID_KEY_WORLD_90		= 250,
        TURGID_KEY_WORLD_91		= 251,
        TURGID_KEY_WORLD_92		= 252,
        TURGID_KEY_WORLD_93		= 253,
        TURGID_KEY_WORLD_94		= 254,
        TURGID_KEY_WORLD_95		= 255,*/		/* 0xFF */
        /*
        // Numeric keypad
        TURGID_KEY_KP0 = 256,
        TURGID_KEY_KP1 = 257,
        TURGID_KEY_KP2 = 258,
        TURGID_KEY_KP3 = 259,
        TURGID_KEY_KP4 = 260,
        TURGID_KEY_KP5 = 261,
        TURGID_KEY_KP6 = 262,
        TURGID_KEY_KP7 = 263,
        TURGID_KEY_KP8 = 264,
        TURGID_KEY_KP9 = 265,
        TURGID_KEY_KP_PERIOD = 266,
        TURGID_KEY_KP_DIVIDE = 267,
        TURGID_KEY_KP_MULTIPLY = 268,
        TURGID_KEY_KP_MINUS = 269,
        TURGID_KEY_KP_PLUS = 270,
        TURGID_KEY_KP_ENTER = 271,
        TURGID_KEY_KP_EQUALS = 272,

        // Arrows + Home/End pad
        TURGID_KEY_UP = 273,
        TURGID_KEY_DOWN = 274,
        TURGID_KEY_RIGHT = 275,
        TURGID_KEY_LEFT = 276,
        TURGID_KEY_INSERT = 277,
        TURGID_KEY_HOME = 278,
        TURGID_KEY_END = 279,
        TURGID_KEY_PAGEUP = 280,
        TURGID_KEY_PAGEDOWN = 281,

        //Function keys
        TURGID_KEY_F1 = 282,
        TURGID_KEY_F2 = 283,
        TURGID_KEY_F3 = 284,
        TURGID_KEY_F4 = 285,
        TURGID_KEY_F5 = 286,
        TURGID_KEY_F6 = 287,
        TURGID_KEY_F7 = 288,
        TURGID_KEY_F8 = 289,
        TURGID_KEY_F9 = 290,
        TURGID_KEY_F10 = 291,
        TURGID_KEY_F11 = 292,
        TURGID_KEY_F12 = 293,
        /*TURGID_KEY_F13			= 294,
        TURGID_KEY_F14			= 295,
        TURGID_KEY_F15			= 296,*/

        /* Key state modifier keys */
        /* TURGID_KEY_NUMLOCK = 300,
        TURGID_KEY_CAPSLOCK = 301,
        TURGID_KEY_SCROLLOCK = 302,
        TURGID_KEY_RSHIFT = 303,
        TURGID_KEY_LSHIFT = 304,
        TURGID_KEY_RCTRL = 305,
        TURGID_KEY_LCTRL = 306,
        TURGID_KEY_RALT = 307,
        TURGID_KEY_LALT = 308,
        TURGID_KEY_RMETA = 309,
        TURGID_KEY_LMETA = 310,
        TURGID_KEY_LSUPER = 311,		/* Left "Windows" key */
        //TURGID_KEY_RSUPER = 312,		/* Right "Windows" key */
        //TURGID_KEY_MODE = 313,		/* "Alt Gr" key */
        //TURGID_KEY_COMPOSE		= 314,		/* Multi-key compose key */

        /* Miscellaneous function keys */
        /* TURGID_KEY_HELP = 315,
        TURGID_KEY_PRINT = 316,
        TURGID_KEY_SYSREQ = 317,
        TURGID_KEY_BREAK = 318,
        TURGID_KEY_MENU = 319,
        //TURGID_KEY_POWER			= 320,		/* Power Macintosh power key */
        // TURGID_KEY_EURO = 321,		/* Some european keyboards */
        //TURGID_KEY_UNDO			= 322,		/* Atari keyboard has Undo */
        // TURGID_KEY_TILDE = 323,

        /* Add any other keys here */
        /*
        TURGID_KEY_LAST
        };

        // Enumeration of valid key mods (possibly OR'd together)
        enum KeyModifier {
        TURGID_KEYMOD_NONE = 0x0000,
        TURGID_KEYMOD_LSHIFT = 0x0001,
        TURGID_KEYMOD_RSHIFT = 0x0002,
        TURGID_KEYMOD_LCTRL = 0x0040,
        TURGID_KEYMOD_RCTRL = 0x0080,
        TURGID_KEYMOD_LALT = 0x0100,
        TURGID_KEYMOD_RALT = 0x0200,
        TURGID_KEYMOD_LMETA = 0x0400,
        TURGID_KEYMOD_RMETA = 0x0800,
        TURGID_KEYMOD_NUM = 0x1000,
        TURGID_KEYMOD_CAPS = 0x2000,
        TURGID_KEYMOD_MODE = 0x4000,
        TURGID_KEYMOD_RESERVED = 0x8000
        };

        #define TURGID_KEYMOD_CTRL	(TURGID_KEYMOD_LCTRL | TURGID_KEYMOD_RCTRL)
        #define TURGID_KEYMOD_SHIFT	(TURGID_KEYMOD_LSHIFT | TURGID_KEYMOD_RSHIFT)
        #define TURGID_KEYMOD_ALT	(TURGID_KEYMOD_LALT | TURGID_KEYMOD_RALT)
        #define TURGID_KEYMOD_META	(TURGID_KEYMOD_LMETA | TURGID_KEYMOD_RMETA)
        */
    } // namespace input
} // namespace hydra

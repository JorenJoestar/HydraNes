#pragma once

#include "Types.h"
#include "DataStructures.h"
#include "Process.h"
#include "File.h"
#include "Log.h"
#include "Time.h"

#if defined(_WIN64)
#include <Windows.h>
#endif


namespace hydra {

#if defined(_WIN64)
    using DllHandle = HMODULE;
    using WindowHandle = HWND;
    using ProcessHandle = HINSTANCE;

    #define HyWinMain() int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow )
    #define HyMain() int main( int argc, cstring* argv )

    #define ThreadLocal thread_local
    #define ForceInline __forceinline

    #define HY_DISABLE_OPTIMIZATION __pragma(optimize( "", off ))
    #define HY_ENABLE_OPTIMIZATION __pragma(optimize( "", on ))
#else
    #define HyMain() int main( int argc, cstring* argv )
#endif

    #define ArrayLength(array) ( sizeof(array)/sizeof((array)[0]) )

    //////////////////////////////////////////////////////////////////////////
    // error
    // http://bitsquid.blogspot.se/2012/01/sensible-error-handling-part-1.html
    //
    // How to initialize different array per thread ?
    ThreadLocal static Array<cstring>* gErrorContextName;
    ThreadLocal static Array<cstring>* gErrorContextData;

    struct ErrorContext {
        ErrorContext( cstring name, cstring data ) {
            gErrorContextName->push_back( name );
            gErrorContextData->push_back( data );
        }

        ~ErrorContext() {
            gErrorContextName->pop_back();
            gErrorContextData->pop_back();
        }
    };

    /*struct SaveResult {
        enum { NO_ERROR, DISK_FULL, WRITE_ERROR } error;
        unsigned saved_bytes;
    };
    SaveResult save_result( SaveId id );*/

    //////////////////////////////////////////////////////////////////////////
    // Asserts
    #define HY_STRINGIFY_DETAIL(x) #x
    #define HY_STRINGIFY(x) HY_STRINGIFY_DETAIL(x)

    #include <Hydra/Kernel/Assert.h>


    //////////////////////////////////////////////////////////////////////////
    // events
    // http://bitsquid.blogspot.se/2009/12/events.html
    // http://bitsquid.blogspot.se/2011/02/managing-decoupling-part-2-polling.html
    // http://bitsquid.blogspot.se/2011/05/monitoring-your-game.html
    // Systems + update = output + event stream
    // Event stream = memory blob [type + size][data] [...]
    struct EventStream {

        template <typename T>
        void AddEvent( T* event ) {
            uint32* type = (uint32*)( _blob->data + _writeOffset );
            *type = T::Type();

            uint32* size = (uint32*)( _blob->data + _writeOffset + 4 );
            *size = sizeof( T );

            uint8* data = ( _blob->data + _writeOffset + 8 );
            memcpy( data, event, *size );

            _writeOffset += 8 + sizeof( T );
            Assert( _writeOffset < _blob->_size );
        }

        // Retrieve current event type
        uint32 GetEventType() const {
            return *(uint32*)( _blob->data + _readOffset );
        }

        // Get event and proceed the reading
        template <typename T>
        const T& GetEvent() {
            uint32 size = *(uint32*)( _blob->data + _readOffset + 4 );
            T& event = *(T*)( _blob->data + _readOffset + 8 );
            _readOffset += 8 + size;
            return event;
        }

        bool HasEvents() { return _writeOffset > 0; }

        void Rewind() { _readOffset = 0; }
        bool EndOfStream() { return _readOffset >= _writeOffset; }

        void Reset() { 
            _writeOffset = 0;
            _readOffset = 0;
        }

        // Blob is (Type + Size + Data)*
        ArrayView<uint8>*   _blob;
        uint32              _writeOffset = 0;          // totalSize < blob.size
        uint32              _readOffset = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    // Entity
    // http://bitsquid.blogspot.se/2014/08/building-data-oriented-entity-system.html
    // http://bitsquid.blogspot.com/2014/09/building-data-oriented-entity-system.html

    

    //////////////////////////////////////////////////////////////////////////
    // timing
    

    //////////////////////////////////////////////////////////////////////////
    // threading
    // http://bitsquid.blogspot.se/2010/03/task-management-practical-example.html


    //////////////////////////////////////////////////////////////////////////
    // command line


    //////////////////////////////////////////////////////////////////////////
    // String + Hash
    // http://bitsquid.blogspot.se/2010/10/static-hash-values.html


    //////////////////////////////////////////////////////////////////////////
    // Code generation
    void CreateEnum( FileHandle Output, char* tokens, cstring name, bool generateMasks, bool addMax, bool fromJsonArray );

    //////////////////////////////////////////////////////////////////////////
    // Systems
    // http://bitsquid.blogspot.se/2011/01/managing-coupling.html
    // http://bitsquid.blogspot.se/2011/02/managing-decoupling-part-2-polling.html
    // http://bitsquid.blogspot.se/2011/02/some-systems-need-to-manipulate-objects.html
    // http://bitsquid.blogspot.se/2011/09/managing-decoupling-part-4-id-lookup.html
    // http://bitsquid.blogspot.se/2010/02/blob-and-i.html

    struct Component {

    };

    struct System {
        uint32 order = 0;

        System(uint32 order) : order(order) {}
        virtual ~System() {}

        virtual void                    Init() {}
        virtual void                    Terminate() {}

        virtual void                    ExecuteCallbacks() {}

        System( const System& ) = delete;
        System& operator=( const System& ) = delete;

        // Output:
        // Events (blob)
        EventStream                     _eventStream;
        // Callbacks (queued)
    };

    struct UpdateSystem : public System {
        UpdateSystem( uint32 order ) : System(order) {}

        virtual void Update() {}
    };

    //////////////////////////////////////////////////////////////////////////
    // Window

    // TODO: heavy refactor!
    struct WindowSystem;

    template<typename T>
    struct Rect {
        T x;
        T y;
        T width;
        T height;
    };

    struct WindowInit {
        WindowSystem*                   outWindow;
        ProcessHandle                   process;
        cstring                         name;
        cstring                         title;
        Rect<uint16>                    rect;
        ArrayView<uint8>*               blob;
        bool                            fullscreen;
    };

    namespace window {
        namespace callbacks {
            enum Types {
                Type_RequestExit, Type_CharWritten, Type_KeyModified, Type_WindowResize, Type_ChangeFocus, Type_ChangeDevice
            };

            //enum Mask {
            //    Type_RequestExit_mask = 1 << 0, Type_CharWritten_mask = 1 << 1, Type_KeyModified_mask = 1 << 2,
            //    Type_WindowResize_mask = 1 << 3, Type_ChangeFocus_Mask = 1 << 4, Type_ChangeDevice_Mask = 1 << 5
            //};

            static cstring              ToString( Types e );

            
            struct KeyModifiedData {
                uint8                   virtualKey;
                uint8                   keyDown;
            };
            
            struct CharModifiedData {
                char                    ansiChar;
            };

            struct WindowResizeData {
                uint16                  width;
                uint16                  height;
            };
            
            struct ChangeFocusData {
                uint8                   inFocus;
            };

            struct ChangeDeviceData {
                uint8                   deviceRemoved;
                uint8                   deviceAdded;
            };

            using RequestExit = void (*)( void* userData );
            using KeyModified = void (*)( void* userData, const KeyModifiedData& );
            using CharModified = void (*)(void* userData, const CharModifiedData& );
            using WindowResized = void (*)( void* userData, const WindowResizeData& );
            using ChangeFocus = void (*)( void* userData, const ChangeFocusData& );
            using ChangeDevice = void( *)( void* userData, const ChangeDeviceData& );
        } // namespace callbacks

        namespace events {
            enum Types {
                Event_KeyInput, Event_MouseMove, Event_MouseClick, Event_MouseWheel, Event_Resize, Event_Focus, Event_Device
            };

            struct Resize {
                uint16                  width;
                uint16                  height;

                static uint32           Type() { return Event_Resize; }
            };

            struct KeyInput {
                uint8                   virtualKey;
                bool                    keyDown;

                static uint32           Type() { return Event_KeyInput; }
            };

            struct MouseMove {
                uint16                  x;
                uint16                  y;

                static uint32           Type() { return Event_MouseMove; }
            };

            struct MouseClick {
                uint16                  x;
                uint16                  y;
                uint8                   button;
                uint8                   state;

                static uint32           Type() { return Event_MouseClick; }
            };

            struct MouseWheel {

                int16                   delta;

                static uint32           Type() { return Event_MouseWheel; }
            };

            struct ChangeFocus {
                uint8                   inFocus;

                static uint32           Type() { return Event_Focus; }
            };

            struct DeviceChange {
                uint8                   deviceRemoved;
                uint8                   deviceAdded;

                static uint32           Type() { return Event_Device; }
            };

        } // namespace Events
    } // namespace window

    struct WindowSystem : public UpdateSystem {

        WindowSystem( WindowInit& init, uint32 order );
        ~WindowSystem() {}

        void                            Update() override;
        void                            ExecuteCallbacks() override;

        void                            AddRequestExitCallback( window::callbacks::RequestExit callback, void* userData );
        void                            AddKeyCallback( window::callbacks::KeyModified callback, void* userData );
        void                            AddCharCallback( window::callbacks::CharModified callback, void* userData );
        void                            AddResizeCallback( window::callbacks::WindowResized callback, void* userData );
        void                            AddFocusCallback( window::callbacks::ChangeFocus callback, void* userData );
        void                            AddDeviceCallback( window::callbacks::ChangeDevice callback, void* userData );
        
        ProcessHandle                   processHandle;
        WindowHandle                    handle;
        cstring                         name;
        Rect<uint16>                    windowArea;
        uint16                          requestedWidth;
        uint16                          requestedHeight;
        uint16                          frameBufferWidth;
        uint16                          frameBufferHeight;
        bool                            isResizing;

        enum Flags {
            Flags_fullscreen = 0,
            Flags_requestExit,
            Flags_inFocus,
            Flags_resize,
            Flags_focus
        };

        Flag8                           flags;
        
        // Callbacks
        Array<window::callbacks::RequestExit>   exitCallbacks;
        Array<void*>                            exitCallbacksUserData;
        
        Array<window::callbacks::KeyModified>   keyCallbacks;
        window::callbacks::KeyModifiedData      keyCallbackData;
        Array<void*>                            keyCallbackUserData;

        Array<window::callbacks::CharModified>  charCallbacks;
        window::callbacks::CharModifiedData     charCallbackData;
        Array<void*>                            charCallbackUserData;

        Array<window::callbacks::WindowResized> resizeCallbacks;
        window::callbacks::WindowResizeData     resizeData;
        Array<void*>                            resizeCallbackUserData;

        Array<window::callbacks::ChangeFocus>   focusCallbacks;
        window::callbacks::ChangeFocusData      focusData;
        Array<void*>                            focusCallbackUserData;

        Array<window::callbacks::ChangeDevice>  deviceCallbacks;
        window::callbacks::ChangeDeviceData     deviceData;
        Array<void*>                            deviceCallbackUserData;

        Flag                                    callbacksActivationMask;
    };

    //////////////////////////////////////////////////////////////////////////
    // Memory

    inline bool IsAligned( voidp address, uint64 alignment ) {
        return ( ( (uintptr)address & ( alignment - 1 ) ) == 0 );
    }

    inline msize GetAlignmentOffset( voidp address, uint64 alignment ) {
        msize offset = 0;

        msize pointer = (msize)address;
        msize alignmentMask = alignment - 1;
        if ( pointer & alignmentMask ) {
            offset = alignment - ( pointer & alignmentMask );
        }

        return offset;
    }

    inline msize GetEffectiveSize( voidp address, msize size, uint64 alignment ) {

        msize offset = GetAlignmentOffset( address, size );
        return size + offset;
    }

    inline voidp GetAlignedPointer( voidp address, uint64 alignment ) {
        const uintptr p = (uintptr)address;
        const uintptr alignmentmask = ~( alignment - 1 );
        const uintptr alignedpointer = ( p + alignment - 1 ) & alignmentmask;

        return (voidp)alignedpointer;
    }

    //////////////////////////////////////////////////////////////////////////
    // Exceptions

    // Declare an object of this type in a scope in order to enable a
    // specified set of floating-point exceptions temporarily. The old
    // exception state will be reset at the end.
    // This class can be nested.
    // From https://randomascii.wordpress.com/2012/04/21/exceptional-floating-point/

    //#include <xmmintrin.h>

    //namespace FloatingPointExceptions {
    //    // Mask to select exceptions to be trapped
    //    enum Masks {
    //        MASK_NONE = 0x0,
    //        MASK_INVALID = _MM_MASK_INVALID,
    //        MASK_DENORM = _MM_MASK_DENORM,
    //        MASK_DIV_ZERO = _MM_MASK_DIV_ZERO,
    //        MASK_OVERFLOW = _MM_MASK_OVERFLOW,
    //        MASK_UNDERFLOW = _MM_MASK_UNDERFLOW,
    //        MASK_INEXACT = _MM_MASK_INEXACT,
    //        MASK_ALL = _MM_MASK_MASK,
    //    };

    //    // Type of exceptions trapped
    //    enum Exceptions {
    //        EXCEPT_NONE = 0x0,
    //        EXCEPT_INVALID = _MM_EXCEPT_INVALID,
    //        EXCEPT_DENORM = _MM_EXCEPT_DENORM,
    //        EXCEPT_DIV_ZERO = _MM_EXCEPT_DIV_ZERO,
    //        EXCEPT_OVERFLOW = _MM_EXCEPT_OVERFLOW,
    //        EXCEPT_UNDERFLOW = _MM_EXCEPT_UNDERFLOW,
    //        EXCEPT_INEXACT = _MM_EXCEPT_INEXACT,
    //        EXCEPT_ALL = _MM_EXCEPT_MASK
    //    };

    //    class ScopedEnabler {
    //    public:
    //        // Save the old register content to restore when the scope is over.
    //        uint32                  savedRegister;

    //        ScopedEnabler( uint32_t newMask = MASK_DIV_ZERO | MASK_INVALID | MASK_OVERFLOW ) {
    //            savedRegister = _mm_getcsr();

    //            // Clear current exceptions if present.
    //            _MM_SET_EXCEPTION_STATE( EXCEPT_NONE );
    //            // Set the current exceptions mask
    //            _MM_SET_EXCEPTION_MASK( MASK_ALL &~( newMask ) );
    //        }

    //        ~ScopedEnabler() {
    //            // Restore previous state
    //            _mm_setcsr( savedRegister );
    //        }

    //        // Check if an exception is present
    //        bool HasTrappedException() { return _MM_GET_EXCEPTION_STATE() != 0; }
    //        // Retrieve current exception
    //        Exceptions GetException() { return (Exceptions)_MM_GET_EXCEPTION_STATE(); }

    //    private:
    //        // For some reasons = delete doesn't work!
    //        ScopedEnabler( const ScopedEnabler& ) {};
    //        ScopedEnabler& operator=( const ScopedEnabler& ) {};
    //    };

    //    class ScopedDisabler {
    //    public:

    //        // Save the old register content to restore when the scope is over.
    //        uint32                  savedRegister;

    //        ScopedDisabler() {
    //            savedRegister = _mm_getcsr();
    //            // Disable exceptions.
    //            _MM_SET_EXCEPTION_MASK( MASK_ALL & ~( MASK_NONE ) );
    //        }

    //        ~ScopedDisabler() {
    //            // Restore previous state
    //            _mm_setcsr( savedRegister );
    //        }

    //    private:
    //        ScopedDisabler( const ScopedDisabler& ) {};
    //        ScopedDisabler& operator=( const ScopedDisabler& ) {};
    //    };
    //}

    //////////////////////////////////////////////////////////////////////////
    // Utility
    template <uint32 count>
    struct WindowMean {
        StaticArray<float, count>           _values;
        float                               _mean;
        uint16                              _addedValues;
        uint16                              _lastValueIndex;
        uint8                               _dirty;

        WindowMean() : _addedValues(0), _lastValueIndex(0), _dirty(true), _mean(0.0f) {}

        void AddValue(float value) {
            if ( _addedValues < count )
                ++_addedValues;

            _values[_lastValueIndex++] = value;
            _lastValueIndex %= count;
            _dirty = true;
        }

        uint32 GetWindowSize() { return count; }

        bool HasEnoughData() {
            return _addedValues >= count;
        }

        float GetMean() {
            if ( HasEnoughData() ) {
                if ( _dirty ) {
                    float _mean = 0.0f;
                    for ( float v : _values )
                        _mean += v;
                    _mean = _mean / count;
                    _dirty = false;
                }
                return _mean;
            }

            return 0.0f;
        }

        float GetStandardDeviation() {
            return 0.0;
        }

        void Clear() {
            _addedValues = 0;
            _lastValueIndex = 0;
            // TODO: clean all values too ?
            _dirty = true;
        }
    };

    //////////////////////////////////////////////////////////////////////////
    // Systems
    
    // Time system
    struct TimeSystem : public UpdateSystem {
        Time                        _frameStart;
        TimeStamp                   _deltaMs;
        int64                       _frameCount;

        TimeSystem( uint32 order );

        void Update() override;
    };

    // Profiler
    struct ProfilerSystem : public UpdateSystem {

        // Array of samples ?

        struct CpuSample {
            Time start, end;
            cstring name;
            uint8 depth;
        };

        struct PerFrameProfiler {
            
        };


        struct Commands {
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

        ProfilerSystem( uint32 order );

        void Update() override;


        void Start();
        void End();

        void PushSample( cstring name );
        void PopSample();


        PerFrameProfiler currentFrameProfiler;
    };
}

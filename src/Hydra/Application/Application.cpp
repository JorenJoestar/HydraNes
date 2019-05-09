#include <Hydra/Application/Application.h>
#include <Hydra/Kernel/Lib.h>
#include <Hydra/Kernel/Input.h>
#include <Hydra/Renderer/Renderer.h>

//////////////////////////////////////////////////////////////////////////
namespace hydra {
namespace application {


void Engine::SortSystemForUpdate() {
    std::sort( _updateSystems.begin(), _updateSystems.end(), []( const System* a, const System* b ) { return a->order < b->order; } );
}

void Engine::InitSystems( gfx::RenderSystemInitContext& ric ) {

    SortSystemForUpdate();

    for ( auto s : _updateSystems ) {
        s->Init();
    }

    for ( auto s : _renderSystems ) {
        s->Init( ric );
    }
}

void Engine::Update() {
    for ( auto s : _updateSystems ) {
        s->Update();
    }
}

void Engine::TerminateSystems() {
    for ( auto s : _updateSystems ) {
        s->Terminate();
    }
/*
    for ( auto s : _renderSystems ) {
        s->Terminate();
    }*/
}

//////////////////////////////////////////////////////////////////////////
InputUpdateSystem::InputUpdateSystem( Init& init, uint32 order ) : UpdateSystem( order ), w( init.w ), i( init.i ) {

}

void InputUpdateSystem::Update() {

    int32 mouseX = i._input.mouse.lastX;
    int32 mouseY = i._input.mouse.lastY;

    if ( w._eventStream.HasEvents() ) {
        EventStream& e = w._eventStream;

        while ( !e.EndOfStream() ) {
            uint32 type = e.GetEventType();
            switch ( type ) {

                case window::events::Event_Focus: {
                    const window::events::ChangeFocus& focusEvent = e.GetEvent<window::events::ChangeFocus>();
                    if ( !focusEvent.inFocus ) {
                        // Reset all keys be sending per key callbacks
                        for ( uint32 j = 0; j < input::KEY_LAST; ++j ) {
                            if ( i._input.keys.KeyDown[j] ) {
                                i._input.keys.KeyDown[j] = false;
                                i._input.keys.LastKeyDown[j] = false;

                                w.callbacksActivationMask.set( window::callbacks::Type_KeyModified );
                                w.keyCallbackData.keyDown = false;
                                w.keyCallbackData.virtualKey = (uint8)j;
                            }
                        }
                    }
                    break;
                }

                case window::events::Event_KeyInput: {
                    const window::events::KeyInput& inputEvent = e.GetEvent<window::events::KeyInput>();
                    if ( inputEvent.keyDown )
                        i.SetKeyDown( inputEvent.virtualKey );
                    else
                        i.SetKeyUp( inputEvent.virtualKey );
                    break;
                }

                case window::events::Event_MouseMove: {
                    const window::events::MouseMove& inputEvent = e.GetEvent<window::events::MouseMove>();
                    mouseX = inputEvent.x;
                    mouseY = inputEvent.y;

                    break;
                }

                case window::events::Event_MouseClick: {
                    const window::events::MouseClick& inputEvent = e.GetEvent<window::events::MouseClick>();
                    if ( inputEvent.state == 0 )
                        i.SetMouseDown( inputEvent.button );
                    else
                        i.SetMouseUp( inputEvent.button );

                    mouseX = inputEvent.x; 
                    mouseY = inputEvent.y;

                    break;
                }

                case window::events::Event_MouseWheel: {
                    const window::events::MouseWheel& inputEvent = e.GetEvent<window::events::MouseWheel>();
                    i._input.mouse.Z = inputEvent.delta;
                    break;
                }

                case window::events::Event_Resize: {
                    const window::events::Resize& inputEvent = e.GetEvent<window::events::Resize>();
                    w.frameBufferWidth = inputEvent.width;
                    w.frameBufferHeight = inputEvent.height;

                    break;
                }

                default:
                    PrintFormat( "Error: invalid message %u. Resetting all events for this system!\n", type );
                    e.Reset();
                    break;
            }
        }
        // Reset all events for this system.
        e.Reset();
    }

    i._input.mouse.UpdatePosition( mouseX, mouseY );
}

//////////////////////////////////////////////////////////////////////////
App::App() : _currentState( nullptr ), _setExit(false) {}

App::~App() {}

StateId App::AddState( State* state ) {
    _states.push_back( state );
    return _states.size() - 1;
}

void App::RemoveState( StateId stateid ) {
    if ( ValidId( stateid ) ) {
        _states.erase( stateid );
    }
}

void App::ChangeState( StateId stateid ) {
    if ( !ValidId( stateid ) )
        return;

    if ( _currentState ) {
        TerminateContext tc{ &_engine };
        _currentState->Terminate( tc );
    }

    _currentState = _states[stateid];
    InitContext ic{ &_engine };
    _currentState->Init( ic );
}

void App::MainLoop()
{
    if ( NoStates() || _currentState == nullptr )
        return;

    bool doMainLoop = true;

    UpdateContext uc{ &_engine };
    RenderContext rc{ &_engine };

    while ( doMainLoop ) {

        doMainLoop = _currentState->Update( uc );
        _currentState->Render( rc );

        if ( _setExit )
            break;
    }

    if ( _currentState ) {
        TerminateContext tc{ &_engine };
        _currentState->Terminate( tc );
    }
}

void App::Resize( uint16 width, uint16 height ) {
    _currentState->Resize( width, height );
}

bool App::ValidId( StateId stateid ) const {
    return stateid < _states.size();
}

bool App::NoStates() const {
    return _states.size() == 0;
}

/*
void SingleExecution( State* state ) {
    state->Init();
    state->Update();
    state->Render();
    state->Terminate();
}*/

} // namespace application
} // namespace hydra


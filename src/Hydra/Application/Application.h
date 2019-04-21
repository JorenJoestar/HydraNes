#pragma once

#include <Hydra/Kernel/Lib.h>

namespace hydra {
    
    struct Engine;
    struct WindowSystem;
    struct UpdateSystem;

    namespace gfx {
        struct RenderSystem;
        struct RenderSystemInitContext;
    }

    namespace input {
        struct InputSystem;
    }

namespace application {

    //////////////////////////////////////////////////////////////////////////
    // Engine
    // Is a set of systems updated
    struct Engine {
        Array<UpdateSystem*>            _updateSystems;
        Array<gfx::RenderSystem*>       _renderSystems;

        template <typename S, typename ... Args>
        S*                              AddUpdate( Args && ... args );

        template <typename S, typename ... Args>
        S*                              AddRender( Args && ... args );

        void                            SortSystemForUpdate();

        void                            InitSystems( gfx::RenderSystemInitContext& ric );
        void                            TerminateSystems();

        void                            Update();
    };

    template <typename S, typename ... Args>
    S* Engine::AddUpdate( Args && ... args ) {
        S* system = new S( std::forward<Args>( args ) ... );
        _updateSystems.push_back( system );
        return system;
    }

    template <typename S, typename ... Args>
    S* Engine::AddRender( Args && ... args ) {
        S* system = new S( std::forward<Args>( args ) ... );
        _renderSystems.push_back( system );
        return system;
    }

    //////////////////////////////////////////////////////////////////////////
    struct InputUpdateSystem : public UpdateSystem {
        WindowSystem&               w;
        input::InputSystem&         i;

        struct Init {
            WindowSystem&           w;
            input::InputSystem&     i;
        };

        InputUpdateSystem( Init& init, uint32 order );

        void Update() override;
    };


    //////////////////////////////////////////////////////////////////////////
    typedef uint32 StateId;

    struct InitContext {
        Engine*                     _engine;
    };

    struct TerminateContext {
        Engine*                     _engine;
    };

    struct UpdateContext {
        Engine*                     _engine;
    };

    struct RenderContext {
        Engine*                     _engine;
    };

    struct State {
        virtual void                Init( InitContext& context ) {}
        virtual void                Terminate( TerminateContext& context ) {}

        virtual bool                Update( UpdateContext& context ) { return false; }
        virtual void                Render( RenderContext& context ) {}

        virtual void                Reload() {}
        virtual void                Resize( uint16 width, uint16 height ) {}

        virtual cstring             Name() { return ""; }
    };

    struct App {

        Engine                      _engine;
        Array<State*>               _states;
        State*                      _currentState;

        bool                        _setExit;

        App();
        ~App();

        // Predicates
        bool                        ValidId( StateId stateid ) const;
        bool                        NoStates() const;

        StateId                     AddState( State* state );
        void                        RemoveState( StateId stateid );

        void                        ChangeState( StateId stateid );

        void                        MainLoop();
        void                        Resize( uint16 width, uint16 height );
    };
    
    //void                        SingleExecution( State* state );
}
}

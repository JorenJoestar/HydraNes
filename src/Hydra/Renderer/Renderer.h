#pragma once

#include <windows.h>
#include <Hydra/Kernel/Lib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Hydra/Renderer/RenderInterfaces.h>
#include <Hydra/Renderer/RenderSystem.h>

namespace hydra {

namespace input {
    struct InputSystem;
} // namespace input

namespace gfx {

struct RenderDatabase;
struct Camera;

struct DrawCommandBuffer {
    
};

struct ComputeCommandBuffer {

};

struct DebugCommandBuffer {

};

struct RenderContext {
    //DrawCommandBuffer& dcb;
    //ComputeCommandBuffer& ccb;
    //DebugCommandBuffer& dbcb;
    void WriteToGpu( VertexBufferHandle& vb, void* sourceData, uint32 size );

    void Set( FrameBufferState& renderTarget );

    void Draw( DrawCall& drawCall );

    Camera* camera;
};


//////////////////////////////////////////////////////////////////////////
// Process the DebugRenderCommands and translates them to RenderCommands.
struct DebugRenderer : public RenderSystem {

    DebugRenderer( uint32 order ) : RenderSystem(order) {}
    ~DebugRenderer() {}

    void Init( RenderSystemInitContext& ctx ) override;
    void Terminate() override;

    void Update() override;
    void Render( RenderContext& rc ) override;

    void AddLine( const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& color );
    void AddTriangle( const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color );

    DrawCall _drawCall;

    static const uint32 kMaxLines = 16000;

    struct DebugVertex {
        glm::vec3 position;
        glm::vec3 color;
    };

    StaticArray<DebugVertex, kMaxLines> _lines;
    
    uint32 _lineCount = 0;
}; // struct DebugRenderer

struct FrameBeginSystem : public UpdateSystem {

}; // struct FrameBeginSystem

struct RenderEngineSystem : public RenderSystem {
    
    RenderDevice        _device;
    WindowSystem*       _w;

    RenderEngineSystem( WindowSystem* w, uint32 order ) : RenderSystem( order ), _w( w ) {
    }

    ~RenderEngineSystem();

    void Init( RenderSystemInitContext& ctx ) override;
    void Terminate() override;
    void Update() override;

    void Resize();
    void Render( RenderContext& rc ) override;
    void Present() override;

    // ...
}; // struct RenderEngineSystem


} // namespace hydra
} // namespace gfx
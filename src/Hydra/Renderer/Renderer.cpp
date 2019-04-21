#include <Hydra/Renderer/Renderer.h>
#include <Hydra/Renderer/Camera.h>

namespace hydra {
namespace gfx {

//////////////////////////////////////////////////////////////////////////
void RenderDevice::InitCommon( WindowSystem* w ) {
    _width = w->frameBufferWidth;
    _height = w->frameBufferHeight;
    _debugOutput = true;
}

//////////////////////////////////////////////////////////////////////////
ShaderStateHandle RenderDatabase::FindShaderState( cstring name ) {
    for ( uint32 i = 0; i < _shaderStates.size(); ++i ) {
        const ShaderState& s = _shaderStates[i];
        if ( strcmp( name, s.name ) == 0 ) {
            return &_shaderStates[i];
        }
    }

    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
RenderEngineSystem::~RenderEngineSystem() {

}

void RenderEngineSystem::Init( RenderSystemInitContext& ctx ) {
    _device.Init( _w );
}

void RenderEngineSystem::Update() {

}

void RenderEngineSystem::Terminate() {

}

void RenderEngineSystem::Render( RenderContext& rc ) {
    _device.Render();
    // Render all debug renderer stuff
}

void RenderEngineSystem::Present() {
    _device.SwapBuffers();
}

//////////////////////////////////////////////////////////////////////////


void DebugRenderer::Init( RenderSystemInitContext& ctx ) {
    PrintFormat( " Here w go!!\n" );

    VertexBufferCreation vbc{ _lines.begin(), kMaxLines * sizeof(DebugVertex), sizeof(DebugVertex), false };
    _drawCall._inputAssembler._vertexBuffer = ctx.rd.CreateVertexBuffer( vbc );

    VertexLayoutElement vls[] = { { 0, 3, VertexDataType::Float, VertexUsage::Position, 0, VertexFrequency::Vertex },
                                  { 0, 3, VertexDataType::Float, VertexUsage::Color, 0, VertexFrequency::Vertex } };
    VertexLayoutCreation vlc = { _drawCall._inputAssembler._vertexBuffer, vls, ArrayLength( vls ) };

    _drawCall._inputAssembler._layout = ctx.rd.CreateVertexLayout( vlc );

    _drawCall._shaders = ctx.shaderCache.FindShaderState( "debug" );

    _lineCount = 0;
}

void DebugRenderer::Terminate() {

}

void DebugRenderer::Update() {
    // map/unmap vertex data
}

void DebugRenderer::Render( RenderContext& rc ) {

    // Axis
    AddLine( glm::vec3{ 0.0f, 0.0f, 0.f }, glm::vec3{ 130.0f, 0.0f, 0.f }, glm::vec3{ 1.f, 0.f, 0.f } );
    AddLine( glm::vec3{ 0.0f, 0.0f, 0.f }, glm::vec3{ 0.0f, 130.0f, 0.f }, glm::vec3{ 0.f, 1.f, 0.f } );
    AddLine( glm::vec3{ 0.0f, 0.0f, 0.f }, glm::vec3{ 0.0f, 0.0f, -130.1f }, glm::vec3{ 0.f, 0.f, 1.f } );

    //AddLine( glm::vec3{ 0.0f, 0.0f, 0.1f }, glm::vec3{ 1300.0f, 80.0f, 0.1f }, glm::vec3{ 1.f, 1.f, 1.f } );
    AddTriangle( glm::vec3{ 30.0f, 20.0f, 0.1f }, glm::vec3{ 130.0f, 80.0f, 0.1f }, glm::vec3{ 200.f, 40.f, 0.1f }, glm::vec3{ 1.f, 1.f, 1.f } );
    
    rc.WriteToGpu( _drawCall._inputAssembler._vertexBuffer, _lines.begin(), sizeof( DebugVertex ) * _lines.maxSize() );

    _drawCall._count = _lineCount * 2;

    rc.Draw( _drawCall );

    _lineCount = 0;
}

void DebugRenderer::AddLine( const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& color ) {
    _lines[_lineCount++] = { { v0 }, { color } };
    _lines[_lineCount++] = { { v1 }, { color } };
}

void DebugRenderer::AddTriangle( const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& color ) {
    AddLine( v0, v1, color );
    AddLine( v1, v2, color );
    AddLine( v2, v0, color );
}



} // namespace hydra
} // namespace gfx

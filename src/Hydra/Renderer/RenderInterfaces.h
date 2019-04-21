#pragma once

#include <Hydra/Kernel/Lib.h>
#include <Hydra/Renderer/RenderResources.h>


#if defined(HY_OPENGL)
#include "RendererOpenGL.h"
#elif defined(HY_DX11)
#include "RendererDX11.h"
#else
#endif


namespace hydra {
namespace gfx {

struct RenderDevice;
    
//////////////////////////////////////////////////////////////////////////
  
struct RenderDatabase {
    Array<VertexProgram>            _vps;
    Array<FragmentProgram>          _fps;
    Array<ShaderState>              _shaderStates;
    Array<Texture>                  _textures;
    Array<Sampler>                  _samplers;
    Array<FrameBufferState>         _fbos;
    Array<VertexBuffer>             _vbs;
    Array<VertexLayout>             _vls;
    Array<ComputeProgram>           _cps;

    VertexLayoutHandle              _postVl;
    VertexProgramHandle             _postVp;


    ShaderStateHandle FindShaderState( cstring name );

    VertexLayoutHandle GetPostProcessVertexLayout() { return _postVl; }
};


struct InputAssembler {
    VertexLayoutHandle              _layout;
    VertexBufferHandle              _vertexBuffer;
}; // struct InputAssembler

struct DrawCall {
    InputAssembler                  _inputAssembler;
    ShaderStateHandle               _shaders;
    uint32                          _count;
}; // struct DrawCall


struct RenderDevice {

    RenderDatabase                  _renderDb;
    uint16                          _width, _height;
    bool                            _debugOutput;

    void Init( WindowSystem* w );
    void InitCommon( WindowSystem* w );

    // Resources
    VertexLayoutHandle              CreateVertexLayout( const VertexLayoutCreation& c );
    VertexBufferHandle              CreateVertexBuffer( const VertexBufferCreation& c );

    VertexProgramHandle             CreateVertexProgram( const ShaderCreation& c );
    FragmentProgramHandle           CreateFragmentProgram( const ShaderCreation& c );
    ComputeProgramHandle            CreateComputeProgram( const ShaderCreation& c );

    TextureHandle                   CreateTexture2D( const TextureCreation& c );
    SamplerHandle                   CreateSampler( const SamplerCreation& c );

    // States
    ShaderStateHandle               CreateShaderState( const ShaderStateCreation& c );
    FrameBufferStateHandle          CreateFrameBufferState( const FrameBufferCreation& c );

    void                            EnableDebugOutput( bool value ) { _debugOutput = value; }
    
    // Render
    void                            Render();

    void                            SwapBuffers();
};
} // namespace gfx
} // namespace hydra

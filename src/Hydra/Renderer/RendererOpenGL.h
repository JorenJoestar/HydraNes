#pragma once

#if defined(HY_OPENGL)
#include "GL/glew.h"
#include <GL/wglew.h>

#include <Hydra/Renderer/RenderResources.h>

namespace hydra {
namespace gfx {

struct VertexProgram {
    GLuint              handle;
};

struct FragmentProgram {
    GLuint              handle;
};

struct ComputeProgram {
    GLuint              handle;
};

struct VertexBuffer {
    GLuint              handle;
    uint32              size;
    uint32              stride;
};

struct VertexLayout {
    GLuint              handle;
};

struct Texture {
    GLuint              handle;
    TextureFormat::Enum format;
    uint16              width;
    uint16              height;
};

struct Sampler {
    GLuint              handle;
};

struct FrameBuffer {
    GLuint              handle;
};

//////////////////////////////////////////////////////////////////////////

// states

struct VertexInputState {
    VertexBindingHandle                 bindings;
    VertexLayoutHandle                  layout;
    IndexBufferHandle                   ib;
};

struct ShaderState {
    GLuint                              handle;                     // shader handle
    VertexProgramHandle                 vp;
    FragmentProgramHandle               fp;
    cstring                             name;
};

struct FrameBufferState {

    GLuint                              handle;                     // fbo handle
    TextureHandle                       rts[kMaxRenderTargets];
    TextureHandle                       ds;                         // depth stencil
    uint8                               clearColor[4];
    uint8                               activeRts;

    uint8                               hasDepthStencil : 1;
    uint8                               clear : 1;
    uint8                               isFrameBuffer : 1;
};


} // namespace hydra
} // namespace gfx

#endif // HY_OPENGL
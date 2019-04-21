#pragma once

#include <Hydra/Kernel/Lib.h>

namespace hydra {

namespace gfx {

struct RenderContext;
struct RenderDevice;
struct RenderDatabase;

struct RenderSystemInitContext {
    RenderDevice& rd;
    RenderDatabase& shaderCache;
};

struct RenderSystem : public UpdateSystem {

    RenderSystem( uint32 order ) : UpdateSystem( order ) {}

    virtual void Init( RenderSystemInitContext& ctx ) {}

    virtual void Render( RenderContext& rc ) {}
    virtual void Present() {}
}; // struct RenderSystem

} // namespace hydra
} // namespace gfx
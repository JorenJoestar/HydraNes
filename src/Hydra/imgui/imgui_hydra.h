#pragma once

namespace hydra {

    struct WindowSystem;

    namespace input {
        struct InputSystem;
    }

    namespace gfx {
        struct RenderDevice;
    }

    void ImGuiInit( WindowSystem& window, gfx::RenderDevice& rd );
    void ImGuiTerminate( gfx::RenderDevice& rd );

    void ImGuiNewFrame( WindowSystem& window, input::InputSystem& input, gfx::RenderDevice& rd );
}
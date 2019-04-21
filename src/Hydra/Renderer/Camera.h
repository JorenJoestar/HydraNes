#pragma once

#include <Hydra/Kernel/Lib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Hydra/Renderer/RenderSystem.h>

namespace hydra {

namespace input {
    struct InputSystem;
} // namespace input

namespace gfx {

struct RenderSystemInitContext;

struct Frustrum {
};

struct Camera {

    void Ortho( float w, float h );
    void Perspective( float fov, float w, float h );

    glm::vec3           _position;
    glm::vec3           _direction;
    glm::vec3           _up;

    glm::mat4           _view;
    glm::mat4           _proj;
    glm::mat4           _viewProj;

    float               _near;
    float               _far;

    float               _viewportWidth;
    float               _viewportHeight;
}; // struct Camera

struct CameraSystem : public RenderSystem {

    CameraSystem( uint32 order ) : RenderSystem( order ) {}

    void Init( RenderSystemInitContext& ctx ) override;
    void Update() override;

    Array<Camera>       _cameras;

    Camera*             _currentCamera;
    Camera*             _mainCamera;
    Camera*             _debugCamera;
}; // struct CameraSystem

struct CameraInputSystem : public RenderSystem {
    CameraInputSystem( uint32 order ) : RenderSystem( order ) {}
    input::InputSystem* _input;
    Camera*             _camera;

    void Update() override;
}; // struct CameraInputSystem


} // namespace hydra
} // namespace gfx
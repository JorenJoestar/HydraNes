#include <Hydra/Renderer/Camera.h>
#include <Hydra/Kernel/Input.h>
#include <Hydra/Renderer/Renderer.h>

namespace hydra {
namespace gfx {

//////////////////////////////////////////////////////////////////////////
void CameraSystem::Init( RenderSystemInitContext& ctx ) {
    
    Camera m;
    m._near = 0.1f;
    m._far = 1000.0f;
    m._position = { 0.0f, 0.0f, 1.0f };
    m._direction = { .0f, .0f, -1.f };
    m._up = { .0f, 1.f, .0f };
    m.Perspective( 45.0f, ctx.rd._width, ctx.rd._height );
    //m.Ortho( ctx.rd._width, ctx.rd._height );

    _cameras.push_back( m );
    _mainCamera = _currentCamera = &_cameras.back();

    _debugCamera = nullptr;
}

void CameraSystem::Update() {
    for ( auto& c : _cameras ) {
        c._view = glm::lookAt( c._position, c._position + c._direction, c._up );
        c._viewProj = c._proj * c._view;
    }
}

void Camera::Ortho( float w, float h ) {
    _proj = glm::ortho( 0.0f, w, 0.0f, h, _near, _far );
    _viewportWidth = w;
    _viewportHeight = h;
}

void Camera::Perspective( float fov, float w, float h ) {
    _proj = glm::perspective( fov, w / h, _near, _far );
    _viewportWidth = w;
    _viewportHeight = h;
}

//////////////////////////////////////////////////////////////////////////
float _cameraSpeed = 0.1f;
float yaw = -90.0f, pitch = 0.0f;

void CameraInputSystem::Update() {

    float cameraSpeedMultiplier = _cameraSpeed;
    input::Keyboard& keys = _input->_input.keys;
    if (keys.IsKeyDown( input::KEY_LCONTROL ) || keys.IsKeyDown( input::KEY_RCONTROL ) ) {
        cameraSpeedMultiplier *= 10.0f;
    }

    if ( keys.IsKeyDown( input::KEY_W ) || keys.IsKeyDown( input::KEY_UP ) ) {
        _camera->_position += cameraSpeedMultiplier * _camera->_direction;
    }
    if ( keys.IsKeyDown( input::KEY_S ) || keys.IsKeyDown( input::KEY_DOWN ) ) {
        _camera->_position -= cameraSpeedMultiplier * _camera->_direction;
    }
    if ( keys.IsKeyDown( input::KEY_A ) || keys.IsKeyDown( input::KEY_LEFT ) ) {
        _camera->_position -= cameraSpeedMultiplier * glm::normalize( glm::cross(_camera->_direction, _camera->_up) );
    }
    if ( keys.IsKeyDown( input::KEY_D ) || keys.IsKeyDown( input::KEY_RIGHT ) ) {
        _camera->_position += cameraSpeedMultiplier * glm::normalize( glm::cross( _camera->_direction, _camera->_up ) );
    }

    if ( !_input->_input.mouse.IsButtonDown( input::MouseButtons::Left ) )
        return;

    float xoffset = _input->_input.mouse.OffsetX() * 1.0f;
    // Invert y-axis!
    float yoffset = _input->_input.mouse.OffsetY() * -1.0f;

    if ( xoffset != 0 || yoffset != 0 ) {
        float sensitivity = 0.05f;
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if ( pitch > 89.0f )
            pitch = 89.0f;
        if ( pitch < -89.0f )
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
        front.y = sin( glm::radians( pitch ) );
        front.z = sin( glm::radians( yaw ) ) * cos( glm::radians( pitch ) );
        _camera->_direction = glm::normalize( front );

        glm::vec3 right = glm::normalize( glm::cross( _camera->_direction, glm::vec3{ 0.f ,1.f, 0.f } ) );  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        _camera->_up = glm::normalize( glm::cross( right, _camera->_direction ) );
    }
}
} // namespace hydra
} // namespace gfx

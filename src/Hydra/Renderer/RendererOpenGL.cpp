#include <Hydra/Renderer/RendererOpenGL.h>
#include <Hydra/Renderer/RenderInterfaces.h>
#include <Hydra/Renderer/Renderer.h>
#include <Hydra/Renderer/Camera.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#if defined(HY_OPENGL)
namespace hydra {
namespace gfx {


struct RenderDeviceGL {
    HDC                 _hdc;
    HGLRC               _glRenderContext;

    VertexLayoutHandle  _debugVl;
    ShaderState         _debugShaderState;

    bool                _debugOutputEnabled;

    void                CreatePostprocessResources( RenderDevice& rd, RenderDatabase& rdb );
    void                CreateDebugResources( RenderDevice& rd, RenderDatabase& rdb );
};

const GLchar* sDebugVSCode =
"#version 330 core\n"
"in vec3 position;\n"
"in vec3 color;\n"
"out vec3 fragColor;\n"
"uniform mat4 viewProjection;\n"
"void main() {\n"
"   gl_Position = viewProjection * vec4( position.xyz, 1.0 );\n"
"   fragColor = color;\n"
"}\n";

const GLchar* sDebugFsCode =
"#version 330 core\n"
"in vec3 fragColor;\n"
"out vec4 color;\n"
"void main() {\n"
"    color = vec4( fragColor, 1.0f );\n"
"}\n";

// Vertex shader with no buffers!
// http://www.slideshare.net/DevCentralAMD/vertex-shader-tricks-bill-bilodeau
// https://rauwendaal.net/2014/06/14/rendering-a-screen-covering-triangle-in-opengl/
const GLchar *sPostProcessVsCode =
"#version 330 core\n"

"out vec2 Frag_UV;\n"
"void main()\n"
"{\n"
"   float x = -1.0 + float((gl_VertexID & 1) << 2);\n"
"   float y = -1.0 + float( ( gl_VertexID & 2 ) << 1 );\n"
"   Frag_UV.x = ( x + 1.0 )*0.5;\n"
"   Frag_UV.y = ( y + 1.0 )*0.5; \n"
"	gl_Position = vec4(x, y,0,1);\n"
"}\n";

const GLchar* sPostProcessFsCode =
"#version 330 core\n"
"uniform sampler2D Texture;\n"
"in vec2 Frag_UV;\n"
"out vec4 Out_Color;\n"
"void main()\n"
"{\n"
"	Out_Color = texture( Texture, Frag_UV.st);\n"
"	Out_Color.a = 1;\n"
// "	Out_Color = vec4(Frag_UV.st,1,1);\n"
"}\n";

static GLchar errorLog[512];

void GLAPIENTRY OpenGLDebugLog( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void* userParam ) {
    /* printf( " Type : %s; Source : %s; ID : %d; Severity : % s\n",
    getStringForType( type ).c_str(),
    getStringForSource( source ).c_str(), id,
    getStringForSeverity( severity ).c_str() );*/
    PrintFormat( "OpenGL error!!! Message : %s\n", message );
}

// Creation and destroy
GLfloat vertices[] = {
    // Positions         // Colors
    0.5f, -0.5f, 0.1f,  1.0f, 0.0f, 0.0f,   // Bottom Right
    -0.5f, -0.5f, 0.1f,  0.0f, 1.0f, 0.0f,   // Bottom Left
    0.0f,  0.5f, 0.1f,  0.0f, 0.0f, 1.0f    // Top 
};

GLfloat screenSpaceVertices[] = {
    // Positions         // Colors
    0.0f, 0.0f, 0.1f,  1.0f, 0.0f, 0.0f,   // Bottom Right
    100.f, 100.f, 0.1f,  0.0f, 1.0f, 0.0f,   // Bottom Left
    0.0f,  100.f, 0.1f,  0.0f, 0.0f, 1.0f    // Top 
};


void RenderDeviceGL::CreatePostprocessResources( RenderDevice& rd, RenderDatabase& rdb ) {
    // Create post process vertex layout
    gfx::VertexBufferCreation vbc = { nullptr, sizeof( float ), sizeof( float ), true };
    gfx::VertexBufferHandle vb = rd.CreateVertexBuffer( vbc );
    // create vl
    gfx::VertexLayoutElement vls[] = { { 0, 1, gfx::VertexDataType::Float, gfx::VertexUsage::Position, 0, gfx::VertexFrequency::Vertex } };
    gfx::VertexLayoutCreation vlc = { vb, vls, ArrayLength( vls ) };
    rdb._postVl = rd.CreateVertexLayout( vlc );

    ShaderStateCreation ssc = { (gfx::VertexProgramHandle)-1, (gfx::FragmentProgramHandle)-1, sPostProcessVsCode, sPostProcessFsCode, "postprocess" };
    rd.CreateShaderState( ssc );
}

void RenderDeviceGL::CreateDebugResources( RenderDevice& rd, RenderDatabase& rdb ) {

    ShaderStateCreation ssc{ ( gfx::VertexProgramHandle ) - 1, ( gfx::FragmentProgramHandle ) - 1, sDebugVSCode, sDebugFsCode, "debug" };
    rd.CreateShaderState( ssc );

    // create vb
    VertexBufferCreation vbc = { screenSpaceVertices, sizeof( vertices ), sizeof( GLfloat ) * 6, true };
    VertexBufferHandle vb = rd.CreateVertexBuffer( vbc );
    // create vl
    VertexLayoutElement vls[] = { { 0, 3, VertexDataType::Float, VertexUsage::Position, 0, VertexFrequency::Vertex },
    { 0, 3, VertexDataType::Float, VertexUsage::Color, 0, VertexFrequency::Vertex } };
    VertexLayoutCreation vlc = { vb, vls, ArrayLength( vls ) };
    _debugVl = rd.CreateVertexLayout( vlc );
}

RenderDeviceGL sDeviceGL;


void RenderDevice::Init( WindowSystem* w ) {
    InitCommon( w );

    // TODO: Gl
    PIXELFORMATDESCRIPTOR pfd;
    memset( &pfd, 0, sizeof( PIXELFORMATDESCRIPTOR ) );
    pfd.nSize = sizeof( PIXELFORMATDESCRIPTOR );
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 0;
    pfd.iLayerType = PFD_MAIN_PLANE;

    HDC hdc = GetDC( w->handle );

    auto nPixelFormat = ChoosePixelFormat( hdc, &pfd );
    SetPixelFormat( hdc, nPixelFormat, &pfd );

    HGLRC glTempContext = wglCreateContext( hdc );
    wglMakeCurrent( hdc, glTempContext );

    GLenum err = glewInit();
    if ( GLEW_OK != err ) {
        PrintFormat( "Error: %s\n", glewGetErrorString( err ) );
    }
    int attribs[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 5,
        WGL_CONTEXT_FLAGS_ARB, 0,
        0
    };

    HGLRC glRenderContext;

    if ( wglewIsSupported( "WGL_ARB_create_context" ) == 1 ) {
        glRenderContext = wglCreateContextAttribsARB( hdc, 0, attribs );
        wglMakeCurrent( NULL, NULL );
        wglDeleteContext( glTempContext );
        wglMakeCurrent( hdc, glRenderContext );
    } else {	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
        glRenderContext = glTempContext;
    }

    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(  OpenGLDebugLog, nullptr );
    GLuint unusedIds = 0;
    glDebugMessageControl( GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true );

    int OpenGLVersion[2];
    glGetIntegerv( GL_MAJOR_VERSION, &OpenGLVersion[0] );
    glGetIntegerv( GL_MINOR_VERSION, &OpenGLVersion[1] );

    wglSwapIntervalEXT( 1 );

    sDeviceGL._hdc = hdc;
    sDeviceGL._glRenderContext = glRenderContext;

    sDeviceGL.CreateDebugResources( *this, _renderDb );
    sDeviceGL.CreatePostprocessResources( *this, _renderDb );

    sDeviceGL._debugOutputEnabled = _debugOutput;

    if ( !_debugOutput )
        glDisable( GL_DEBUG_OUTPUT );
}

void RenderDevice::Render() {

    if ( _debugOutput != sDeviceGL._debugOutputEnabled ) {
        sDeviceGL._debugOutputEnabled = _debugOutput;

        if ( _debugOutput ) {
            glEnable( GL_DEBUG_OUTPUT );
        }
        else {
            glDisable( GL_DEBUG_OUTPUT );
        }
    }

    glViewport( 0, 0, _width, _height );
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_STENCIL_TEST );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    glDisable( GL_DEPTH_TEST );
    glDisable( GL_STENCIL_TEST );
}

void RenderDevice::SwapBuffers() {
    ::SwapBuffers( sDeviceGL._hdc );
}

//////////////////////////////////////////////////////////////////////////
uint32 s_VertexDataSizes[] = {
    4, 1, 1, 2, 2, 4, 4, 0
};

uint32 s_VertexDataTypes[] = {
    GL_FLOAT, GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT
};

uint8 s_VertexNormalized[] = {
    GL_FALSE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE
};

/*
// https://www.khronos.org/opengles/sdk/docs/man3/docbook4/xhtml/glTexImage2D.xml
// https://msdn.microsoft.com/en-us/library/windows/desktop/bb173059(v=vs.85).aspx
// https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
// https://www.opengl.org/wiki/GLAPI/glTexStorage2D
*/

static GLuint toGLInternalFormat( TextureFormat::Enum format ) {

    //UNKNOWN, R32G32B32A32_TYPELESS, R32G32B32A32_FLOAT, R32G32B32A32_UINT, R32G32B32A32_SINT, R32G32B32_TYPELESS, R32G32B32_FLOAT, R32G32B32_UINT, R32G32B32_SINT, R16G16B16A16_TYPELESS, R16G16B16A16_FLOAT, 
    // R16G16B16A16_UNORM, R16G16B16A16_UINT, R16G16B16A16_SNORM, R16G16B16A16_SINT, R32G32_TYPELESS, R32G32_FLOAT, R32G32_UINT, R32G32_SINT, R32G8X24_TYPELESS, D32_FLOAT_S8X24_UINT, R32_FLOAT_X8X24_TYPELESS, 
    // X32_TYPELESS_G8X24_UINT, R10G10B10A2_TYPELESS, R10G10B10A2_UNORM, R10G10B10A2_UINT, R11G11B10_FLOAT, R8G8B8A8_TYPELESS, R8G8B8A8_UNORM, R8G8B8A8_UNORM_SRGB, R8G8B8A8_UINT, R8G8B8A8_SNORM, R8G8B8A8_SINT, 
    //R16G16_TYPELESS, R16G16_FLOAT, R16G16_UNORM, R16G16_UINT, R16G16_SNORM, R16G16_SINT, R32_TYPELESS, D32_FLOAT, R32_FLOAT, R32_UINT, R32_SINT, R24G8_TYPELESS, D24_UNORM_S8_UINT,     
    //R24_UNORM_X8_TYPELESS, X24_TYPELESS_G8_UINT, R8G8_TYPELESS, R8G8_UNORM, R8G8_UINT, R8G8_SNORM, R8G8_SINT, R16_TYPELESS, R16_FLOAT, D16_UNORM, R16_UNORM, R16_UINT, R16_SNORM, R16_SINT, 
    // R8_TYPELESS, R8_UNORM, R8_UINT, R8_SNORM, R8_SINT, A8_UNORM, R1_UNORM, R9G9B9E5_SHAREDEXP, R8G8_B8G8_UNORM, G8R8_G8B8_UNORM, BC1_TYPELESS, BC1_UNORM, BC1_UNORM_SRGB, BC2_TYPELESS, BC2_UNORM, 
    // BC2_UNORM_SRGB, BC3_TYPELESS, BC3_UNORM, BC3_UNORM_SRGB, BC4_TYPELESS, BC4_UNORM, BC4_SNORM, BC5_TYPELESS, BC5_UNORM, BC5_SNORM, B5G6R5_UNORM, B5G5R5A1_UNORM, B8G8R8A8_UNORM, B8G8R8X8_UNORM, 
    // R10G10B10_XR_BIAS_A2_UNORM, B8G8R8A8_TYPELESS, B8G8R8A8_UNORM_SRGB, B8G8R8X8_TYPELESS, B8G8R8X8_UNORM_SRGB, BC6H_TYPELESS, BC6H_UF16, BC6H_SF16, BC7_TYPELESS, BC7_UNORM, BC7_UNORM_SRGB, FORCE_UINT, max
    static GLuint glFormats[] = { GL_FALSE, GL_FALSE, GL_RGBA32F, GL_RGBA32UI, GL_RGBA32I, GL_FALSE, GL_RGB32F, GL_RGB32UI, GL_RGB32I, GL_FALSE, GL_RGBA16F, 
                                  GL_RGBA16, GL_RGBA16UI, GL_RGBA16_SNORM, GL_RGBA16I, GL_FALSE, GL_RG32F, GL_RG32UI, GL_RG32I, GL_FALSE, GL_DEPTH32F_STENCIL8, GL_FALSE,
                                  GL_FALSE, GL_FALSE, GL_RGB10_A2, GL_RGB10_A2UI, GL_R11F_G11F_B10F, GL_FALSE, GL_RGBA8, GL_SRGB8_ALPHA8, GL_RGBA8UI, GL_RGBA8_SNORM, GL_RGBA8I,
                                  GL_FALSE, GL_RG16F, GL_RG16, GL_RG16UI, GL_RG16_SNORM, GL_R16I, GL_FALSE, GL_DEPTH_COMPONENT32F, GL_R32F, GL_R32UI, GL_R32I, GL_FALSE, GL_DEPTH24_STENCIL8,
                                  GL_FALSE, GL_FALSE, GL_FALSE, GL_RG8, GL_RG8UI, GL_RG8_SNORM, GL_RG8I, GL_FALSE, GL_R16F, GL_DEPTH_COMPONENT16, GL_R16, GL_R16UI, GL_R16_SNORM, GL_R16I,
                                  GL_FALSE, GL_R8, GL_R8UI, GL_R8_SNORM, GL_R8I, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 
                                  GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, 
                                  GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE };
    return glFormats[format];
}

static GLuint toGLFormat( TextureFormat::Enum format ) {

    switch ( format ) {

        // rgba
        case TextureFormat::R32G32B32A32_TYPELESS:
        case TextureFormat::R16G16B16A16_TYPELESS:
        case TextureFormat::R8G8B8A8_TYPELESS:

        case TextureFormat::R32G32B32A32_FLOAT:
        case TextureFormat::R16G16B16A16_FLOAT:
        case TextureFormat::R8G8B8A8_UNORM_SRGB:
        case TextureFormat::R16G16B16A16_UNORM:
        case TextureFormat::R16G16B16A16_SNORM:
        case TextureFormat::R8G8B8A8_UNORM:
        case TextureFormat::R8G8B8A8_SNORM:

        case TextureFormat::R10G10B10A2_TYPELESS:
        case TextureFormat::R10G10B10A2_UNORM:
        case TextureFormat::R10G10B10A2_UINT:
            return GL_RGBA;

        case TextureFormat::R32G32B32A32_UINT:
        case TextureFormat::R32G32B32A32_SINT:
        case TextureFormat::R16G16B16A16_UINT:
        case TextureFormat::R16G16B16A16_SINT:
        case TextureFormat::R8G8B8A8_UINT:
        case TextureFormat::R8G8B8A8_SINT:
            return GL_RGBA_INTEGER;

            // rgb
        case TextureFormat::R32G32B32_TYPELESS:
        case TextureFormat::R32G32B32_FLOAT:
        case TextureFormat::R11G11B10_FLOAT:
            return GL_RGB;

        case TextureFormat::R32G32B32_UINT:
        case TextureFormat::R32G32B32_SINT:
            return GL_RGB_INTEGER;

            // rg
        case TextureFormat::R32G32_TYPELESS:
        case TextureFormat::R32G32_FLOAT:
        case TextureFormat::R16G16_TYPELESS:
        case TextureFormat::R16G16_FLOAT:
        case TextureFormat::R8G8_TYPELESS:
        case TextureFormat::R16G16_UNORM:
        case TextureFormat::R16G16_SNORM:
        case TextureFormat::R8G8_UNORM:
        case TextureFormat::R8G8_SNORM:
            return GL_RG;

        case TextureFormat::R32G32_UINT:
        case TextureFormat::R32G32_SINT:
        case TextureFormat::R16G16_UINT:
        case TextureFormat::R16G16_SINT:
        case TextureFormat::R8G8_UINT:
        case TextureFormat::R8G8_SINT:
            return GL_RG_INTEGER;

            // r
        case TextureFormat::R32_TYPELESS:
        case TextureFormat::R32_FLOAT:
        case TextureFormat::R16_TYPELESS:
        case TextureFormat::R16_FLOAT:
        case TextureFormat::R16_UNORM:
        case TextureFormat::R16_SNORM:
        case TextureFormat::R8_TYPELESS:
        case TextureFormat::R8_UNORM:
        case TextureFormat::R8_SNORM:
        case TextureFormat::R1_UNORM:
            return GL_RED;

        case TextureFormat::R32_UINT:
        case TextureFormat::R32_SINT:
        case TextureFormat::R16_UINT:
        case TextureFormat::R16_SINT:
        case TextureFormat::R8_UINT:
        case TextureFormat::R8_SINT:
            return GL_RED_INTEGER;

        case TextureFormat::D16_UNORM:
        case TextureFormat::D32_FLOAT:
            return GL_DEPTH_COMPONENT;

        case TextureFormat::D32_FLOAT_S8X24_UINT:
        case TextureFormat::D24_UNORM_S8_UINT:
            return GL_DEPTH_STENCIL;

        case TextureFormat::B8G8R8A8_TYPELESS:
        case TextureFormat::B8G8R8X8_TYPELESS:
        case TextureFormat::B8G8R8X8_UNORM_SRGB:
        case TextureFormat::B8G8R8A8_UNORM_SRGB:
        case TextureFormat::B5G5R5A1_UNORM:
        case TextureFormat::B8G8R8A8_UNORM:
        case TextureFormat::B8G8R8X8_UNORM:
            return GL_BGRA;

        case TextureFormat::B5G6R5_UNORM:
            return GL_BGR_INTEGER;

        default:
            Assert( false && "format not recognized" );
            return GL_RGBA;
    }
}

static GLuint toGLType( TextureFormat::Enum format ) {

    switch ( format ) {
        case TextureFormat::R10G10B10A2_UNORM:
        case TextureFormat::R10G10B10A2_UINT:
            return GL_UNSIGNED_INT_10_10_10_2;
            
        case TextureFormat::R32G32B32A32_FLOAT:
        case TextureFormat::R32G32B32_FLOAT:
        case TextureFormat::R32G32_FLOAT:
        case TextureFormat::R16G16_FLOAT:
        case TextureFormat::R32_FLOAT:
        case TextureFormat::R11G11B10_FLOAT:
            return GL_FLOAT;

        case TextureFormat::R16G16B16A16_FLOAT:
        case TextureFormat::R16_FLOAT:
            return GL_HALF_FLOAT;

            
        case TextureFormat::R32G32B32A32_UINT :
        case TextureFormat::R32G32B32_UINT:
        case TextureFormat::R32G32_UINT:
        case TextureFormat::R32_UINT:
            return GL_UNSIGNED_INT;
            
        case TextureFormat::R32G32B32A32_SINT:
        case TextureFormat::R32G32B32_SINT:
        case TextureFormat::R32G32_SINT:
        case TextureFormat::R32_SINT:
            return GL_INT;
            
        case TextureFormat::R16G16B16A16_UNORM:
        case TextureFormat::R16G16B16A16_UINT:
        case TextureFormat::R16G16_UNORM:
        case TextureFormat::R16G16_UINT:
        case TextureFormat::R16_UNORM:
        case TextureFormat::R16_UINT:
            return GL_UNSIGNED_SHORT;
            
        case TextureFormat::R16G16B16A16_SNORM:
        case TextureFormat::R16G16B16A16_SINT:
        case TextureFormat::R16G16_SNORM:
        case TextureFormat::R16G16_SINT:
        case TextureFormat::R16_SNORM:
        case TextureFormat::R16_SINT:
            return GL_SHORT;
            
        case TextureFormat::R8G8B8A8_UNORM:
        case TextureFormat::R8G8B8A8_UNORM_SRGB:
        case TextureFormat::R8G8B8A8_UINT:
        case TextureFormat::R8G8_UNORM:
        case TextureFormat::R8G8_UINT:
        case TextureFormat::R8_UNORM:
        case TextureFormat::R8_UINT:
            return GL_UNSIGNED_BYTE;
            
        case TextureFormat::R8G8B8A8_SNORM:
        case TextureFormat::R8G8B8A8_SINT:
        case TextureFormat::R8G8_SNORM:
        case TextureFormat::R8G8_SINT:
        case TextureFormat::R8_SNORM:
        case TextureFormat::R8_SINT:
            return GL_BYTE;

        case TextureFormat::D32_FLOAT_S8X24_UINT:
            return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
            
        case TextureFormat::D32_FLOAT:
            return GL_FLOAT;
            
        case TextureFormat::D24_UNORM_S8_UINT:
            return GL_UNSIGNED_INT_24_8;
            
        case TextureFormat::D16_UNORM:
            return GL_UNSIGNED_SHORT;
        default:
            Assert( false && "format not recognized" );
            return GL_UNSIGNED_BYTE;
    }
}

//////////////////////////////////////////////////////////////////////////
void* HandleToData( GLuint handle ) {
    return (void*)(uintptr_t)handle;
}

// https://www.opengl.org/wiki_132/index.php/Vertex_Rendering
VertexLayoutHandle RenderDevice::CreateVertexLayout( const VertexLayoutCreation& c ) {
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    glBindBuffer( GL_ARRAY_BUFFER, c.vb->handle );
    
    uint32 offset = 0;
    const uint32 stride = c.vb->stride;
    for ( uint32 i = 0; i < c.elementCount; ++i ) {
        const VertexLayoutElement& e = c.elements[i];
        const uint32 glType = s_VertexDataTypes[e.type];
        const uint8 glNormalized = s_VertexNormalized[e.type];
        glVertexAttribPointer( i, e.size, glType, glNormalized, stride, HandleToData(offset) );
        glEnableVertexAttribArray( i );
        uint32 size = s_VertexDataSizes[e.type];
        offset += e.size * size;
    }

    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindVertexArray( 0 );

    VertexLayout vl;
    vl.handle = vao;

    _renderDb._vls.push_back( vl );

    return &_renderDb._vls.back();
}

VertexBufferHandle RenderDevice::CreateVertexBuffer( const VertexBufferCreation& c ) {
    GLuint vbo;
    glGenBuffers( 1, &vbo );
    glBindBuffer( GL_ARRAY_BUFFER, vbo );
    if ( c.data != nullptr ) {
        glBufferData( GL_ARRAY_BUFFER, c.size, c.data, c.isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );
    }

    VertexBuffer vb;
    vb.handle = vbo;
    vb.size = c.size;
    vb.stride = c.stride;

    _renderDb._vbs.push_back( vb );

    return &_renderDb._vbs.back();
}

static void CheckShaderLinking( GLuint shaderHandle ) {
    GLint result = 0;
    glGetProgramiv( shaderHandle, GL_LINK_STATUS, &result );
    if ( result == GL_FALSE ) {
        errorLog[0] = 0;
        glGetShaderInfoLog( shaderHandle, 512, 0, &errorLog[0] );
        PrintFormat( "Error creating program %s\n", errorLog );
    }
}

static GLuint CreateProgram( GLuint shaderStage, const GLchar* code ) {
    GLuint handle = glCreateShader( shaderStage );
    
    glShaderSource( handle, 1, &code, 0 );
    glCompileShader( handle );

    GLint result = 0;
    glGetShaderiv( handle, GL_COMPILE_STATUS, &result );
    if ( result == GL_FALSE ) {

        errorLog[0] = 0;
        glGetShaderInfoLog( handle, 512, 0, &errorLog[0] );
        PrintFormat( "Error creating shader %s\n", errorLog );

        glDeleteShader( handle );
    }

    return handle;
}

VertexProgramHandle RenderDevice::CreateVertexProgram( const ShaderCreation& c ) {
    VertexProgram vp;
    vp.handle = CreateProgram( GL_VERTEX_SHADER, c.code );
    _renderDb._vps.push_back( vp );

    return &_renderDb._vps.back();
}

FragmentProgramHandle RenderDevice::CreateFragmentProgram( const ShaderCreation& c ) {
    FragmentProgram fp;
    fp.handle = CreateProgram( GL_FRAGMENT_SHADER, c.code );
    _renderDb._fps.push_back( fp );

    return &_renderDb._fps.back();
}

ComputeProgramHandle RenderDevice::CreateComputeProgram( const ShaderCreation& c ) {
    
    GLuint computeHandle = CreateProgram( GL_COMPUTE_SHADER, c.code );

    GLuint shaderHandle = glCreateProgram();
    glAttachShader( shaderHandle, computeHandle );
    glLinkProgram( shaderHandle );

    CheckShaderLinking( shaderHandle );

    ComputeProgram cp;
    cp.handle = shaderHandle;
    _renderDb._cps.push_back( cp );

    return &_renderDb._cps.back();
}

ShaderStateHandle RenderDevice::CreateShaderState( const ShaderStateCreation& c ) {
    
    VertexProgramHandle vp = c.vp;
    if ( c.vpSource ) {
        ShaderCreation sc{ c.vpSource, c.name };
        vp = CreateVertexProgram( sc );
    }

    FragmentProgramHandle fp = c.fp;
    if ( c.fpSource ) {
        ShaderCreation sc{ c.fpSource, c.name };
        fp = CreateFragmentProgram( sc );
    }

    ShaderState shaderState;
    shaderState.vp = vp;
    shaderState.fp = fp;

    GLuint shaderHandle = glCreateProgram();
    glAttachShader( shaderHandle, vp->handle );
    glAttachShader( shaderHandle, fp->handle );
    glLinkProgram( shaderHandle );

    CheckShaderLinking( shaderHandle );

    shaderState.handle = shaderHandle;
    shaderState.name = c.name;
    _renderDb._shaderStates.push_back( shaderState );

    return &_renderDb._shaderStates.back();
}

TextureHandle RenderDevice::CreateTexture2D( const TextureCreation& c ) {
    GLuint handle;
    glGenTextures( 1, &handle );
    glBindTexture( GL_TEXTURE_2D, handle );

    glTexImage2D( GL_TEXTURE_2D, 0, toGLInternalFormat( c.format ), c.width, c.height, 0, toGLFormat( c.format ), toGLType( c.format ), c.data );

    // TODO:
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBindTexture( GL_TEXTURE_2D, 0 );
    Texture tex;
    tex.handle = handle;
    tex.format = c.format;
    tex.width = c.width;
    tex.height = c.height;

    _renderDb._textures.push_back( tex );

    return &_renderDb._textures.back();
}

SamplerHandle RenderDevice::CreateSampler( const SamplerCreation& c ) {
    GLuint handle;
    glGenSamplers(1, &handle);
    glBindSampler( GL_SAMPLER, handle );

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    glBindSampler( GL_SAMPLER, 0 );

    return nullptr;
}

FrameBufferStateHandle RenderDevice::CreateFrameBufferState( const FrameBufferCreation& c ) {
    GLuint fbo;
    glGenFramebuffers( 1, &fbo );

    glBindFramebuffer( GL_FRAMEBUFFER, fbo );

    for (uint32 i = 0; i < c.activeRts; ++i) {
        GLuint handle = c.rts[i]->handle;
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, handle, 0 );
    }
    
    if ( c.hasDepthStencil ) {
        GLuint handle = c.ds->handle;
        glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, handle, 0 );
    }

    GLuint result = glCheckFramebufferStatus( GL_FRAMEBUFFER );
    if ( result != GL_FRAMEBUFFER_COMPLETE ) {
        Assert( false && "Error creating framebuffer." );
    }

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    FrameBufferState fbs;
    fbs.activeRts = c.activeRts;
    fbs.handle = fbo;
    fbs.hasDepthStencil = c.hasDepthStencil;

    for ( uint32 i = 0; i < c.activeRts; ++i ) {
        fbs.rts[i] = c.rts[i];
    }

    fbs.clear = c.clear;
    fbs.clearColor[0] = c.clearColor[0];
    fbs.clearColor[1] = c.clearColor[1];
    fbs.clearColor[2] = c.clearColor[2];
    fbs.clearColor[3] = c.clearColor[3];
    fbs.isFrameBuffer = c.isFrameBuffer;

    _renderDb._fbos.push_back( fbs );

    return &_renderDb._fbos.back();
}

//////////////////////////////////////////////////////////////////////////

void RenderContext::WriteToGpu( VertexBufferHandle& vb, void* sourceData, uint32 size ) {

    glBindBuffer( GL_ARRAY_BUFFER, (GLuint)vb->handle );
    glBufferData( GL_ARRAY_BUFFER, size, sourceData, GL_DYNAMIC_DRAW );

}

void RenderContext::Set( FrameBufferState& renderTarget ) {
    glBindFramebuffer( GL_FRAMEBUFFER, renderTarget.isFrameBuffer ? 0 : renderTarget.handle );

    if ( renderTarget.clear ) {
        glClearColor( renderTarget.clearColor[0] / 256.0f, renderTarget.clearColor[1] / 256.0f, renderTarget.clearColor[2] / 256.0f, renderTarget.clearColor[3] / 256.0f );

        GLbitfield mask = GL_COLOR_BUFFER_BIT;
        mask |= renderTarget.hasDepthStencil ? GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT : 0;

        if ( renderTarget.hasDepthStencil ) {
            glEnable( GL_DEPTH_TEST );
            glClear( mask );
        }
        else {
            glDisable( GL_DEPTH_TEST );
            glClear( mask );
        }
    }
}

void RenderContext::Draw( DrawCall& drawCall ) {
    
    glBindBuffer( GL_ARRAY_BUFFER, (GLuint)drawCall._inputAssembler._vertexBuffer->handle );

    GLuint programHandle = drawCall._shaders->handle;
    glUseProgram( programHandle );
    GLuint modelLoc = glGetUniformLocation( programHandle, "viewProjection" );
    glUniformMatrix4fv( modelLoc, 1, GL_FALSE, glm::value_ptr( camera->_viewProj ) );

    // translate commands to render system commands.
    glBindVertexArray( drawCall._inputAssembler._layout->handle );
    glDrawArrays( GL_LINES, 0, drawCall._count );

    glBindVertexArray( 0 );
}


} // namespace hydra
} // namespace gfx

#endif // HY_OPENGL

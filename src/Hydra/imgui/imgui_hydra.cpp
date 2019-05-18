#include "imgui_hydra.h"

#include <Hydra/imgui/imgui.h>
#include <Hydra/Kernel/Lib.h>
#include <Hydra/Kernel/Input.h>
#include <Hydra/Renderer/Renderer.h>

#if defined(HY_OPENGL)
#include <GL/glew.h>
#include <GL/wglew.h>
#endif

namespace hydra {
    
//////////////////////////////////////////////////////////////////////////
// imgui porting to Hydra

// Data
//static GLFWwindow*  g_Window = NULL;
static hydra::Time  g_Time = time::Now();

#if defined(HY_OPENGL)
static GLuint       g_FontTexture = 0;
#endif

static int          g_ShaderHandle = 0;
static int          g_AttribLocationTex = 0, g_AttribLocationProjMtx = 0;
static unsigned int g_VboHandle = 0, g_VaoHandle = 0, g_ElementsHandle = 0;
static float        g_MouseWheelScrollScale = 1.f / 120.f;

//////////////////////////////////////////////////////////////////////////
gfx::ShaderStateHandle g_ShaderState;
gfx::TextureHandle g_Texture;

#if defined(HY_OPENGL)
// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplGlfwGL3_RenderDrawLists( ImDrawData* draw_data ) {

    // Backup GL state
    GLint last_program; glGetIntegerv( GL_CURRENT_PROGRAM, &last_program );
    GLint last_texture; glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
    GLint last_array_buffer; glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &last_array_buffer );
    GLint last_element_array_buffer; glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer );
    GLint last_vertex_array; glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &last_vertex_array );
    GLint last_blend_src; glGetIntegerv( GL_BLEND_SRC, &last_blend_src );
    GLint last_blend_dst; glGetIntegerv( GL_BLEND_DST, &last_blend_dst );
    GLint last_blend_equation_rgb; glGetIntegerv( GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb );
    GLint last_blend_equation_alpha; glGetIntegerv( GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha );
    GLint last_viewport[4]; glGetIntegerv( GL_VIEWPORT, last_viewport );
    GLboolean last_enable_blend = glIsEnabled( GL_BLEND );
    GLboolean last_enable_cull_face = glIsEnabled( GL_CULL_FACE );
    GLboolean last_enable_depth_test = glIsEnabled( GL_DEPTH_TEST );
    GLboolean last_enable_scissor_test = glIsEnabled( GL_SCISSOR_TEST );

    // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
    glEnable( GL_BLEND );
    glBlendEquation( GL_FUNC_ADD );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glDisable( GL_CULL_FACE );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_SCISSOR_TEST );
    glActiveTexture( GL_TEXTURE0 );

    // Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
    ImGuiIO& io = ImGui::GetIO();
    int fb_width = (int)( io.DisplaySize.x * io.DisplayFramebufferScale.x );
    int fb_height = (int)( io.DisplaySize.y * io.DisplayFramebufferScale.y );
    draw_data->ScaleClipRects( io.DisplayFramebufferScale );

    // Setup viewport, orthographic projection matrix
    glViewport( 0, 0, (GLsizei)fb_width, (GLsizei)fb_height );
    const float ortho_projection[4][4] =
    {
        { 2.0f / io.DisplaySize.x, 0.0f,                   0.0f, 0.0f },
        { 0.0f,                  2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
        { 0.0f,                  0.0f,                  -1.0f, 0.0f },
        { -1.0f,                  1.0f,                   0.0f, 1.0f },
    };
    glUseProgram( g_ShaderHandle );
    glUniform1i( g_AttribLocationTex, 0 );
    glUniformMatrix4fv( g_AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0] );
    glBindVertexArray( g_VaoHandle );

    for ( int n = 0; n < draw_data->CmdListsCount; n++ ) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawIdx* idx_buffer_offset = 0;

        glBindBuffer( GL_ARRAY_BUFFER, g_VboHandle );
        glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof( ImDrawVert ), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW );

        glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, g_ElementsHandle );
        glBufferData( GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof( ImDrawIdx ), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW );

        for ( const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++ ) {
            if ( pcmd->UserCallback ) {
                pcmd->UserCallback( cmd_list, pcmd );
            } else {
                glBindTexture( GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId );
                glScissor( (int)pcmd->ClipRect.x, (int)( fb_height - pcmd->ClipRect.w ), (int)( pcmd->ClipRect.z - pcmd->ClipRect.x ), (int)( pcmd->ClipRect.w - pcmd->ClipRect.y ) );
                glDrawElements( GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof( ImDrawIdx ) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset );
            }
            idx_buffer_offset += pcmd->ElemCount;
        }
    }

    // Restore modified GL state
    glUseProgram( last_program );
    glBindTexture( GL_TEXTURE_2D, last_texture );
    glBindBuffer( GL_ARRAY_BUFFER, last_array_buffer );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer );
    glBindVertexArray( last_vertex_array );
    glBlendEquationSeparate( last_blend_equation_rgb, last_blend_equation_alpha );
    glBlendFunc( last_blend_src, last_blend_dst );
    if ( last_enable_blend ) glEnable( GL_BLEND ); else glDisable( GL_BLEND );
    if ( last_enable_cull_face ) glEnable( GL_CULL_FACE ); else glDisable( GL_CULL_FACE );
    if ( last_enable_depth_test ) glEnable( GL_DEPTH_TEST ); else glDisable( GL_DEPTH_TEST );
    if ( last_enable_scissor_test ) glEnable( GL_SCISSOR_TEST ); else glDisable( GL_SCISSOR_TEST );
    glViewport( last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3] );
}

static const char* ImGui_ImplGlfwGL3_GetClipboardText( void* userData ) {
    return "";/// glfwGetClipboardString( g_Window );
}

static void ImGui_ImplGlfwGL3_SetClipboardText( void* userData, const char* text ) {
  //  glfwSetClipboardString( g_Window, text );
}

void ImGui_ImplGlfwGL3_ScrollCallback( void* userData, const window::callbacks::MouseWheelData& data ) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += (float)data.delta * g_MouseWheelScrollScale;
}

void ImGui_ImplGlfwGL3_KeyCallback( void* userData, const window::callbacks::KeyModifiedData& data ) {
    ImGuiIO& io = ImGui::GetIO();
    // TODO: add proper value.
    if ( data.keyDown )
        io.KeysDown[data.virtualKey] = true;
    else
        io.KeysDown[data.virtualKey] = false;
 
   io.KeyCtrl = io.KeysDown[hydra::input::KEY_LCONTROL] || io.KeysDown[hydra::input::KEY_RCONTROL];
   io.KeyShift = io.KeysDown[hydra::input::KEY_LSHIFT] || io.KeysDown[hydra::input::KEY_RSHIFT];
   io.KeyAlt = io.KeysDown[hydra::input::KEY_LALT] || io.KeysDown[hydra::input::KEY_RALT];
}

void ImGui_ImplGlfwGL3_CharCallback( void* userData, const window::callbacks::CharModifiedData& data ) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter( data.ansiChar );
}

void ImGui_ImplGlfwGL3_FocusCallback( void* userData, const window::callbacks::ChangeFocusData& data ) {
    ImGuiIO& io = ImGui::GetIO();

    for ( uint32 i = 0; i < 512; ++i ) {
        io.KeysDown[i] = false;
    }

    io.KeyAlt = io.KeyShift = io.KeyCtrl = false;
}


bool ImGui_ImplGlfwGL3_CreateFontsTexture( gfx::RenderDevice& rd ) {
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32( &pixels, &width, &height );   // Load as RGBA 32-bits for OpenGL3 demo because it is more likely to be compatible with user's existing shader.

    GLint last_texture;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );

    gfx::TextureCreation tc { pixels, (uint16)width, (uint16)height, 1, gfx::TextureFormat::R8G8B8A8_UNORM };
    g_Texture = rd.CreateTexture2D( tc );
    // LINEAR SAMPLER

    g_FontTexture = g_Texture->handle;

    // Store our identifier
    io.Fonts->TexID = (void*)(uintptr_t)( g_FontTexture );

    // Restore state
    glBindTexture( GL_TEXTURE_2D, last_texture );

    return true;
}

bool ImGui_ImplGlfwGL3_CreateDeviceObjects( gfx::RenderDevice& rd ) {
    // Backup GL state
    GLint last_texture, last_array_buffer, last_vertex_array;
    glGetIntegerv( GL_TEXTURE_BINDING_2D, &last_texture );
    glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &last_array_buffer );
    glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &last_vertex_array );

    const GLchar *vertex_shader =
        "#version 330\n"
        "uniform mat4 ProjMtx;\n"
        "layout(location=0) in vec2 Position;\n"
        "layout(location=1) in vec2 UV;\n"
        "layout(location=2) in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "	Frag_UV = UV;\n"
        "	Frag_Color = Color;\n"
        "	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    const GLchar* fragment_shader =
        "#version 330\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
        "}\n";

    gfx::ShaderStateCreation ssc { ( gfx::VertexProgramHandle ) - 1, ( gfx::FragmentProgramHandle ) - 1, vertex_shader, fragment_shader, "ImGui" };
    g_ShaderState = rd.CreateShaderState( ssc );

    g_ShaderHandle = g_ShaderState->handle;

    g_AttribLocationTex = glGetUniformLocation( g_ShaderHandle, "Texture" );
    g_AttribLocationProjMtx = glGetUniformLocation( g_ShaderHandle, "ProjMtx" );
    /* ImDrawVert
    ImVec2  pos;
    ImVec2  uv;
    ImU32   col;
    */

    gfx::VertexBufferCreation vbc = { nullptr, sizeof( ImDrawVert ), sizeof( ImDrawVert ), false };
    gfx::VertexBufferHandle vb = rd.CreateVertexBuffer( vbc );
    // create vl
    gfx::VertexLayoutElement vls[] = { { 0, 2, gfx::VertexDataType::Float, gfx::VertexUsage::Position, 0, gfx::VertexFrequency::Vertex },
                                       { 0, 2, gfx::VertexDataType::Float, gfx::VertexUsage::Texcoord, 0, gfx::VertexFrequency::Vertex },
                                       { 0, 4, gfx::VertexDataType::UByte, gfx::VertexUsage::Color, 0,    gfx::VertexFrequency::Vertex } };
    gfx::VertexLayoutCreation vlc = { vb, vls, ArrayLength( vls ) };
    gfx::VertexLayoutHandle vl = rd.CreateVertexLayout( vlc );

    g_VboHandle = vb->handle;
    g_VaoHandle = vl->handle;

    glGenBuffers( 1, &g_ElementsHandle );

    ImGui_ImplGlfwGL3_CreateFontsTexture( rd );

    // Restore modified GL state
    glBindTexture( GL_TEXTURE_2D, last_texture );
    glBindBuffer( GL_ARRAY_BUFFER, last_array_buffer );
    glBindVertexArray( last_vertex_array );

    return true;
}

void    ImGui_ImplGlfwGL3_InvalidateDeviceObjects() {
    if ( g_VaoHandle ) glDeleteVertexArrays( 1, &g_VaoHandle );
    if ( g_VboHandle ) glDeleteBuffers( 1, &g_VboHandle );
    if ( g_ElementsHandle ) glDeleteBuffers( 1, &g_ElementsHandle );
    g_VaoHandle = g_VboHandle = g_ElementsHandle = 0;

    // Destroy shader state
    //glDeleteProgram( g_ShaderHandle );
    //g_ShaderHandle = 0;

    if ( g_FontTexture ) {
        // Destroy textures
        //glDeleteTextures( 1, &g_FontTexture );

        ImGui::GetIO().Fonts->TexID = 0;
        g_FontTexture = 0;
    }
}
#endif // HY_OPENGL

void ImGuiInit( WindowSystem& window, gfx::RenderDevice& rd ) {

    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();
    io.KeyMap[ImGuiKey_Tab] = input::KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
    io.KeyMap[ImGuiKey_LeftArrow] = input::KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = input::KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = input::KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = input::KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = input::KEY_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = input::KEY_NEXT;
    io.KeyMap[ImGuiKey_Home] = input::KEY_HOME;
    io.KeyMap[ImGuiKey_End] = input::KEY_END;
    io.KeyMap[ImGuiKey_Delete] = input::KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = input::KEY_BACK;
    io.KeyMap[ImGuiKey_Enter] = input::KEY_RETURN;
    io.KeyMap[ImGuiKey_Escape] = input::KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = input::KEY_A;
    io.KeyMap[ImGuiKey_C] = input::KEY_C;
    io.KeyMap[ImGuiKey_V] = input::KEY_V;
    io.KeyMap[ImGuiKey_X] = input::KEY_X;
    io.KeyMap[ImGuiKey_Y] = input::KEY_Y;
    io.KeyMap[ImGuiKey_Z] = input::KEY_Z;

#if defined(HY_OPENGL)
    // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
    io.RenderDrawListsFn = ImGui_ImplGlfwGL3_RenderDrawLists;
    io.SetClipboardTextFn = ImGui_ImplGlfwGL3_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfwGL3_GetClipboardText;
#ifdef _WIN32
    io.ImeWindowHandle = window.handle;
#endif // _WIN32

#endif // HY_OPENGL
    window.AddKeyCallback( ImGui_ImplGlfwGL3_KeyCallback, nullptr );
    window.AddCharCallback( ImGui_ImplGlfwGL3_CharCallback, nullptr );
    window.AddFocusCallback( ImGui_ImplGlfwGL3_FocusCallback, nullptr );
    window.AddMouseWheelCallback( ImGui_ImplGlfwGL3_ScrollCallback, nullptr );
}

void ImGuiTerminate( gfx::RenderDevice& rd ) {
#if defined(HY_OPENGL)
    ImGui_ImplGlfwGL3_InvalidateDeviceObjects();
#endif
    ImGui::DestroyContext();
}

void ImGuiNewFrame( WindowSystem& window, input::InputSystem& i, gfx::RenderDevice& rd ) {
#if defined(HY_OPENGL)
    if ( !g_FontTexture )
        ImGui_ImplGlfwGL3_CreateDeviceObjects( rd );
#endif
    ImGuiIO& io = ImGui::GetIO();
    input::InputComponent& input = i._input;

    io.DisplaySize = ImVec2( (float)window.frameBufferWidth, (float)window.frameBufferHeight );
    io.DisplayFramebufferScale = ImVec2( 1.0f, 1.0f );

    // Setup time step
    Time current_time = hydra::time::Now();
    io.DeltaTime = (float)( time::IntervalMilliseconds( current_time, g_Time ) / 1000.0f) + 0.0000001f;
    g_Time = current_time;

    // Setup inputs that are not bound to callbacks.

    // Mouse position
    if ( window.flags.test(WindowSystem::Flags_inFocus) ) {
        io.MousePos = ImVec2( (float)input.mouse.lastX, (float)input.mouse.lastY );   // Mouse position in screen coordinates (set to -1,-1 if no mouse / on another screen, etc.)
    } else {
        io.MousePos = ImVec2( -1, -1 );
    }
    
    // Mouse button
    for ( int i = 0; i < 3; i++ ) {
        const bool buttonWasDown = input.mouse.IsButtonDown( ( input::MouseButtons::Buttons )i );
        io.MouseDown[i] = buttonWasDown;    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    }

    // Start the frame
    ImGui::NewFrame();
}

} // namespace hydra

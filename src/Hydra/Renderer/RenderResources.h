#pragma once

#include <Hydra/Kernel/Types.h>

namespace hydra {
namespace gfx {

    namespace ComparisonFunction {
        enum Enum {
            Never, Less, Equal, LessEqual, Greater, NotEqual, GreaterEqual, Always, max
        };

        static const char* sStrings[] = {
            "Never", "Less", "Equal", "LessEqual", "Greater", "NotEqual", "GreaterEqual", "Always", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace ComparisonFunction

    namespace ConservativeRasterization {
        enum Enum {
            Off, On, max
        };

        static const char* sStrings[] = {
            "Off", "On", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace ConservativeRasterization

    namespace CullFace {
        enum Enum {
            None, Back, Front, max
        };

        static const char* sStrings[] = {
            "None", "Back", "Front", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace CullFace

    namespace DepthWriteMask {
        enum Enum {
            Zero, All, max
        };

        static const char* sStrings[] = {
            "Zero", "All", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace DepthWriteMask

    namespace FillMode {
        enum Enum {
            Wireframe, Solid, max
        };

        static const char* sStrings[] = {
            "Wireframe", "Solid", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace FillMode

    namespace FrontClockwise {
        enum Enum {
            True, False, max
        };

        static const char* sStrings[] = {
            "True", "False", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace FrontClockwise

    namespace StencilOperation {
        enum Enum {
            Keep, Zero, Replace, IncrSat, DecrSat, Invert, Incr, Decr, max
        };

        static const char* sStrings[] = {
            "Keep", "Zero", "Replace", "IncrSat", "DecrSat", "Invert", "Incr", "Decr", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace StencilOperation

    namespace TextureFormat {
        enum Enum {
            UNKNOWN, R32G32B32A32_TYPELESS, R32G32B32A32_FLOAT, R32G32B32A32_UINT, R32G32B32A32_SINT, R32G32B32_TYPELESS, R32G32B32_FLOAT, R32G32B32_UINT, R32G32B32_SINT, R16G16B16A16_TYPELESS, R16G16B16A16_FLOAT, R16G16B16A16_UNORM, R16G16B16A16_UINT, R16G16B16A16_SNORM, R16G16B16A16_SINT, R32G32_TYPELESS, R32G32_FLOAT, R32G32_UINT, R32G32_SINT, R32G8X24_TYPELESS, D32_FLOAT_S8X24_UINT, R32_FLOAT_X8X24_TYPELESS, X32_TYPELESS_G8X24_UINT, R10G10B10A2_TYPELESS, R10G10B10A2_UNORM, R10G10B10A2_UINT, R11G11B10_FLOAT, R8G8B8A8_TYPELESS, R8G8B8A8_UNORM, R8G8B8A8_UNORM_SRGB, R8G8B8A8_UINT, R8G8B8A8_SNORM, R8G8B8A8_SINT, R16G16_TYPELESS, R16G16_FLOAT, R16G16_UNORM, R16G16_UINT, R16G16_SNORM, R16G16_SINT, R32_TYPELESS, D32_FLOAT, R32_FLOAT, R32_UINT, R32_SINT, R24G8_TYPELESS, D24_UNORM_S8_UINT, R24_UNORM_X8_TYPELESS, X24_TYPELESS_G8_UINT, R8G8_TYPELESS, R8G8_UNORM, R8G8_UINT, R8G8_SNORM, R8G8_SINT, R16_TYPELESS, R16_FLOAT, D16_UNORM, R16_UNORM, R16_UINT, R16_SNORM, R16_SINT, R8_TYPELESS, R8_UNORM, R8_UINT, R8_SNORM, R8_SINT, A8_UNORM, R1_UNORM, R9G9B9E5_SHAREDEXP, R8G8_B8G8_UNORM, G8R8_G8B8_UNORM, BC1_TYPELESS, BC1_UNORM, BC1_UNORM_SRGB, BC2_TYPELESS, BC2_UNORM, BC2_UNORM_SRGB, BC3_TYPELESS, BC3_UNORM, BC3_UNORM_SRGB, BC4_TYPELESS, BC4_UNORM, BC4_SNORM, BC5_TYPELESS, BC5_UNORM, BC5_SNORM, B5G6R5_UNORM, B5G5R5A1_UNORM, B8G8R8A8_UNORM, B8G8R8X8_UNORM, R10G10B10_XR_BIAS_A2_UNORM, B8G8R8A8_TYPELESS, B8G8R8A8_UNORM_SRGB, B8G8R8X8_TYPELESS, B8G8R8X8_UNORM_SRGB, BC6H_TYPELESS, BC6H_UF16, BC6H_SF16, BC7_TYPELESS, BC7_UNORM, BC7_UNORM_SRGB, FORCE_UINT, max
        };

        static const char* sStrings[] = {
            "UNKNOWN", "R32G32B32A32_TYPELESS", "R32G32B32A32_FLOAT", "R32G32B32A32_UINT", "R32G32B32A32_SINT", "R32G32B32_TYPELESS", "R32G32B32_FLOAT", "R32G32B32_UINT", "R32G32B32_SINT", "R16G16B16A16_TYPELESS", "R16G16B16A16_FLOAT", "R16G16B16A16_UNORM", "R16G16B16A16_UINT", "R16G16B16A16_SNORM", "R16G16B16A16_SINT", "R32G32_TYPELESS", "R32G32_FLOAT", "R32G32_UINT", "R32G32_SINT", "R32G8X24_TYPELESS", "D32_FLOAT_S8X24_UINT", "R32_FLOAT_X8X24_TYPELESS", "X32_TYPELESS_G8X24_UINT", "R10G10B10A2_TYPELESS", "R10G10B10A2_UNORM", "R10G10B10A2_UINT", "R11G11B10_FLOAT", "R8G8B8A8_TYPELESS", "R8G8B8A8_UNORM", "R8G8B8A8_UNORM_SRGB", "R8G8B8A8_UINT", "R8G8B8A8_SNORM", "R8G8B8A8_SINT", "R16G16_TYPELESS", "R16G16_FLOAT", "R16G16_UNORM", "R16G16_UINT", "R16G16_SNORM", "R16G16_SINT", "R32_TYPELESS", "D32_FLOAT", "R32_FLOAT", "R32_UINT", "R32_SINT", "R24G8_TYPELESS", "D24_UNORM_S8_UINT", "R24_UNORM_X8_TYPELESS", "X24_TYPELESS_G8_UINT", "R8G8_TYPELESS", "R8G8_UNORM", "R8G8_UINT", "R8G8_SNORM", "R8G8_SINT", "R16_TYPELESS", "R16_FLOAT", "D16_UNORM", "R16_UNORM", "R16_UINT", "R16_SNORM", "R16_SINT", "R8_TYPELESS", "R8_UNORM", "R8_UINT", "R8_SNORM", "R8_SINT", "A8_UNORM", "R1_UNORM", "R9G9B9E5_SHAREDEXP", "R8G8_B8G8_UNORM", "G8R8_G8B8_UNORM", "BC1_TYPELESS", "BC1_UNORM", "BC1_UNORM_SRGB", "BC2_TYPELESS", "BC2_UNORM", "BC2_UNORM_SRGB", "BC3_TYPELESS", "BC3_UNORM", "BC3_UNORM_SRGB", "BC4_TYPELESS", "BC4_UNORM", "BC4_SNORM", "BC5_TYPELESS", "BC5_UNORM", "BC5_SNORM", "B5G6R5_UNORM", "B5G5R5A1_UNORM", "B8G8R8A8_UNORM", "B8G8R8X8_UNORM", "R10G10B10_XR_BIAS_A2_UNORM", "B8G8R8A8_TYPELESS", "B8G8R8A8_UNORM_SRGB", "B8G8R8X8_TYPELESS", "B8G8R8X8_UNORM_SRGB", "BC6H_TYPELESS", "BC6H_UF16", "BC6H_SF16", "BC7_TYPELESS", "BC7_UNORM", "BC7_UNORM_SRGB", "FORCE_UINT", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace TextureFormat

    namespace VertexDataType {
        enum Enum {
            Float, UByte, Byte, UShort, Short, UInt, Int, max
        };

        static const char* sStrings[] = {
            "Float", "UByte", "Byte", "UShort", "Short", "UInt", "Int", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace VertexDataType

    namespace VertexUsage {
        enum Enum {
            Position, Texcoord, Normal, Color, max
        };

        static const char* sStrings[] = {
            "Position", "Texcoord", "Normal", "Color", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace VertexUsage

    namespace VertexFrequency {
        enum Enum {
            Vertex, Instance, max
        };

        static const char* sStrings[] = {
            "Vertex", "Instance", "max"
        };

        static const char* ToString( Enum e ) {
            return sStrings[(int)e];
        }
    } // namespace VertexFrequency


    struct VertexLayoutElement {
        int stream;
        int size;
        VertexDataType::Enum type;
        VertexUsage::Enum usage;
        int usageIndex;
        VertexFrequency::Enum frequency;
    }; // struct VertexLayoutElement


    typedef struct VertexBuffer* VertexBufferHandle;
    typedef struct IndexBuffer* IndexBufferHandle;
    typedef struct ConstantBuffer* ConstantBufferHandle;
    
    typedef struct VertexBinding* VertexBindingHandle;
    typedef struct VertexLayout* VertexLayoutHandle;

    typedef struct VertexProgram* VertexProgramHandle;
    typedef struct FragmentProgram* FragmentProgramHandle;
    typedef struct ComputeProgram* ComputeProgramHandle;

    typedef struct Texture* TextureHandle;
    typedef struct Sampler* SamplerHandle;

    typedef struct VertexInputState* VertexInputStateHandle;
    typedef struct ShaderState* ShaderStateHandle;
    typedef struct FrameBufferState* FrameBufferStateHandle;


    struct ShaderCreation {
        cstring code;
        cstring name;
    }; // struct VertexProgramCreation

    struct VertexBufferCreation {
        const void*                         data;
        uint32                              size;
        uint32                              stride;
        bool                                isStatic;
    };

    struct VertexLayoutCreation {
        VertexBufferHandle                  vb;
        const VertexLayoutElement*          elements;
        uint32                              elementCount;
    };

    struct ShaderStateCreation {
        VertexProgramHandle                 vp;
        FragmentProgramHandle               fp;

        cstring                             vpSource;
        cstring                             fpSource;

        cstring                             name;
    };

    struct TextureCreation {
        const void*                         data;
        uint16                              width;
        uint16                              height;
        uint16                              depth;
        TextureFormat::Enum                 format;
    };

    struct SamplerCreation {

    };

    static const uint32 kMaxRenderTargets = 8;

    struct FrameBufferCreation {
        TextureHandle                       rts[kMaxRenderTargets];
        TextureHandle                       ds;             // depth stencil
        uint8                               activeRts;
        uint8                               clearColor[4];
        uint8                               hasDepthStencil : 1;
        uint8                               clear : 1;
        uint8                               isFrameBuffer : 1;

        void setColor( uint8 r, uint8 g, uint8 b, uint8 a ) { clearColor[0] = r; clearColor[1] = g; clearColor[2] = b; clearColor[3] = a; }
    };

// resources creation params
// resources attributes
   
} // namespace hydra
} // namespace gfx


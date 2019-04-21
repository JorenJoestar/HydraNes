#pragma once

#include <stdint.h>

namespace hydra {

    //////////////////////////////////////////////////////////////////////////
    // Native types
    typedef uint8_t                     uint8;
    typedef int8_t                      int8;

    typedef uint16_t                    uint16;
    typedef int16_t                     int16;

    typedef uint32_t                    uint32;
    typedef int32_t                     int32;

    typedef uint64_t                    uint64;
    typedef int64_t                     int64;

    // Memory types
    typedef size_t                      msize;
    typedef void*                       voidp;
    typedef uintptr_t                   uintptr;

    typedef const char*                 cstring;

    //////////////////////////////////////////////////////////////////////////
    // Constants
    static const uint32                 kFloatPosInfHex = 0x7f800000;
    static const uint32                 kFloatNegInfHex = 0xff800000;
    static const uint32                 kFloatNanHex = 0xffc00000;

    //////////////////////////////////////////////////////////////////////////
    // 
    #define Kilo(size) (size * 1024)
    #define Mega(size) (size * 1024 * 1024)
    #define Giga(size) (size * 1024 * 1024 * 1024)

} // namespace hydra 
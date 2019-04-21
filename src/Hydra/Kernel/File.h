#pragma once

#include "hydra/kernel/DataStructures.h"

#if defined(_WIN64)
#include <Windows.h>
#endif

namespace hydra {

#if defined(_WIN64)
    using FileTime = FILETIME;
#endif

    using FileHandle = FILE*;

    void                    ReadFileIntoMemory( cstring filename, cstring mode, Buffer& memory );
    void                    ReadTextFileIntoMemory( cstring filename, cstring mode, String& string );          // Reads content and null terminate the string.

    void                    OpenFile( cstring filename, cstring mode, FileHandle* file );
    void                    CloseFile( FileHandle file );
    int32                   ReadFile( uint8* memory, uint32 elementSize, uint32 count, FileHandle file );

    void                    FindFilesInPath( cstring pattern, StdArray<String>& files );
    void                    FindFilesInPath( cstring extension, cstring searchPath, StdArray<String>& files, StdArray<String>& directories );

    FileTime                GetLastWriteTime( cstring filename );

    struct ScopedFile {
        ScopedFile( cstring filename, cstring mode );
        ~ScopedFile();

        FileHandle      _file;
    };
} // namespace hydra
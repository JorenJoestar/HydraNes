#include "Hydra/Kernel/File.h"

namespace hydra {
    
//////////////////////////////////////////////////////////////////////////
void OpenFile( cstring filename, cstring mode, FileHandle* file ) {
    fopen_s( file, filename, mode );
}

void CloseFile( FileHandle file ) {
    if ( file )
        fclose( file );
}

int32 ReadFile( uint8* memory, uint32 elementSize, uint32 count, FileHandle file ) {
    int32 bytesRead = (int32)fread( memory, elementSize, count, file );
    return bytesRead;
}

static long GetFileSize( FileHandle f ) {
    long fileSizeSigned;

    fseek( f, 0, SEEK_END );
    fileSizeSigned = ftell( f );
    fseek( f, 0, SEEK_SET );

    return fileSizeSigned;
}

void ReadFileIntoMemory( cstring filename, cstring mode, Buffer& memory ) {
    FileHandle f;
    OpenFile( filename, mode, &f );
    if ( !f )
        return;

    long fileSizeSigned = GetFileSize( f );

    memory.resize( fileSizeSigned );

    msize readsize = fread( memory.data, 1, fileSizeSigned, f );
    fclose( f );
}

void ReadTextFileIntoMemory( cstring filename, cstring mode, String& string ) {
    Assert( false && "not implemented" );
    //FileHandle f;
    //OpenFile( filename, mode, &f );
    //if ( !f )
    //    return;

    //long fileSizeSigned = GetFileSize( f );

    //string.resize( fileSizeSigned + 1 );
    //
    //
    ////msize readsize = fread( string.data, 1, fileSizeSigned, f );
    //fclose( f );

    //string[readsize] = 0;
}

FileTime GetLastWriteTime( cstring filename ) {
#if defined(_WIN64)
    FILETIME lastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA data;
    if ( GetFileAttributesExA( filename, GetFileExInfoStandard, &data ) ) {
        lastWriteTime = data.ftLastWriteTime;
    }

    return lastWriteTime;
#else
    FileTime time = 0;
    return time;
#endif
}

void FindFilesInPath( cstring name, StdArray<String>& files ) {

    std::string pattern( name );
    pattern.append( "\\*" );

    WIN32_FIND_DATAA data;
    HANDLE hFind;
    if ( (hFind = FindFirstFileA( pattern.c_str(), &data )) != INVALID_HANDLE_VALUE ) {
        do {
            files.push_back( data.cFileName );
        } while ( FindNextFileA( hFind, &data ) != 0 );
        FindClose( hFind );
    }
}

void FindFilesInPath( cstring extension, cstring searchPath, StdArray<String>& files, StdArray<String>& directories ) {  

    std::string pattern( searchPath );
    pattern.append( "\\*" );

    WIN32_FIND_DATAA data;
    HANDLE hFind;
    if ( (hFind = FindFirstFileA( pattern.c_str(), &data )) != INVALID_HANDLE_VALUE ) {
        do {
            if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) {
                directories.push_back( data.cFileName );
            } else {
                if ( strstr( data.cFileName, extension ) ) {
                    files.push_back( data.cFileName );
                }
            }

        } while ( FindNextFileA( hFind, &data ) != 0 );
        FindClose( hFind );
    }
}

ScopedFile::ScopedFile( cstring filename, cstring mode ) {
    OpenFile( filename, mode, &_file );
}

ScopedFile::~ScopedFile() {
    CloseFile( _file );
}

}
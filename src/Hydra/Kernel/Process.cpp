#include "Hydra/Kernel/Process.h"
#include "Hydra/Kernel/Lib.h"

namespace hydra {

StaticArray<char, 256> s_LogBuffer;

void Win32GetError( char* buffer, uint32 size ) {
    DWORD errorCode = GetLastError();

    char* errorString;
    if ( !FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER,
        NULL, errorCode, MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ), (LPSTR)&errorString, 0, NULL ) )
        return;

    sprintf_s( buffer, size, "%s", errorString );

    LocalFree( errorString );
}

bool ExecuteProcess( cstring workingDirectory, cstring processFullpath, cstring arguments ) {

    STARTUPINFOA startupInfo = {};
    startupInfo.cb = sizeof( startupInfo );
    startupInfo.dwFlags = STARTF_USESHOWWINDOW;
    startupInfo.wShowWindow = SW_SHOW;

    PROCESS_INFORMATION processInfo = {};
    if ( CreateProcessA( processFullpath, (char*)arguments, 0, 0, FALSE, 0, 0, workingDirectory, &startupInfo, &processInfo ) ) {
        // Blocking version.
        WaitForSingleObject( processInfo.hProcess, INFINITE );
        CloseHandle( processInfo.hThread );
        CloseHandle( processInfo.hProcess );

        return true;
    } else {
        Win32GetError( s_LogBuffer.begin(), 256 );
        PrintFormat( "Error Compiling: %s\n", s_LogBuffer.data );

        return false;
    }
}

} // namespace hydra
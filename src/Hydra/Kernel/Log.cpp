#include "Hydra/Kernel/Log.h"
#include "hydra/Kernel/Lib.h"

namespace hydra {
    //////////////////////////////////////////////////////////////////////////
static const msize kStringBufferMax = 1024 * 1024;
static char sLogBuffer[kStringBufferMax];

#define HY_ENABLE_CONSOLE
#define HY_ENABLE_MSVS_OUTPUT

//////////////////////////////////////////////////////////////////////////
static void PrintVaList( const char* format, va_list args ) {
    vsnprintf_s( sLogBuffer, ArrayLength( sLogBuffer ), format, args );
    sLogBuffer[ArrayLength( sLogBuffer ) - 1] = '\0';
}

static void OutputConsole() {
#if defined (HY_ENABLE_CONSOLE)
    printf( "%s", sLogBuffer );
#endif
}

static void OutputVisualStudio() {
#if defined (HY_ENABLE_MSVS_OUTPUT)
    OutputDebugStringA( sLogBuffer );
#endif
}

void PrintFormat( const char* format, ... ) {
    va_list args;
    va_start( args, format );
    PrintVaList( format, args );
    va_end( args );

    OutputConsole();
    OutputVisualStudio();
}

void PrintFormatConsole( const char* format, ... ) {
    va_list args;
    va_start( args, format );
    PrintVaList( format, args );
    va_end( args );

    OutputConsole();
}

#if defined(_MSC_VER)

void PrintFormatVisualStudio( const char* format, ... ) {
    va_list args;
    va_start( args, format );
    PrintVaList( format, args );
    va_end( args );

    OutputVisualStudio();
}

#endif // _MSC_VER

}
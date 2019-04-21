#pragma once

#include "hydra/kernel/types.h"

namespace hydra {

    void PrintFormat( cstring format, ... );
    void PrintFormatConsole( cstring format, ... );
#if defined(_MSC_VER)
    void PrintFormatVisualStudio( cstring format, ... );
#endif

} // namespace hydra
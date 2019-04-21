#pragma once

#include <Hydra/Kernel/Types.h>

// Function
#include <functional>

namespace hydra {
    // Define generic callback
    using AssertCallbackDelegate = bool( *)( cstring, uint32, cstring );
    extern AssertCallbackDelegate AssertCallbackDelegateImpl;
    // Provide one implementation
    bool AssertCallback( cstring filename, uint32 line, cstring cond );

    //#define Capture(var) PrintFormat("Var )

    #define HY_ASSERT_ENABLED

        // Assert blocks the execution of the program, check just output the failure.
    #if defined(HY_ASSERT_ENABLED)
    #define Assert(cond) if(!(cond)) { AssertCallbackDelegateImpl(__FILE__, __LINE__, #cond); __debugbreak(); }
    #define Check(cond) if(!(cond)) { AssertCallbackDelegateImpl(__FILE__, __LINE__, #cond); }
    #else
    #define Assert(cond)
    #define Check(cond)
    #endif

        // Used for heavy debugging: pre and post conditions of a method/function.
    #define HY_ASSERT_PREPOST

    #if defined(HY_ASSERT_PREPOST)
    #define Expects(cond)           Assert(cond);
    #define Ensures(cond)           Assert(cond);
    #else
    #define Expects(cond)
    #define Ensures(cond)
    #endif

} // namespace hydra

#pragma once

#include <chrono>
#include "Hydra/Kernel/Types.h"

namespace hydra {

    using Time = std::chrono::steady_clock::time_point;
    using TimeStamp = int64;
    using Seconds = std::chrono::seconds;
    using Milliseconds = std::chrono::milliseconds;
    using Microseconds = std::chrono::microseconds;

    namespace time {

        // https://kjellkod.wordpress.com/2012/02/06/exploring-c11-part-1-time/

        inline Time Now() { return std::chrono::high_resolution_clock::now(); }

        inline TimeStamp IntervalSeconds( Time end, Time start ) { return std::chrono::duration_cast<std::chrono::seconds>( end - start ).count(); }
        inline TimeStamp IntervalMilliseconds( Time end, Time start ) { return std::chrono::duration_cast<std::chrono::milliseconds>( end - start ).count(); }
        inline TimeStamp IntervalMicroseconds( Time end, Time start ) { return std::chrono::duration_cast<std::chrono::microseconds>( end - start ).count(); }

        struct StopWatch {
            void Start() { _start = Now(); }
            void Reset() { _start = Now(); }

            TimeStamp                       ElapsedMilliseconds() { return IntervalMilliseconds( Now(), _start ); }
            TimeStamp                       ElapsedMicroseconds() { return IntervalMicroseconds( Now(), _start ); }

            Time _start;
        };

        // Scope a time and write out to a variable.
        template <typename t>
        struct ScopedTimer {

            ScopedTimer( int64* elapsed ) : elapsed( elapsed ) { _clock.Start(); }
            ~ScopedTimer() {
                *_elapsed = std::chrono::duration_cast<t>( Now() - _clock._start ).count();
            }

            StopWatch           _clock;
            int64*              _elapsed;
        };

        using ScopedTimerSeconds = ScopedTimer<Seconds>;
        using ScopedTimerMilliseconds = ScopedTimer<Milliseconds>;
        using ScopedTimerMicroseconds = ScopedTimer<Microseconds>;
    }

} // namespace hydra
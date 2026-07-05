#include "statistics_console.hpp"

namespace statistics_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Statistics console (Data, statistics, and ML). Awaits wire-up.";
    return s;
}

}

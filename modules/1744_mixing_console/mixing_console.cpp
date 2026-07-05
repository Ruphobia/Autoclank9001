#include "mixing_console.hpp"

namespace mixing_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Mixing console (Signal processing and audio). Awaits wire-up.";
    return s;
}

}

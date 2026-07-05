#include "midi_router_and_monitor.hpp"

namespace midi_router_and_monitor {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: MIDI router and monitor (Signal processing and audio). Awaits wire-up.";
    return s;
}

}

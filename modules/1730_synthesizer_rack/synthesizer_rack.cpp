#include "synthesizer_rack.hpp"

namespace synthesizer_rack {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Synthesizer rack (Signal processing and audio). Awaits wire-up.";
    return s;
}

}

#include "wave_spectrum_analyzer.hpp"

namespace wave_spectrum_analyzer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wave spectrum analyzer (Oceanography and marine science). Awaits wire-up.";
    return s;
}

}

#include "spectrum_analyzer_console.hpp"

namespace spectrum_analyzer_console {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrum analyzer console (Aviation, amateur radio, and RF). Awaits wire-up.";
    return s;
}

}

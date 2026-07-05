#include "spectrum_viewer.hpp"

namespace spectrum_viewer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Spectrum viewer (Chemistry and lab). Awaits wire-up.";
    return s;
}

}

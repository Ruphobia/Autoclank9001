#include "octave_band_spectrum_tool.hpp"

namespace octave_band_spectrum_tool {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Octave band spectrum tool (Acoustic, thermal, hydraulic, pneumatic engineering). Awaits wire-up.";
    return s;
}

}

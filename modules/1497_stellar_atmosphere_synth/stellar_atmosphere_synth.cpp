#include "stellar_atmosphere_synth.hpp"

namespace stellar_atmosphere_synth {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Stellar Atmosphere Synth (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}

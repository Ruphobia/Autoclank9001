#include "vibrational_spectrum_simulator.hpp"

namespace vibrational_spectrum_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Vibrational spectrum simulator (Chemistry and lab). Awaits wire-up.";
    return s;
}

}

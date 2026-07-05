#include "nmr_simulator.hpp"

namespace nmr_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: NMR simulator (Chemistry and lab). Awaits wire-up.";
    return s;
}

}

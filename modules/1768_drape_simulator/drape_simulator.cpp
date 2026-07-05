#include "drape_simulator.hpp"

namespace drape_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Drape Simulator (Textiles, sewing, soft crafts). Awaits wire-up.";
    return s;
}

}

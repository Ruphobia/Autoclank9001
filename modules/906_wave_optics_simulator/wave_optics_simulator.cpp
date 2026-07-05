#include "wave_optics_simulator.hpp"

namespace wave_optics_simulator {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Wave optics simulator (Physics and math). Awaits wire-up.";
    return s;
}

}

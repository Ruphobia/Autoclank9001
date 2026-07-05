#include "granular_and_concatenative_synthesis_lab.hpp"

namespace granular_and_concatenative_synthesis_lab {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Granular and concatenative synthesis lab (Signal processing and audio). Awaits wire-up.";
    return s;
}

}

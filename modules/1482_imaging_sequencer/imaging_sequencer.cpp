#include "imaging_sequencer.hpp"

namespace imaging_sequencer {

void init()     {}
void shutdown() {}

Status status() {
    Status s;
    s.ready  = false;
    s.detail = "stub: Imaging Sequencer (Astronomy and astrophysics). Awaits wire-up.";
    return s;
}

}
